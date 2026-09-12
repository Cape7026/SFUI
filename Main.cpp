#include <SFML/Graphics.hpp>

#include <string>
#include <vector>
#include <sstream>
#include <windows.h>
#include <dwmapi.h>

#include "SFUI/SFUI.hpp"

#pragma comment(lib, "dwmapi.lib")

void letterboxView(sf::View &view, unsigned int windowWidth, unsigned int windowHeight)
{
    float windowRatio = static_cast<float>(windowWidth) / windowHeight;
    float viewRatio = view.getSize().x / view.getSize().y;

    float sizeX = 1.f, sizeY = 1.f;
    float posX = 0.f, posY = 0.f;

    if (windowRatio > viewRatio)
    {
        sizeX = viewRatio / windowRatio;
        posX = (1.f - sizeX) / 2.f;
    }
    else
    {
        sizeY = windowRatio / viewRatio;
        posY = (1.f - sizeY) / 2.f;
    }

    view.setViewport(sf::FloatRect({posX, posY}, {sizeX, sizeY}));
}

int main()
{
    sf::ContextSettings settings{0, 0, 16};

    sf::RenderWindow window(sf::VideoMode({800, 600}), "SFUI", sf::State::Windowed, settings);

    window.setFramerateLimit(100);

    HWND hwnd = window.getNativeHandle();

    COLORREF titleBarColor = RGB(30, 30, 30);

    DwmSetWindowAttribute(
        hwnd,
        DWMWA_CAPTION_COLOR,
        &titleBarColor,
        sizeof(titleBarColor)
    );
    sf::View camera;
    camera.setSize(sf::Vector2f{static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y)});
    camera.setCenter(sf::Vector2f{static_cast<float>(window.getSize().x) / 2.f, static_cast<float>(window.getSize().y) / 2.f});

    sf::Font font;

    sf::Font iconFont("C:/Windows/Fonts/SegoeIcons.ttf");

    std::cout << iconFont.hasGlyph(U'\uE700') << '\n';

    if (!font.openFromFile("Aptos.ttf"))
    {
        MessageBoxA(
            nullptr,
            "Failed to load arial.ttf.",
            "Font Loading Error",
            MB_OK | MB_ICONERROR);

        return -1;
    }
    font.setSmooth(false);
    iconFont.setSmooth(false);

    
    UIManager ui;

    /* sf::Text statusText(font, "Waiting...", 18u);
    statusText.setFillColor(sf::Color(200, 200, 200));
    statusText.setPosition({50.f, 420.f}); */

    std::cout << "A\n";
    Button buttonA;
    std::cout << "B\n";
    buttonA.setSize({32.f, 32.f}); // 89 32
    std::cout << "C\n";
    buttonA.setPosition({50.f, 48.f});
    std::cout << "D\n";
    buttonA.setFont(iconFont);
    std::cout << "E\n";
    buttonA.setLabel(sf::String(U"\uE713"));
    std::cout << "F\n";
    auto btnA = std::make_shared<Button>(buttonA);

    std::cout << "G\n";

    // bug here
    Slider sliderA(0.f, 100.f, 10.f, font);
    std::cout << "H\n";
    sliderA.setSize({200, 10});
    std::cout << "I\n";
    // sliderA.setValue(10.f);
    std::cout << "J\n";
    sliderA.setPosition({50.f, 58.f});
    auto sldA = std::make_shared<Slider>(sliderA);


    //btnA->setEnabled(false);

   /*  btnA->setOnClick([&]()
                     { statusText.setString("Btn A clicked"); }); */

    ui.add(btnA);
    ui.add(sldA);

    

    bool isFocused = true;

    while (window.isOpen())
    {
        ui.handelVisual();
        while (std::optional<sf::Event> event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            else if (event->is<sf::Event::FocusLost>())
            {
                isFocused = false;
            }
            else if (event->is<sf::Event::FocusGained>())
            {
                isFocused = true;
            }
            else if (event->is<sf::Event::Resized>())
            {
                camera.setSize({static_cast<float>(window.getSize().x),
                                static_cast<float>(window.getSize().y)});

                camera.setCenter({window.getSize().x / 2.f,
                                  window.getSize().y / 2.f});

                letterboxView(camera, window.getSize().x, window.getSize().y);
                window.setView(camera);
            }
            ui.handleEvent(*event, window);
        }

        window.clear(sf::Color(30, 30, 30));
        ui.draw(window);
        //window.draw(statusText);
        window.display();
    }

    return 0;
}
