#include "Button.hpp"

void Button::handleEvent(const sf::Event &event, const sf::RenderWindow &window)
{
    if (!m_enabled)
        return;

    if (const auto *mm = event.getIf<sf::Event::MouseMoved>())
    {
        sf::Vector2f mp = window.mapPixelToCoords({mm->position.x, mm->position.y});
        m_hovered = containsPoint(mp);
    }

    if (const auto *mb = event.getIf<sf::Event::MouseButtonPressed>())
    {
        if (mb->button == sf::Mouse::Button::Left)
        {
            sf::Vector2f mp = window.mapPixelToCoords({mb->position.x, mb->position.y});
            if (containsPoint(mp))
                m_pressed = true;
        }
    }

    if (const auto *mb = event.getIf<sf::Event::MouseButtonReleased>())
    {
        if (mb->button == sf::Mouse::Button::Left)
        {
            sf::Vector2f mp = window.mapPixelToCoords({mb->position.x, mb->position.y});
            if (m_pressed && containsPoint(mp))
            {
                if (m_onClick)
                    m_onClick();
            }
            m_pressed = false;
        }
    }
}

void Button::handleVisual()
{
    m_shape.setPosition(m_position);
    m_shape.setSize(m_size);
    m_shape.setFillColor(m_theme.btnIdle);
    m_shape.setCornerPointCount(256);

    if (!m_enabled)
        m_shape.setFillColor(m_theme.btnDisabled);
    else if (m_pressed)
        m_shape.setFillColor(m_theme.btnActive);
    else if (m_hovered)
        m_shape.setFillColor(m_theme.btnHover);
    else
        m_shape.setFillColor(m_theme.btnIdle);

    float radius = std::min(m_shape.getSize().x, m_shape.getSize().y) * 0.15f;
    m_shape.setRadius(radius);

    if (!m_text)
        m_text.emplace(m_font, m_label, m_theme.charSize);

    m_text->setFillColor(m_enabled ? m_theme.textNormal : m_theme.textDisabled);
    adjustTextPosition();
    adjustTextSize();
}

void Button::draw(sf::RenderWindow &window)
{
    if (!m_visible)
        return;
    window.draw(m_shape);
    window.draw(*m_text);
}

void Button::adjustTextPosition()
{
    sf::FloatRect tb = m_text->getLocalBounds();
    m_text->setOrigin({tb.position.x + tb.size.x / 2.f, tb.position.y + tb.size.y / 2.f});
    m_text->setPosition(m_position + m_size / 2.f);
}

void Button::adjustTextSize()
{
    sf::Vector2f btnSize = m_shape.getSize();
    unsigned int size = static_cast<unsigned int>(btnSize.y * 0.6f);
    m_text->setCharacterSize(size);
    sf::FloatRect bounds = m_text->getLocalBounds();
    float widthScale = (btnSize.x * 0.6f) / bounds.size.x;
    float heightScale = (btnSize.y * 0.6f) / bounds.size.y;
    float scale = std::min(widthScale, heightScale);
    m_text->setCharacterSize(static_cast<unsigned int>(size * scale));
}

