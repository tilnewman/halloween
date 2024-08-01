#ifndef SFMLSLIDERS_HPP_INCLUDED
#define SFMLSLIDERS_HPP_INCLUDED
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
            const Random & random,
            const sf::FloatRect & region,
            const std::pair<float, float> & speed);

        const sf::Vector2f position() const;

        void update(const Random & random, const float frameTimeSec);

      private:
        SliderDrift<float, float> m_horizDrifter;
        SliderDrift<float, float> m_vertDrifter;
    };

} // namespace util

#endif // SFMLSLIDERS_HPP_INCLUDED
