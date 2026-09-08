#include <SFML/Graphics.hpp>

#include <string>
#include <vector>
#include <sstream>
#include <windows.h>

#include "SFUI/SFUI.hpp"

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
    sf::View camera;
    camera.setSize(sf::Vector2f{static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y)});
    camera.setCenter(sf::Vector2f{static_cast<float>(window.getSize().x) / 2.f, static_cast<float>(window.getSize().y) / 2.f});

    sf::Font font;

    if (!font.openFromFile("arial.ttf"))
    {
        MessageBoxA(
            nullptr,
            "Failed to load arial.ttf.",
            "Font Loading Error",
            MB_OK | MB_ICONERROR);

        return -1;
    }

    Theme theme;
    UIManager ui;

    sf::Text statusText(font, "Waiting...", 18u);
    statusText.setFillColor(sf::Color(200, 200, 200));
    statusText.setPosition({50.f, 420.f});



    Button buttonA;
    buttonA.setSize({150.f, 50.f});
    buttonA.setPosition({50.f, 48.f});
    buttonA.setFont(font);
    buttonA.setLabel("Click Me");

    Button buttonB;
    buttonB.setSize({150.f, 50.f});
    buttonB.setPosition({220.f, 48.f});
    buttonB.setFont(font);
    buttonB.setLabel("Toggle Slider");

    Button buttonC;
    buttonC.setSize({150.f, 50.f});
    buttonC.setPosition({390.f, 48.f});
    buttonC.setFont(font);
    buttonC.setLabel("Disabled");

    Slider sliderA(0.f, 100.f, 50.f);
    sliderA.setSize(sf::Vector2f(500.f, 10.f));
    sliderA.setPosition(sf::Vector2f(50.f, 180.f));
    sliderA.setFont(font);

    Slider sliderB(-1.f, 1.f, 0.f);
    sliderB.setSize(sf::Vector2f(300.f, 30.f));
    sliderB.setPosition(sf::Vector2f(50.f, 280.f));
    sliderB.setFont(font);

    auto btnA = std::make_shared<Button>(buttonA);
    auto btnB = std::make_shared<Button>(buttonB);
    auto btnDisabled = std::make_shared<Button>(buttonC);
    btnDisabled->setEnabled(false);

    auto sldrA = std::make_shared<Slider>(sliderA);
    auto sldrB = std::make_shared<Slider>(sliderB);

    btnA->setOnClick([&]()
                     { statusText.setString("Btn A clicked"); });

    btnB->setOnClick([&]()
                     { sldrA->setVisible(!sldrA->isEnabled()); });

    sldrA->setOnChange([&](float v)
                       { statusText.setString("Slider A: " + std::to_string(static_cast<int>(v))); });

    sldrB->setOnChange([&](float v)
                       {
                            std::ostringstream oss;
                            oss << std::fixed << std::setprecision(2) << v;
                            statusText.setString("Slider B: " + oss.str()); });

    ui.add(btnA);
    ui.add(btnB);
    ui.add(btnDisabled);
    ui.add(sldrA);
    ui.add(sldrB);

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

                letterboxView(
                    camera,
                    window.getSize().x,
                    window.getSize().y);

                window.setView(camera);
            }

            ui.handleEvent(*event, window);
        }

        window.clear(sf::Color(30, 30, 30));
        ui.draw(window);
        window.draw(statusText);
        window.display();
    }

    return 0;
}

