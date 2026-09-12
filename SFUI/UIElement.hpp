#pragma once

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <functional>
#include <iostream>
#include <windows.h>
#include <dwmapi.h>
#include <SFML/Graphics.hpp>

#include "RoundedRectangleShape.hpp"
#include "Theme.hpp"


#include <optional>

class UIElement
{
public:
    UIElement(): m_font(nullptr){};

    //Theme darkTheme;

    using ClickCallback = std::function<void()>;
    using ChangeCallbackFloat = std::function<void(float)>;
    using changeCallbackInt = std::function<void(int)>;

    // UIElement(const Theme &theme = Theme{});

    virtual void setPosition(sf::Vector2f position);
    virtual void setSize(sf::Vector2f size);
    virtual void setScale(sf::Vector2f scale);
    virtual void setRotation(sf::Angle angle);
    virtual void setVisible(bool v);
    virtual void setEnabled(bool e);
    virtual void setFont(const sf::Font &font);
    virtual void setIcon(sf::Image icon);
    // virtual void setIcon(sf::String icon);
    virtual void setCharecterSize(float size);
    void setLabel(sf::String label = "");
    void setTheme(Theme theme);
    void setOnClick(ClickCallback cb);
    void setOnChange(ChangeCallbackFloat cb);

    sf::Vector2f getPosition() const;
    sf::Vector2f getSize() const;
    sf::Vector2f getScale() const;
    sf::Angle getRotation() const;
    bool isVisible() const;
    bool isEnabled() const;

    virtual void handleEvent(const sf::Event &event, const sf::RenderWindow &window) = 0;
    virtual void handleVisual() = 0;
    virtual void draw(sf::RenderWindow &window) = 0;

protected:
    sf::FloatRect getBounds() const;
    bool containsPoint(sf::Vector2f point) const;

    sf::Vector2f m_position;
    sf::Vector2f m_size;
    sf::Vector2f m_scale;
    sf::Angle m_angle;
    bool m_visible = true;
    bool m_enabled = true;
    const sf::Font *m_font;
    sf::Image m_icon;
    sf::String m_label;
    float m_charecterSize;

    Theme m_theme = darkTheme;

    ClickCallback m_onClick;
    ChangeCallbackFloat m_onChange;
};
