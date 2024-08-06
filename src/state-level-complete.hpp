#ifndef STATELEVELCOMPLETE_HPP_INCLUDED
#define STATELEVELCOMPLETE_HPP_INCLUDED
//
// state-level-complete.hpp
//
#include "states.hpp"

#include <string>
#include <vector>

#include <SFML/Graphics/Text.hpp>

//

namespace sf
{
    class Event;
    class RenderTarget;
    class RenderStates;
} // namespace sf

//

namespace halloween
{
    struct Context;

    //

    struct Bonus
    {
        Bonus()
            : score(0)
            , text()
        {}

        Bonus(const int s, const sf::Text & t)
            : score(s)
            , text(t)
        {}

        int score;
        sf::Text text;
    };

    //

    class LevelCompleteState : public StateBase
    {
      public:
        explicit LevelCompleteState(const Context & context);
        ~LevelCompleteState() override = default;

        void onEnter(Context & context) final;
        void onExit(Context & context) final;
        bool handleEvent(Context & context, const sf::Event & event) final;
        void update(Context & context, const float frameTimeSec) final;
        void draw(const Context & c, sf::RenderTarget & t, sf::RenderStates & s) const final;

      private:
        bool popAndDisplayNextBonus(Context & context);
        void updateScoreText(const Context & context);

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
