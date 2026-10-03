#include "Slider.hpp"

Slider::Slider(float minVal, float maxVal, float initVal, sf::Font &font)
    : m_min(minVal),
      m_max(maxVal),
      m_value(std::clamp(initVal, minVal, maxVal)),
      m_dragging(false),
      //m_font(&font),
      m_valueLabel(std::in_place, font, ".", m_theme.charSize - 2u)
{
}

void Slider::setValue(float v)
{
    m_value = std::clamp(v, m_min, m_max);
    updateKnob();
    updateLabel();

    if (m_onChange)
        m_onChange(m_value);
}

float Slider::getValue() const
{
    return m_value;
}

void Slider::handleEvent(const sf::Event &event, const sf::RenderWindow &window)
{
    if (!m_enabled || !m_visible)
        return;

    if (const auto *mouseButton = event.getIf<sf::Event::MouseButtonPressed>())
    {
        if (mouseButton->button == sf::Mouse::Button::Left)
        {
            sf::Vector2f mousePosition = window.mapPixelToCoords({mouseButton->position.x, mouseButton->position.y});

            if (m_sliderBounds.getGlobalBounds().contains(mousePosition) || m_knob.getGlobalBounds().contains(mousePosition))
            {
                m_dragging = true;
                applyDrag(mousePosition.x);
            }
        }
    }

    if (const auto *mouseButton = event.getIf<sf::Event::MouseButtonReleased>())
    {
        if (mouseButton->button == sf::Mouse::Button::Left)
            m_dragging = false;
    }

    if (const auto *mouseMoved = event.getIf<sf::Event::MouseMoved>())
    {
        if (m_dragging)
        {
            sf::Vector2f mousePosition = window.mapPixelToCoords({mouseMoved->position.x, mouseMoved->position.y});
            applyDrag(mousePosition.x);
        }
    }
}


void Slider::handleVisual()
{
    if (!m_enabled)
    {
        m_track.setFillColor(m_theme.sliderTrack);
        m_fill.setFillColor(m_theme.sliderFillDisabled);
        m_knob.setFillColor(m_theme.sliderKnobDisabled);
        return;
    }
    else
    {
        float midY = trackMidY();

        m_track.setSize({m_size.x, m_size.y}); //kTrackH in place of m_size.y
        m_track.setPosition({trackLeft(), midY - m_size.y / 2.f}); //kTrackH in place of m_size.y
        m_track.setFillColor(m_theme.sliderTrack);
        m_track.setCornerPointCount(256);
        m_track.setRadius(100);

        m_fill = m_track;
        m_fill.setFillColor(m_theme.sliderFill);

        m_knob.setRadius(m_size.y); //kKnobR // radius * 2 thats why its bigger 
        m_knob.setPointCount(256);
        m_knob.setFillColor(m_theme.sliderKnob);
        m_knob.setOrigin({m_knob.getRadius(), m_knob.getRadius()});

        m_sliderBounds.setSize({m_size.x, m_size.y * 2.5f});
        m_sliderBounds.setPosition({trackLeft() + m_size.x / 2.f, midY - m_size.y / 2.f});
        m_sliderBounds.setFillColor(sf::Color::Transparent);
        m_sliderBounds.setOutlineColor(m_theme.btnBorder);
        m_sliderBounds.setOutlineThickness(1.f);
        m_sliderBounds.setOrigin({m_size.x / 2.f, m_size.y / 2.f});

        updateKnob();
        updateLabel();
    }
}


void Slider::draw(sf::RenderWindow &window)
{
    if (!m_visible)
    {
        return;
    }
    else
    {
        window.draw(m_track);
        window.draw(m_fill);
        window.draw(m_knob);
        // window.draw(m_sliderBounds);
        if (m_valueLabel.has_value())
        {
            window.draw(*m_valueLabel);
        }
    }
}

float Slider::trackLeft() const
{
    return m_position.x;
}

float Slider::trackRight() const
{
    return m_position.x + m_size.x;
}

float Slider::trackMidY() const
{
    return m_position.y + m_size.y / 2.f;
}



void Slider::applyDrag(float mouseX)
{
    float ratio = (mouseX - trackLeft()) / (trackRight() - trackLeft());

    setValue(m_min + std::clamp(ratio, 0.f, 1.f) * (m_max - m_min));
}

void Slider::updateKnob()
{
    float ratio = (m_value - m_min) / (m_max - m_min);
    float knobX = trackLeft() + ratio * (trackRight() - trackLeft());

    m_knob.setPosition({knobX, trackMidY()});
    m_fill.setSize({knobX - trackLeft(), m_size.y});  // kTrackH
}

void Slider::updateLabel()
{
    if (!m_valueLabel.has_value())
        return;

    std::ostringstream oss;

    oss << std::fixed << std::setprecision(1) << m_value;

    m_valueLabel->setString(oss.str());
    m_valueLabel->setFillColor(m_theme.textNormal);

    sf::FloatRect lb = m_valueLabel->getLocalBounds();

    m_valueLabel->setOrigin({lb.position.x + lb.size.x / 2.f, 0.f});
    m_valueLabel->setPosition({m_position.x + m_size.x / 2.f, m_position.y + m_size.y + 4.f});
}

