#ifndef SCREEN_REGIONS_HPP_INCLUDED
#define SCREEN_REGIONS_HPP_INCLUDED
//
// screen-regions.hpp
//
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Vertex.hpp>

namespace halloween
{
    struct Context;
    struct Resources;

    class ScreenRegions
    {
      public:
        ScreenRegions();

        void setup(const sf::Vector2u & windowSize);

        inline const sf::Vector2f wholeSize() const { return m_wholeSize; }
        inline const sf::FloatRect wholeRegion() const { return m_wholeRegion; }
        inline const sf::FloatRect mapRegion() const { return m_mapRegion; }
        inline const sf::FloatRect infoRegion() const { return m_infoRegion; }

      private:
        sf::Vector2f m_wholeSize;
        sf::FloatRect m_wholeRegion;
        sf::FloatRect m_mapRegion;
        sf::FloatRect m_infoRegion;
    };

} // namespace halloween

#endif // SCREEN_REGIONS_HPP_INCLUDED
