#include <SFML/Graphics.hpp>

#include "SFUI/SFUI.hpp"

int main()
{
    sf::RenderWindow window(sf::VideoMode({800, 600}), "SFUI", sf::State::Windowed);

    sf::Font font;
    if (!font.openFromFile("arial.ttf"))
    {}

    UIManager ui;

    Button buttonA;
    buttonA.setSize({100.f, 50.f});
    buttonA.setPosition({50.f, 48.f});
    buttonA.setFont(font);
    buttonA.setLabel("Print");
    auto btnA = std::make_shared<Button>(buttonA);

    ui.add(btnA);

    while (window.isOpen())
    {
        // Set visual updates
        ui.handelVisual();
        while (std::optional<sf::Event> event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            ui.handleEvent(*event, window);
        }

        window.clear(sf::Color(30, 30, 30));
        // Draw to window
        ui.draw(window);
        window.display();
    }
}

