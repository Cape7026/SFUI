

#pragma once

#include "SFML/Graphics.hpp"
#include <math.h>
#include <numbers>

namespace sf
{
    ////////////////////////////////////////////////////////////
    /// \brief Specialized shape representing a rounded rectangle
    ///
    ////////////////////////////////////////////////////////////
    class RoundedRectangleShape : public sf::Shape
    {
    public:
        virtual size_t getPointCount() const override
        {
            return m_cornerPointCount * 4;
        }

        virtual sf::Vector2f getPoint(size_t index) const override
        {
            if (m_cornerPointCount == 1 || m_radius == 0.0f)
            {
                sf::RectangleShape rect(m_size);
                return rect.getPoint(index);
            }
            else
            {
                size_t corner = index / m_cornerPointCount;
                float angle = 0;
                bool isLastPointOnCorner = (index % m_cornerPointCount == m_cornerPointCount - 1);
                if (isLastPointOnCorner)
                {
                    angle = 90.0f;
                }
                else
                {
                    angle = (index % m_cornerPointCount) * m_anglePerPoint;
                }
                sf::Vector2f point;
                switch (corner)
                {
                case 0:
                    point = {m_radius, m_radius};
                    angle -= 180;
                    break;
                case 1:
                    point = {m_size.x - m_radius, m_radius};
                    angle -= 90;
                    break;
                case 2:
                    point = {m_size.x - m_radius, m_size.y - m_radius};
                    break;
                default:
                    point = {m_radius, m_size.y - m_radius};
                    angle += 90;
                }
                float radians = angle * std::numbers::pi_v<float> / 180.0f;
                point += {
                    std::cos(angle * std::numbers::pi_v<float> / 180.0f) * m_radius,
                    std::sin(angle * std::numbers::pi_v<float> / 180.0f) * m_radius};
                return point;
            }
        }

        void setSize(sf::Vector2f size)
        {
            m_size = size;
            update();
        }

        sf::Vector2f getSize()
        {
            return m_size;
        }

        void setRadius(float radius)
        {
            m_radius = std::clamp(radius, 0.f, std::min(m_size.x, m_size.y) / 2.f);
            update();
        }

        float getRadius()
        {
            return m_radius;
        }

        int getCornerPointCount()
        {
            return m_cornerPointCount;
        }

        void setCornerPointCount(float points)
        {
            m_cornerPointCount = points;
            m_anglePerPoint = 90.0f / points;
            update();
        }

    private:
        // TODO: Per edge rounding
        size_t m_cornerPointCount = 2;
        float m_anglePerPoint;
        float m_radius = 0.0f;
        sf::Vector2f m_size;
    };

}
