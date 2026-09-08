#include "Label.hpp"

void Label::handleEvent(const sf::Event &event, const sf::RenderWindow &window)
{
    
}

void Label::handleVisual()
{
    m_text.setCharacterSize(m_charecterSize);
    m_text.setFont(m_font);
    m_text.setString(m_label);
}

void Label::draw(sf::RenderWindow &window)
{
    window.draw(m_text);
}