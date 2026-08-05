#ifndef INFO_REGION_HPP_INCLUDED
#define INFO_REGION_HPP_INCLUDED
//
// info-region.hpp
//
#include <vector>

#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Vertex.hpp>

namespace sf
{
    class RenderTarget;
}

namespace halloween
{
    struct Context;

    class InfoRegion
    {
      public:
        InfoRegion();

        void setup(const Context & t_context);
        void reset(const Context & t_context);
        void update(const Context & t_context, const float t_frameTimeSec);
        void draw(sf::RenderTarget & t_target, sf::RenderStates t_states) const;

        constexpr int score() const noexcept { return m_score; }
        void scoreAdjust(const int t_adustment);

        constexpr int lives() const noexcept { return m_lives; }
        void livesAdjust(const int t_adjustment);

        constexpr int darts() const noexcept { return m_dartCount; }
        void dartsAdjust(const int t_adjustment);

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
