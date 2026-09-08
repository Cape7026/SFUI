

#include "UIElement.hpp"
UIElement::UIElement(const Theme &theme)
    : m_visible(true), m_enabled(true), m_theme(theme)
{
}

UIElement::~UIElement() = default;

void UIElement::setPosition(sf::Vector2f position)
{
    m_position = position;
}

void UIElement::setSize(sf::Vector2f size)
{
    m_size = size;
}

void UIElement::setScale(sf::Vector2f scale)
{
    m_scale = scale;
}

void UIElement::setRotation(sf::Angle angle)
{
    m_angle = angle;
}

void UIElement::setVisible(bool v)
{
    m_visible = v;
}

void UIElement::setEnabled(bool e)
{
    m_enabled = e;
}

void UIElement::setFont(sf::Font font)
{
    m_font = font;
}

void UIElement::setIcon(sf::Image icon)
{
    m_icon = icon;
}

void UIElement::setLabel(std::string label)
{
    m_label = label;
}

void UIElement::setTheme(Theme theme)
{
    m_theme = theme;
}

void UIElement::setOnClick(ClickCallback cb)
{
    m_onClick = std::move(cb);
}

void UIElement::setOnChange(ChangeCallbackFloat cb)
{
    m_onChange = std::move(cb);
}

sf::Vector2f UIElement::getPosition() const
{
    return m_position;
}

sf::Vector2f UIElement::getSize() const
{
    return m_size;
}

sf::Vector2f UIElement::getScale() const
{
    return m_scale;
}

sf::Angle UIElement::getRotation() const
{
    return m_angle;
}

bool UIElement::isVisible() const
{
    return m_visible;
}

bool UIElement::isEnabled() const
{
    return m_enabled;
}

sf::FloatRect UIElement::getBounds() const
{
    return sf::FloatRect(m_position, m_size);
}

bool UIElement::containsPoint(sf::Vector2f point) const
{
    return getBounds().contains(point);
}