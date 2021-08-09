#ifndef SCREEN_REGIONS_HPP_INCLUDED
#define SCREEN_REGIONS_HPP_INCLUDED
//
// screen-regions.hpp
//
#include <vector>

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Vertex.hpp>

namespace slnghn
{
    struct Context;
    struct Resources;

    class ScreenRegions
    {
      public:
        ScreenRegions();

        void setup(Context & context, const sf::Vector2u & WINDOW_SIZE);

        sf::Vector2f wholeSize() const { return m_wholeSize; }
        sf::FloatRect wholeRegion() const { return m_wholeRegion; }
        sf::FloatRect mapRegion() const { return m_mapRegion; }
        sf::FloatRect infoRegion() const { return m_infoRegion; }

      private:
        sf::Vector2f m_wholeSize;
        sf::FloatRect m_wholeRegion;
        sf::FloatRect m_mapRegion;
        sf::FloatRect m_infoRegion;
    };

} // namespace slnghn

#endif // SCREEN_REGIONS_HPP_INCLUDED
