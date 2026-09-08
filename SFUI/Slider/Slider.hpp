#pragma once

#include "../UIElement.hpp"

class Slider : public UIElement
{
public:
    Slider(float minVal, float maxVal, float initVal);

    void setValue(float v);
    float getValue() const;

    void handleEvent(const sf::Event &event, const sf::RenderWindow &window) override;
    void handleVisual() override;
    void draw(sf::RenderWindow &window) override;

private:
    static constexpr float kTrackH = 6.f;
    static constexpr float kKnobR = 9.f;

    float trackLeft() const;
    float trackRight() const;
    float trackMidY() const;

    void buildShapes();
    void applyDrag(float mouseX);
    void updateKnob();
    void updateLabel();

    float m_min, m_max, m_value;
    bool m_dragging;

    sf::RoundedRectangleShape m_track;
    sf::RoundedRectangleShape m_fill;
    sf::CircleShape m_knob;
    std::optional<sf::Text> m_valueLabel;
};