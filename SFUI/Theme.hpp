#pragma once

#include <SFML/Graphics.hpp>

struct Theme
{
    sf::Color noColor = sf::Color::Transparent;

    sf::Color windowBackground = sf::Color(sf::Color(32, 32, 32));

    sf::Color btnIdle = sf::Color(45, 45, 45);
    sf::Color btnHover = sf::Color(50, 50, 50);
    sf::Color btnClicked = sf::Color(39, 39, 39);
    sf::Color btnActive = sf::Color(75, 181, 236);
    sf::Color btnDisabled = sf::Color(26, 26, 26);

    sf::Color textNormal = sf::Color(255, 255, 255);
    sf::Color textDisabled = sf::Color(100, 100, 100);

    sf::Color sliderTrack = sf::Color(50, 50, 50);
    sf::Color sliderFill = sf::Color(40, 120, 200);
    sf::Color sliderKnob = sf::Color(220, 220, 220);
    sf::Color sliderFillDiabled = sf::Color(36, 36, 36);
    sf::Color sliderKnobDisabled = sf::Color(26, 26, 26);

    sf::Color outline = sf::Color(50, 50, 50);
    float outlineThick = 1.5f;
    unsigned charSize = 16u;
};
