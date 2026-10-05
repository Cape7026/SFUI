#pragma once

#include <SFML/Graphics.hpp>
#include <Windows.h>
#include <algorithm>
#include <cstdint>
#include <string>
#include <utility>

namespace SFUI
{

    class WindowMod
    {
    private:
        static void focusWindow(HWND target)
        {
            if (IsIconic(target))
                ShowWindow(target, SW_RESTORE);

            HWND fg = GetForegroundWindow();
            DWORD fgThread = fg ? GetWindowThreadProcessId(fg, nullptr) : 0;
            DWORD thisThread = GetCurrentThreadId();
            bool attached = fgThread && fgThread != thisThread &&
                            AttachThreadInput(thisThread, fgThread, TRUE);

            BringWindowToTop(target);
            SetForegroundWindow(target);

            if (attached)
                AttachThreadInput(thisThread, fgThread, FALSE);
        }

        static HWND findWindowOfProcess(DWORD pid)
        {
            struct Search
            {
                DWORD pid;
                HWND found;
            } search{pid, nullptr};

            EnumWindows(
                [](HWND window, LPARAM lParam) -> BOOL
                {
                    auto &s = *reinterpret_cast<Search *>(lParam);

                    if (!IsWindowVisible(window) || GetWindow(window, GW_OWNER))
                        return TRUE;

                    DWORD windowPid = 0;
                    GetWindowThreadProcessId(window, &windowPid);
                    if (windowPid != s.pid)
                        return TRUE;

                    s.found = window;
                    return FALSE;
                },
                reinterpret_cast<LPARAM>(&search));

            return search.found;
        }

        static void leaveFullscreen(HWND hwnd)
        {
            if (!fullscreen)
                return;

            internalChange = true;

            LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
            SetWindowLongPtrW(hwnd, GWL_STYLE, style | WS_OVERLAPPEDWINDOW);
            SetWindowPlacement(hwnd, &prevPlacement);
            SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                             SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

            internalChange = false;
            fullscreen = false;
        }

        inline static WINDOWPLACEMENT prevPlacement = {sizeof(WINDOWPLACEMENT)};
        inline static bool fullscreen = false;
        inline static bool internalChange = false;
        inline static bool lastKeyWasRepeat = false;

    public:
        static void ensureSingleInstance()
        {
            wchar_t selfPath[MAX_PATH]{};
            GetModuleFileNameW(nullptr, selfPath, MAX_PATH);

            std::wstring name = L"Local\\SingleInstance_";
            for (const wchar_t *c = selfPath; *c; ++c)
                name += (*c == L'\\' || *c == L'/' || *c == L':') ? L'_' : *c;

            // Handle and view are never closed so the object lives as long as this process
            HANDLE map = CreateFileMappingW(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE,
                                            0, sizeof(std::uint64_t), name.c_str());
            if (!map)
                return;

            const bool alreadyRunning = GetLastError() == ERROR_ALREADY_EXISTS;

            void *view = MapViewOfFile(map, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(std::uint64_t));
            if (!view)
                return;

            volatile std::uint64_t *slot = static_cast<volatile std::uint64_t *>(view);

            if (!alreadyRunning)
            {
                *slot = GetCurrentProcessId();
                return;
            }

            // First instance may still be starting up, so retry for a bit
            HWND target = nullptr;
            for (int i = 0; i < 200 && !target; ++i)
            {
                const DWORD pid = static_cast<DWORD>(*slot);
                if (pid)
                    target = findWindowOfProcess(pid);

                if (!target)
                    Sleep(10);
            }

            if (target)
                focusWindow(target);

            ExitProcess(0);
        }

        static void toggleFullscreen(sf::RenderWindow &window)
        {
            // Subclass is needed to read the key repeat flag
            install(window);

            if (lastKeyWasRepeat)
                return;

            HWND hwnd = window.getNativeHandle();

            if (fullscreen)
            {
                leaveFullscreen(hwnd);
                return;
            }

            MONITORINFO mi = {sizeof(MONITORINFO)};
            HMONITOR mon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);

            if (GetWindowPlacement(hwnd, &prevPlacement) && GetMonitorInfoW(mon, &mi))
            {
                LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);

                internalChange = true;

                SetWindowLongPtrW(hwnd, GWL_STYLE, style & ~WS_OVERLAPPEDWINDOW);
                SetWindowPos(hwnd, HWND_TOP,
                             mi.rcMonitor.left, mi.rcMonitor.top,
                             mi.rcMonitor.right - mi.rcMonitor.left,
                             mi.rcMonitor.bottom - mi.rcMonitor.top,
                             SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

                internalChange = false;
                fullscreen = true;
            }
        }

        static bool resizeRenderFix(sf::RenderWindow &window)
        {
            if (!install(window))
                return false;

            getState().resizeEnabled = true;
            return true;
        }

    private:
        struct State
        {
            HWND hwnd = nullptr;
            WNDPROC originalWndProc = nullptr;
            HHOOK messageHook = nullptr;

            bool installed = false;
            bool resizeEnabled = false;
            bool resizing = false;

            WPARAM hit = 0;

            POINT capturePoint{};
            RECT resizeRect{};
            RECT originalRect{};

            LONG minWidth = 1;
            LONG minHeight = 1;
            LONG maxWidth = 50000;
            LONG maxHeight = 50000;

            ~State()
            {
                if (installed)
                {
                    finishResize(false);

                    if (messageHook)
                        UnhookWindowsHookEx(messageHook);

                    if (hwnd && IsWindow(hwnd))
                    {
                        SetWindowLongPtrW(
                            hwnd,
                            GWLP_WNDPROC,
                            reinterpret_cast<LONG_PTR>(
                                originalWndProc));

                        RemovePropW(
                            hwnd,
                            propertyName());
                    }
                }
            }

            void finishResize(bool restore)
            {
                if (!resizing)
                    return;

                if (restore)
                {
                    SetWindowPos(
                        hwnd,
                        nullptr,
                        originalRect.left,
                        originalRect.top,
                        originalRect.right - originalRect.left,
                        originalRect.bottom - originalRect.top,
                        SWP_NOZORDER |
                            SWP_NOACTIVATE |
                            SWP_NOOWNERZORDER);
                }

                if (GetCapture() == hwnd)
                    ReleaseCapture();

                resizing = false;
                hit = 0;
            }
        };

        static State &getState()
        {
            static State state;
            return state;
        }

        static constexpr LPCWSTR propertyName()
        {
            return L"SFUI_WindowMod";
        }

        static bool install(sf::RenderWindow &window)
        {
            State &state = getState();

            if (state.installed)
                return true;

            state.hwnd = window.getNativeHandle();

            if (!state.hwnd)
                return false;

            SetPropW(
                state.hwnd,
                propertyName(),
                &state);

            SetLastError(0);

            state.originalWndProc =
                reinterpret_cast<WNDPROC>(
                    SetWindowLongPtrW(
                        state.hwnd,
                        GWLP_WNDPROC,
                        reinterpret_cast<LONG_PTR>(
                            &WindowMod::wndProc)));

            if (!state.originalWndProc && GetLastError() != 0)
            {
                RemovePropW(
                    state.hwnd,
                    propertyName());

                state.hwnd = nullptr;
                return false;
            }

            state.messageHook = SetWindowsHookExW(
                WH_GETMESSAGE,
                &WindowMod::getMessageProc,
                nullptr,
                GetCurrentThreadId());

            if (!state.messageHook)
            {
                SetWindowLongPtrW(
                    state.hwnd,
                    GWLP_WNDPROC,
                    reinterpret_cast<LONG_PTR>(
                        state.originalWndProc));

                RemovePropW(
                    state.hwnd,
                    propertyName());

                state.hwnd = nullptr;
                state.originalWndProc = nullptr;

                return false;
            }

            state.installed = true;
            return true;
        }

        static bool isResizeHit(WPARAM hit)
        {
            switch (hit)
            {
            case HTLEFT:
            case HTRIGHT:
            case HTTOP:
            case HTBOTTOM:
            case HTTOPLEFT:
            case HTTOPRIGHT:
            case HTBOTTOMLEFT:
            case HTBOTTOMRIGHT:
                return true;

            default:
                return false;
            }
        }

        static bool hasLeft(WPARAM hit)
        {
            return hit == HTLEFT ||
                   hit == HTTOPLEFT ||
                   hit == HTBOTTOMLEFT;
        }

        static bool hasRight(WPARAM hit)
        {
            return hit == HTRIGHT ||
                   hit == HTTOPRIGHT ||
                   hit == HTBOTTOMRIGHT;
        }

        static bool hasTop(WPARAM hit)
        {
            return hit == HTTOP ||
                   hit == HTTOPLEFT ||
                   hit == HTTOPRIGHT;
        }

        static bool hasBottom(WPARAM hit)
        {
            return hit == HTBOTTOM ||
                   hit == HTBOTTOMLEFT ||
                   hit == HTBOTTOMRIGHT;
        }

        static HCURSOR cursorForHit(WPARAM hit)
        {
            switch (hit)
            {
            case HTLEFT:
            case HTRIGHT:
                return LoadCursorW(
                    nullptr,
                    reinterpret_cast<LPCWSTR>(IDC_SIZEWE));

            case HTTOP:
            case HTBOTTOM:
                return LoadCursorW(
                    nullptr,
                    reinterpret_cast<LPCWSTR>(IDC_SIZENS));

            case HTTOPLEFT:
            case HTBOTTOMRIGHT:
                return LoadCursorW(
                    nullptr,
                    reinterpret_cast<LPCWSTR>(IDC_SIZENWSE));

            case HTTOPRIGHT:
            case HTBOTTOMLEFT:
                return LoadCursorW(
                    nullptr,
                    reinterpret_cast<LPCWSTR>(IDC_SIZENESW));

            default:
                return LoadCursorW(
                    nullptr,
                    reinterpret_cast<LPCWSTR>(IDC_ARROW));
            }
        }

        static WPARAM systemCommandToHit(WPARAM command)
        {
            switch (command & 0x000F)
            {
            case 1:
                return HTLEFT;
            case 2:
                return HTRIGHT;
            case 3:
                return HTTOP;
            case 4:
                return HTTOPLEFT;
            case 5:
                return HTTOPRIGHT;
            case 6:
                return HTBOTTOM;
            case 7:
                return HTBOTTOMLEFT;
            case 8:
                return HTBOTTOMRIGHT;
            default:
                return 0;
            }
        }

        static bool beginResize(
            State &state,
            WPARAM hit,
            const POINT *initialPoint = nullptr)
        {
            if (state.resizing)
                return true;

            if (!isResizeHit(hit))
                return false;

            if (IsZoomed(state.hwnd) || IsIconic(state.hwnd))
                return false;

            if (!GetWindowRect(state.hwnd, &state.resizeRect))
                return false;

            if (initialPoint)
            {
                state.capturePoint = *initialPoint;
            }
            else if (!GetCursorPos(&state.capturePoint))
            {
                return false;
            }

            MINMAXINFO minMax{};

            minMax.ptMinTrackSize.x =
                GetSystemMetrics(SM_CXMINTRACK);

            minMax.ptMinTrackSize.y =
                GetSystemMetrics(SM_CYMINTRACK);

            minMax.ptMaxTrackSize.x =
                GetSystemMetrics(SM_CXMAXTRACK);

            minMax.ptMaxTrackSize.y =
                GetSystemMetrics(SM_CYMAXTRACK);

            CallWindowProcW(
                state.originalWndProc,
                state.hwnd,
                WM_GETMINMAXINFO,
                0,
                reinterpret_cast<LPARAM>(&minMax));

            state.minWidth =
                std::max<LONG>(1, minMax.ptMinTrackSize.x);

            state.minHeight =
                std::max<LONG>(1, minMax.ptMinTrackSize.y);

            state.maxWidth =
                std::max<LONG>(
                    state.minWidth,
                    minMax.ptMaxTrackSize.x);

            state.maxHeight =
                std::max<LONG>(
                    state.minHeight,
                    minMax.ptMaxTrackSize.y);

            state.hit = hit;
            state.resizing = true;

            if (!SetCapture(state.hwnd))
            {
                state.resizing = false;
                return false;
            }

            SetCursor(cursorForHit(state.hit));

            return true;
        }

        static void applyResize(
            State &state,
            const POINT &mousePoint)
        {
            if (!state.resizing)
                return;

            LONG dx =
                mousePoint.x - state.capturePoint.x;

            LONG dy =
                mousePoint.y - state.capturePoint.y;

            if (dx == 0 && dy == 0)
                return;

            RECT rect = state.resizeRect;
            POINT adjustedPoint = mousePoint;
            WPARAM sizingEdge = 0;

            if (hasLeft(state.hit))
            {
                const LONG maxLeft =
                    rect.right - state.maxWidth;

                const LONG minLeft =
                    rect.right - state.minWidth;

                rect.left = std::clamp(
                    rect.left + dx,
                    maxLeft,
                    minLeft);

                adjustedPoint.x = rect.left;
                sizingEdge = WMSZ_LEFT;
            }
            else if (hasRight(state.hit))
            {
                const LONG minRight =
                    rect.left + state.minWidth;

                const LONG maxRight =
                    rect.left + state.maxWidth;

                rect.right = std::clamp(
                    rect.right + dx,
                    minRight,
                    maxRight);

                adjustedPoint.x = rect.right;
                sizingEdge = WMSZ_RIGHT;
            }

            if (hasTop(state.hit))
            {
                const LONG maxTop =
                    rect.bottom - state.maxHeight;

                const LONG minTop =
                    rect.bottom - state.minHeight;

                rect.top = std::clamp(
                    rect.top + dy,
                    maxTop,
                    minTop);

                if (sizingEdge == WMSZ_LEFT)
                    sizingEdge = WMSZ_TOPLEFT;
                else if (sizingEdge == WMSZ_RIGHT)
                    sizingEdge = WMSZ_TOPRIGHT;
                else
                    sizingEdge = WMSZ_TOP;

                adjustedPoint.y = rect.top;
            }
            else if (hasBottom(state.hit))
            {
                const LONG minBottom =
                    rect.top + state.minHeight;

                const LONG maxBottom =
                    rect.top + state.maxHeight;

                rect.bottom = std::clamp(
                    rect.bottom + dy,
                    minBottom,
                    maxBottom);

                if (sizingEdge == WMSZ_LEFT)
                    sizingEdge = WMSZ_BOTTOMLEFT;
                else if (sizingEdge == WMSZ_RIGHT)
                    sizingEdge = WMSZ_BOTTOMRIGHT;
                else
                    sizingEdge = WMSZ_BOTTOM;

                adjustedPoint.y = rect.bottom;
            }

            CallWindowProcW(
                state.originalWndProc,
                state.hwnd,
                WM_SIZING,
                sizingEdge,
                reinterpret_cast<LPARAM>(&rect));

            const LONG width =
                rect.right - rect.left;

            const LONG height =
                rect.bottom - rect.top;

            if (width <= 0 || height <= 0)
                return;

            SetWindowPos(
                state.hwnd,
                nullptr,
                rect.left,
                rect.top,
                width,
                height,
                SWP_NOZORDER |
                    SWP_NOACTIVATE |
                    SWP_NOOWNERZORDER);

            state.resizeRect = rect;
            state.capturePoint = adjustedPoint;

            SetCursor(cursorForHit(state.hit));
        }

        static void handleResizeMessage(
            State &state,
            const MSG &message)
        {
            if (!state.resizing ||
                message.hwnd != state.hwnd)
                return;

            switch (message.message)
            {
            case WM_MOUSEMOVE:
            case WM_NCMOUSEMOVE:
                applyResize(state, message.pt);
                break;

            case WM_LBUTTONUP:
            case WM_NCLBUTTONUP:
                state.finishResize(false);
                break;

            case WM_KEYDOWN:
                if (message.wParam == VK_ESCAPE)
                    state.finishResize(true);
                else if (message.wParam == VK_RETURN)
                    state.finishResize(false);
                break;

            default:
                break;
            }
        }

        static LRESULT handleMessage(
            State &state,
            UINT message,
            WPARAM wParam,
            LPARAM lParam)
        {
            switch (message)
            {
            case WM_NCLBUTTONDOWN:
            {
                if (state.resizeEnabled && isResizeHit(wParam))
                {
                    POINT point{
                        static_cast<SHORT>(LOWORD(lParam)),
                        static_cast<SHORT>(HIWORD(lParam))};

                    if (!GetWindowRect(
                            state.hwnd,
                            &state.originalRect))
                        break;

                    if (beginResize(
                            state,
                            wParam,
                            &point))
                        return 0;
                }

                break;
            }

            case WM_SYSCOMMAND:
            {
                const WPARAM command = wParam & 0xFFF0;

                // Fullscreen ignores minimize/restore/maximize (Win+Down etc.)
                if (fullscreen && !IsIconic(state.hwnd) &&
                    (command == SC_MINIMIZE ||
                     command == SC_RESTORE ||
                     command == SC_MAXIMIZE))
                    return 0;

                if (state.resizeEnabled && command == SC_SIZE)
                {
                    WPARAM hit =
                        systemCommandToHit(wParam);

                    if (isResizeHit(hit) &&
                        beginResize(state, hit))
                        return 0;
                }

                break;
            }

            case WM_WINDOWPOSCHANGING:
            {
                // Catches size/move changes that skip WM_SYSCOMMAND
                if (fullscreen && !internalChange && !IsIconic(state.hwnd))
                {
                    auto *pos = reinterpret_cast<WINDOWPOS *>(lParam);

                    // -32000 is where Windows parks a minimizing window, let that through
                    if (pos->x != -32000 || pos->y != -32000)
                        pos->flags |= SWP_NOMOVE | SWP_NOSIZE;
                }

                break;
            }

            case WM_SETCURSOR:
            {
                if (state.resizing)
                {
                    SetCursor(
                        cursorForHit(state.hit));

                    return TRUE;
                }

                break;
            }

            case WM_CAPTURECHANGED:
            {
                if (state.resizing &&
                    reinterpret_cast<HWND>(lParam) != state.hwnd)
                {
                    state.resizing = false;
                }

                break;
            }

            case WM_CANCELMODE:
            {
                if (state.resizing)
                    state.finishResize(false);

                break;
            }

            case WM_KEYDOWN:
            {
                // Bit 30 is set when the key was already down (auto repeat)
                lastKeyWasRepeat = (lParam & (1 << 30)) != 0;

                if (state.resizing &&
                    wParam == VK_ESCAPE)
                {
                    state.finishResize(true);
                    return 0;
                }

                break;
            }

            case WM_SYSKEYDOWN:
            {
                lastKeyWasRepeat = (lParam & (1 << 30)) != 0;
                break;
            }

            case WM_KEYUP:
            case WM_SYSKEYUP:
            {
                lastKeyWasRepeat = false;
                break;
            }

            case WM_DESTROY:
            {
                if (state.resizing)
                    state.finishResize(false);

                break;
            }

            default:
                break;
            }

            return CallWindowProcW(
                state.originalWndProc,
                state.hwnd,
                message,
                wParam,
                lParam);
        }

        static LRESULT CALLBACK wndProc(
            HWND hwnd,
            UINT message,
            WPARAM wParam,
            LPARAM lParam)
        {
            auto *state =
                static_cast<State *>(
                    GetPropW(
                        hwnd,
                        propertyName()));

            if (!state)
                return DefWindowProcW(
                    hwnd,
                    message,
                    wParam,
                    lParam);

            return handleMessage(
                *state,
                message,
                wParam,
                lParam);
        }

        static LRESULT CALLBACK getMessageProc(
            int code,
            WPARAM wParam,
            LPARAM lParam)
        {
            if (code == HC_ACTION && lParam != 0)
            {
                const MSG &message =
                    *reinterpret_cast<const MSG *>(lParam);

                if (message.hwnd)
                {
                    auto *state =
                        static_cast<State *>(
                            GetPropW(
                                message.hwnd,
                                propertyName()));

                    if (state)
                    {
                        handleResizeMessage(
                            *state,
                            message);
                    }
                }
            }

            return CallNextHookEx(
                nullptr,
                code,
                wParam,
                lParam);
        }
    };

}