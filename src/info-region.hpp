#ifndef INFO_REGION_HPP_INCLUDED
#define INFO_REGION_HPP_INCLUDED
//
// info-region.hpp
//
#include <vector>

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderStates.hpp>
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
        void reset(Context & context);
        void update(Context & context, const float frameTimeSec);
        void draw(sf::RenderTarget & target, sf::RenderStates states) const;

        inline int score() const { return m_score; }
        void scoreAdjust(const int adj);

        inline int lives() const { return m_lives; }
        void livesAdjust(const int adj);

        inline int darts() const { return m_dartCount; }
        void dartsAdjust(const int adj);

      private:
        void updateText();

      private:
        int m_score;
        int m_visibleScore;
        float m_elapsedTimeSec;
        float m_timePerScoreUpdate;
        int m_lives;
        int m_dartCount;
        sf::Text m_text;
        sf::FloatRect m_region;
        std::vector<sf::Vertex> m_bgVerts;
    };

} // namespace halloween

#endif // INFO_REGION_HPP_INCLUDED
