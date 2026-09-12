#pragma once

#include "UIElement.hpp"

/* class Theme
{
    public:
    sf::Color noColor = sf::Color::Transparent;

    sf::Color windowBackground = sf::Color(sf::Color(32, 32, 32));
    sf::Color accentColor = sf::Color(75, 181, 236);

    sf::Color btnIdle = sf::Color(45, 45, 45);
    sf::Color btnHover = sf::Color(50, 50, 50);
    sf::Color btnClicked = sf::Color(39, 39, 39);
    sf::Color btnActive = accentColor;
    sf::Color btnBorder = sf::Color(53, 53, 53);
    sf::Color btnBorderClick = sf::Color(48, 48, 48);
    sf::Color btnDisabled = sf::Color(26, 26, 26);

    sf::Color textNormal = sf::Color(255, 255, 255);
    sf::Color textDisabled = sf::Color(100, 100, 100);

    sf::Color sliderTrack = sf::Color(154, 154, 154);
    sf::Color sliderFill = accentColor;
    sf::Color sliderKnob = accentColor;
    sf::Color sliderKnobOutline = sf::Color(69, 69, 69);
    sf::Color sliderFillDiabled = sf::Color(36, 36, 36);
    sf::Color sliderKnobDisabled = sf::Color(26, 26, 26);

    //sf::Color outline = sf::Color(50, 50, 50);
    float outlineThick = 1.f;
    unsigned charSize = 16u;
}; */

#pragma once

#include <SFML/Graphics.hpp>

/* struct Theme
{

    sf::Color windowBackground;
    sf::Color accentColor;

    sf::Color btnIdle;
    sf::Color btnHover;
    sf::Color btnClicked;
    sf::Color btnActive;
    sf::Color btnBorder;
    sf::Color btnBorderClick;
    sf::Color btnDisabled;

    sf::Color textNormal;
    sf::Color textDisabled;

    sf::Color sliderTrack;
    sf::Color sliderFill;
    sf::Color sliderKnob;
    sf::Color sliderKnobOutline;
    sf::Color sliderFillDisabled;
    sf::Color sliderKnobDisabled;

    sf::Font font;
    float outlineThick;
    unsigned charSize;
}; */

// extern Theme whiteTheme;

struct Theme
{
    sf::Color windowBackground;
    sf::Color accentColor;

    sf::Color btnIdle;
    sf::Color btnHover;
    sf::Color btnClicked;
    sf::Color btnActive;
    sf::Color btnBorder;
    sf::Color btnBorderClick;
    sf::Color btnDisabled;

    sf::Color textNormal;
    sf::Color textDisabled;

    sf::Color sliderTrack;
    sf::Color sliderFill;
    sf::Color sliderKnob;
    sf::Color sliderInnerKnob;
    sf::Color sliderKnobOutline;
    sf::Color sliderFillDisabled;
    sf::Color sliderKnobDisabled;

    float outlineThick;
    unsigned charSize;

    Theme(
        const sf::Color &windowBackground,
        const sf::Color &accentColor,
        const sf::Color &btnIdle,
        const sf::Color &btnHover,
        const sf::Color &btnClicked,
        const sf::Color &btnActive,
        const sf::Color &btnBorder,
        const sf::Color &btnBorderClick,
        const sf::Color &btnDisabled,
        const sf::Color &textNormal,
        const sf::Color &textDisabled,
        const sf::Color &sliderTrack,
        const sf::Color &sliderFill,
        const sf::Color &sliderKnob,
        const sf::Color &sliderInnerKnob,
        const sf::Color &sliderKnobOutline,
        const sf::Color &sliderFillDisabled,
        const sf::Color &sliderKnobDisabled,
        // const sf::Font &font,
        float outlineThick,
        unsigned charSize)
        : windowBackground(windowBackground),
          accentColor(accentColor),
          btnIdle(btnIdle),
          btnHover(btnHover),
          btnClicked(btnClicked),
          btnActive(btnActive),
          btnBorder(btnBorder),
          btnBorderClick(btnBorderClick),
          btnDisabled(btnDisabled),
          textNormal(textNormal),
          textDisabled(textDisabled),
          sliderTrack(sliderTrack),
          sliderFill(sliderFill),
          sliderKnob(sliderKnob),
          sliderInnerKnob(sliderInnerKnob),
          sliderKnobOutline(sliderKnobOutline),
          sliderFillDisabled(sliderFillDisabled),
          sliderKnobDisabled(sliderKnobDisabled),
          outlineThick(outlineThick),
          charSize(charSize)
    {
    }
};

inline Theme darkTheme(
    sf::Color(32, 32, 32),
    sf::Color(75, 181, 236),
    sf::Color(45, 45, 45),
    sf::Color(50, 50, 50),
    sf::Color(39, 39, 39),
    sf::Color(75, 181, 236),
    sf::Color(53, 53, 53),
    sf::Color(48, 48, 48),
    sf::Color(26, 26, 26),
    sf::Color(255, 255, 255),
    sf::Color(100, 100, 100),
    sf::Color(154, 154, 154),
    sf::Color(75, 181, 236),
    sf::Color(75, 181, 236),
    sf::Color(69, 69, 69),
    sf::Color(36, 36, 36),
    sf::Color(26, 26, 26),
    1.f,
    16u);
