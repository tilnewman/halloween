#ifndef INFO_REGION_HPP_INCLUDED
#define INFO_REGION_HPP_INCLUDED
//
// info-region.hpp
//
#include <vector>

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Vertex.hpp>

namespace halloween
{
    struct Context;

    class InfoRegion
    {
      public:
        InfoRegion();

        void setup(Context & context);
        void reset();
        void draw(sf::RenderTarget & target, sf::RenderStates states) const;

        inline int score() const { return m_score; }
        void scoreAdjust(const int ADJ);

        inline int lives() const { return m_lives; }
        void livesAdjust(const int ADJ);

      private:
        void updateText();

      private:
        int m_score;
        int m_lives;
        sf::Text m_text;
        sf::FloatRect m_region;
        std::vector<sf::Vertex> m_bgVerts;
    };

} // namespace halloween

#endif // INFO_REGION_HPP_INCLUDED
