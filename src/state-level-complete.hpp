#ifndef STATELEVELCOMPLETE_HPP_INCLUDED
#define STATELEVELCOMPLETE_HPP_INCLUDED
//
// state-level-complete.hpp
//
#include "sfml-defaults.hpp"
#include "states.hpp"

#include <string>
#include <vector>

#include <SFML/Graphics/Text.hpp>

//

namespace sf
{
    class Event;
    class RenderTarget;
    struct RenderStates;
} // namespace sf

//

namespace halloween
{
    struct Context;

    //

    struct Bonus
    {
        Bonus()
            : score{ 0 }
            , text{ util::SfmlDefaults::instance().font() }
        {}

        Bonus(const int t_score, const sf::Text & t_text)
            : score{ t_score }
            , text{ t_text }
        {}

        int score;
        sf::Text text;
    };

    //

    class LevelCompleteState final: public StateBase
    {
      public:
        explicit LevelCompleteState(const Context & context);
        ~LevelCompleteState() final = default;

        void onEnter(const Context & t_context) final;
        void onExit(const Context & t_context) final;
        bool handleEvent(const Context & t_context, const sf::Event & t_event) final;
        void update(const Context & t_context, const float t_frameTimeSec) final;

        void draw(
            const Context & t_context,
            sf::RenderTarget & t_target,
            sf::RenderStates & t_states) const final;

      private:
        bool popAndDisplayNextBonus(const Context & t_context);
        void updateScoreText(const Context & t_context);

      private:
        sf::Text m_levelCompleteText;
        sf::Text m_scoreText;
        sf::Text m_bonusText;
        std::vector<Bonus> m_bonuses;
        sf::FloatRect m_bonusTextRegion;

        bool m_isPreWaiting;
        bool m_isShowingBonuses;
        bool m_isPostWaiting;
        const float m_timeBetweenScoreUpdateSec;
        int m_scoreDisplayed;
    };

} // namespace halloween

#endif // STATELEVELCOMPLETE_HPP_INCLUDED
