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
    if (!m_enabled)
        return;

    if (const auto *mb = event.getIf<sf::Event::MouseButtonPressed>())
    {
        if (mb->button == sf::Mouse::Button::Left)
        {
            sf::Vector2f mp = window.mapPixelToCoords({mb->position.x, mb->position.y});

            if (containsPoint(mp))
            {
                m_dragging = true;
                applyDrag(mp.x);
            }
        }
    }

    if (const auto *mb = event.getIf<sf::Event::MouseButtonReleased>())
    {
        if (mb->button == sf::Mouse::Button::Left)
            m_dragging = false;
    }

    if (const auto *mm = event.getIf<sf::Event::MouseMoved>())
    {
        if (m_dragging)
        {
            sf::Vector2f mp = window.mapPixelToCoords({mm->position.x, mm->position.y});
            applyDrag(mp.x);
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

        m_track.setSize({m_size.x, kTrackH});
        m_track.setPosition({trackLeft(), midY - kTrackH / 2.f});
        m_track.setFillColor(m_theme.sliderTrack);
        m_track.setCornerPointCount(256);
        m_track.setRadius(100);

        m_fill = m_track;
        m_fill.setFillColor(m_theme.sliderFill);

        m_knob.setRadius(kKnobR);
        m_knob.setPointCount(256);
        m_knob.setFillColor(m_theme.sliderKnob);
        m_knob.setOrigin({kKnobR, kKnobR});

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
        window.draw(*m_valueLabel);
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

void Slider::buildShapes()
{
}

void Slider::applyDrag(float mouseX)
{
    float t = (mouseX - trackLeft()) / (trackRight() - trackLeft());

    setValue(m_min + std::clamp(t, 0.f, 1.f) * (m_max - m_min));
}

void Slider::updateKnob()
{
    float t = (m_value - m_min) / (m_max - m_min);
    float kx = trackLeft() + t * (trackRight() - trackLeft());

    m_knob.setPosition({kx, trackMidY()});
    m_fill.setSize({kx - trackLeft(), kTrackH});
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

