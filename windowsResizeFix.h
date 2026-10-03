#pragma once

#include <SFML/Graphics.hpp>
#include <Windows.h>
#include <algorithm>

namespace sfml
{
class Win32ResizeFix
{
public:
    explicit Win32ResizeFix(sf::RenderWindow& window)
        : m_hwnd(window.getNativeHandle())
    {
        if (!m_hwnd)
            return;

        SetPropW(m_hwnd, propertyName(), this);

        SetLastError(0);
        m_originalWndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtrW(m_hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Win32ResizeFix::wndProc)));

        if (!m_originalWndProc && GetLastError() != 0)
        {
            RemovePropW(m_hwnd, propertyName());
            return;
        }

        m_messageHook = SetWindowsHookExW(WH_GETMESSAGE, &Win32ResizeFix::getMessageProc, nullptr, GetCurrentThreadId());

        if (!m_messageHook)
        {
            SetWindowLongPtrW(m_hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(m_originalWndProc));
            RemovePropW(m_hwnd, propertyName());
            return;
        }

        m_installed = true;
    }

    ~Win32ResizeFix()
    {
        if (!m_installed)
            return;

        finishResize(false);

        if (m_messageHook)
        {
            UnhookWindowsHookEx(m_messageHook);
            m_messageHook = nullptr;
        }

        if (IsWindow(m_hwnd))
            SetWindowLongPtrW(m_hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(m_originalWndProc));

        RemovePropW(m_hwnd, propertyName());
    }

    Win32ResizeFix(const Win32ResizeFix&) = delete;
    Win32ResizeFix& operator=(const Win32ResizeFix&) = delete;

private:
    static constexpr LPCWSTR propertyName()
    {
        return L"SFML_Win32ResizeFix";
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
        return hit == HTLEFT || hit == HTTOPLEFT || hit == HTBOTTOMLEFT;
    }

    static bool hasRight(WPARAM hit)
    {
        return hit == HTRIGHT || hit == HTTOPRIGHT || hit == HTBOTTOMRIGHT;
    }

    static bool hasTop(WPARAM hit)
    {
        return hit == HTTOP || hit == HTTOPLEFT || hit == HTTOPRIGHT;
    }

    static bool hasBottom(WPARAM hit)
    {
        return hit == HTBOTTOM || hit == HTBOTTOMLEFT || hit == HTBOTTOMRIGHT;
    }

    static HCURSOR cursorForHit(WPARAM hit)
    {
        switch (hit)
        {
            case HTLEFT:
            case HTRIGHT:
                return LoadCursorW(nullptr, reinterpret_cast<LPCWSTR>(IDC_SIZEWE));

            case HTTOP:
            case HTBOTTOM:
                return LoadCursorW(nullptr, reinterpret_cast<LPCWSTR>(IDC_SIZENS));

            case HTTOPLEFT:
            case HTBOTTOMRIGHT:
                return LoadCursorW(nullptr, reinterpret_cast<LPCWSTR>(IDC_SIZENWSE));

            case HTTOPRIGHT:
            case HTBOTTOMLEFT:
                return LoadCursorW(nullptr, reinterpret_cast<LPCWSTR>(IDC_SIZENESW));

            default:
                return LoadCursorW(nullptr, reinterpret_cast<LPCWSTR>(IDC_ARROW));
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

    bool beginResize(WPARAM hit, const POINT* initialPoint = nullptr)
    {
        if (m_resizing)
            return true;

        if (!isResizeHit(hit))
            return false;

        if (IsZoomed(m_hwnd) || IsIconic(m_hwnd))
            return false;

        if (!GetWindowRect(m_hwnd, &m_resizeRect))
            return false;

        if (initialPoint)
        {
            m_capturePoint = *initialPoint;
        }
        else if (!GetCursorPos(&m_capturePoint))
        {
            return false;
        }

        MINMAXINFO minMax{};
        minMax.ptMinTrackSize.x = GetSystemMetrics(SM_CXMINTRACK);
        minMax.ptMinTrackSize.y = GetSystemMetrics(SM_CYMINTRACK);
        minMax.ptMaxTrackSize.x = GetSystemMetrics(SM_CXMAXTRACK);
        minMax.ptMaxTrackSize.y = GetSystemMetrics(SM_CYMAXTRACK);

        CallWindowProcW(m_originalWndProc, m_hwnd, WM_GETMINMAXINFO, 0, reinterpret_cast<LPARAM>(&minMax));

        m_minWidth = std::max<LONG>(1, minMax.ptMinTrackSize.x);
        m_minHeight = std::max<LONG>(1, minMax.ptMinTrackSize.y);
        m_maxWidth = std::max<LONG>(m_minWidth, minMax.ptMaxTrackSize.x);
        m_maxHeight = std::max<LONG>(m_minHeight, minMax.ptMaxTrackSize.y);

        m_hit = hit;
        m_resizing = true;

        if (!SetCapture(m_hwnd))
        {
            m_resizing = false;
            return false;
        }

        SetCursor(cursorForHit(m_hit));
        return true;
    }

    void applyResize(const POINT& mousePoint)
    {
        if (!m_resizing)
            return;

        LONG dx = mousePoint.x - m_capturePoint.x;
        LONG dy = mousePoint.y - m_capturePoint.y;

        if (dx == 0 && dy == 0)
            return;

        RECT rect = m_resizeRect;
        POINT adjustedPoint = mousePoint;
        WPARAM sizingEdge = 0;

        if (hasLeft(m_hit))
        {
            const LONG maxLeft = rect.right - m_maxWidth;
            const LONG minLeft = rect.right - m_minWidth;
            const LONG newLeft = rect.left + dx;

            rect.left = std::clamp(newLeft, maxLeft, minLeft);
            adjustedPoint.x = rect.left;
            sizingEdge = WMSZ_LEFT;
        }
        else if (hasRight(m_hit))
        {
            const LONG minRight = rect.left + m_minWidth;
            const LONG maxRight = rect.left + m_maxWidth;
            const LONG newRight = rect.right + dx;

            rect.right = std::clamp(newRight, minRight, maxRight);
            adjustedPoint.x = rect.right;
            sizingEdge = WMSZ_RIGHT;
        }

        if (hasTop(m_hit))
        {
            const LONG maxTop = rect.bottom - m_maxHeight;
            const LONG minTop = rect.bottom - m_minHeight;
            const LONG newTop = rect.top + dy;

            rect.top = std::clamp(newTop, maxTop, minTop);

            if (sizingEdge == WMSZ_LEFT)
                sizingEdge = WMSZ_TOPLEFT;
            else if (sizingEdge == WMSZ_RIGHT)
                sizingEdge = WMSZ_TOPRIGHT;
            else
                sizingEdge = WMSZ_TOP;

            adjustedPoint.y = rect.top;
        }
        else if (hasBottom(m_hit))
        {
            const LONG minBottom = rect.top + m_minHeight;
            const LONG maxBottom = rect.top + m_maxHeight;
            const LONG newBottom = rect.bottom + dy;

            rect.bottom = std::clamp(newBottom, minBottom, maxBottom);

            if (sizingEdge == WMSZ_LEFT)
                sizingEdge = WMSZ_BOTTOMLEFT;
            else if (sizingEdge == WMSZ_RIGHT)
                sizingEdge = WMSZ_BOTTOMRIGHT;
            else
                sizingEdge = WMSZ_BOTTOM;

            adjustedPoint.y = rect.bottom;
        }

        CallWindowProcW(m_originalWndProc, m_hwnd, WM_SIZING, sizingEdge, reinterpret_cast<LPARAM>(&rect));

        const LONG width = rect.right - rect.left;
        const LONG height = rect.bottom - rect.top;

        if (width <= 0 || height <= 0)
            return;

        SetWindowPos(m_hwnd, nullptr, rect.left, rect.top, width, height, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER);

        m_resizeRect = rect;
        m_capturePoint = adjustedPoint;

        SetCursor(cursorForHit(m_hit));
    }

    void finishResize(bool restore)
    {
        if (!m_resizing)
            return;

        if (restore)
        {
            const LONG width = m_resizeRect.right - m_resizeRect.left;
            const LONG height = m_resizeRect.bottom - m_resizeRect.top;

            SetWindowPos(m_hwnd, nullptr, m_originalRect.left, m_originalRect.top, m_originalRect.right - m_originalRect.left, m_originalRect.bottom - m_originalRect.top, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
        }

        if (GetCapture() == m_hwnd)
            ReleaseCapture();

        m_resizing = false;
        m_hit = 0;
    }

    void handleResizeMessage(const MSG& message)
    {
        if (!m_resizing)
            return;

        if (message.hwnd != m_hwnd)
            return;

        switch (message.message)
        {
            case WM_MOUSEMOVE:
            case WM_NCMOUSEMOVE:
                applyResize(message.pt);
                break;

            case WM_LBUTTONUP:
            case WM_NCLBUTTONUP:
                finishResize(false);
                break;

            case WM_KEYDOWN:
                if (message.wParam == VK_ESCAPE)
                    finishResize(true);
                else if (message.wParam == VK_RETURN)
                    finishResize(false);
                break;

            default:
                break;
        }
    }

    LRESULT handleMessage(UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch (message)
        {
            case WM_NCLBUTTONDOWN:
            {
                if (isResizeHit(wParam))
                {
                    POINT point{};
                    point.x = static_cast<SHORT>(LOWORD(lParam));
                    point.y = static_cast<SHORT>(HIWORD(lParam));

                    m_originalRect = {};
                    if (!GetWindowRect(m_hwnd, &m_originalRect))
                        break;

                    if (beginResize(wParam, &point))
                        return 0;
                }

                break;
            }

            case WM_SYSCOMMAND:
            {
                if ((wParam & 0xFFF0) == SC_SIZE)
                {
                    const WPARAM hit = systemCommandToHit(wParam);

                    if (isResizeHit(hit))
                    {
                        if (beginResize(hit))
                            return 0;
                    }
                }

                break;
            }

            case WM_SETCURSOR:
            {
                if (m_resizing)
                {
                    SetCursor(cursorForHit(m_hit));
                    return TRUE;
                }

                break;
            }

            case WM_CAPTURECHANGED:
            {
                if (m_resizing && reinterpret_cast<HWND>(lParam) != m_hwnd)
                    m_resizing = false;

                break;
            }

            case WM_CANCELMODE:
            {
                if (m_resizing)
                    finishResize(false);

                break;
            }

            case WM_KEYDOWN:
            {
                if (m_resizing && wParam == VK_ESCAPE)
                {
                    finishResize(true);
                    return 0;
                }

                break;
            }

            case WM_DESTROY:
            {
                if (m_resizing)
                    finishResize(false);

                break;
            }

            default:
                break;
        }

        return CallWindowProcW(m_originalWndProc, m_hwnd, message, wParam, lParam);
    }

    static LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        auto* self = static_cast<Win32ResizeFix*>(GetPropW(hwnd, propertyName()));

        if (!self)
            return DefWindowProcW(hwnd, message, wParam, lParam);

        return self->handleMessage(message, wParam, lParam);
    }

    static LRESULT CALLBACK getMessageProc(int code, WPARAM wParam, LPARAM lParam)
    {
        if (code == HC_ACTION && lParam != 0)
        {
            const MSG& message = *reinterpret_cast<const MSG*>(lParam);

            if (message.hwnd)
            {
                auto* self = static_cast<Win32ResizeFix*>(GetPropW(message.hwnd, propertyName()));

                if (self)
                    self->handleResizeMessage(message);
            }
        }

        return CallNextHookEx(nullptr, code, wParam, lParam);
    }

private:
    HWND m_hwnd = nullptr;
    WNDPROC m_originalWndProc = nullptr;
    HHOOK m_messageHook = nullptr;

    bool m_installed = false;
    bool m_resizing = false;

    WPARAM m_hit = 0;

    POINT m_capturePoint{};
    RECT m_resizeRect{};
    RECT m_originalRect{};

    LONG m_minWidth = 1;
    LONG m_minHeight = 1;
    LONG m_maxWidth = 50000;
    LONG m_maxHeight = 50000;
};
}