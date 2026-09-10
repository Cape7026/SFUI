#include "Label.hpp"

void Label::handleEvent(const sf::Event &event, const sf::RenderWindow &window)
{
}

void Label::handleVisual()
{
    if (!m_text)
    {
        m_text.emplace(*m_font, m_label, m_theme.charSize);
        m_text->setPosition(m_position);
    }

    if (!m_enabled)
        m_text->setFillColor(m_theme.textDisabled);
    else
        m_text->setFillColor(m_theme.textNormal);
}

void Label::draw(sf::RenderWindow &window)
{
    if (!m_visible)
    {
        return;
    }
    else
    {
        window.draw(*m_text);
    }
}