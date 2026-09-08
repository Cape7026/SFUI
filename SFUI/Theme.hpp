#pragma once

#include <SFML/Graphics.hpp>

struct Theme
{
    sf::Color noColor = sf::Color::Transparent;

    sf::Color windowBackground = sf::Color(sf::Color(30, 30, 30));

    sf::Color btnIdle = sf::Color(43, 43, 43);
    sf::Color btnHover = sf::Color(70, 70, 70);
    sf::Color btnActive = sf::Color(40, 120, 200);
    sf::Color btnDisabled = sf::Color(26, 26, 26);

    sf::Color textNormal = sf::Color(230, 230, 230);
    sf::Color textDisabled = sf::Color(100, 100, 100);

    sf::Color sliderTrack = sf::Color(50, 50, 50);
    sf::Color sliderFill = sf::Color(40, 120, 200);
    sf::Color sliderKnob = sf::Color(220, 220, 220);

    sf::Color outline = sf::Color(50, 50, 50);
    float outlineThick = 1.5f;
    unsigned charSize = 16u;
};
