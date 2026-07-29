#ifndef SFML_SLIDERS_HPP_INCLUDED
#define SFML_SLIDERS_HPP_INCLUDED
//
// sfml-sliders.hpp
//
#include "sliders.hpp"

#include <utility>

#include <SFML/Graphics/Rect.hpp>

namespace util
{

    class PositionDrifter
    {
      public:
        PositionDrifter();

        void setup(
            const Random & t_random,
            const sf::FloatRect & t_region,
            const std::pair<float, float> & t_speed);

        const sf::Vector2f position() const;

        void update(const Random & t_random, const float t_frameTimeSec);

      private:
        SliderDrift<float, float> m_horizDrifter;
        SliderDrift<float, float> m_vertDrifter;
    };

} // namespace util

#endif // SFML_SLIDERS_HPP_INCLUDED
