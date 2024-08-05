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
        sf::Text m_levelCompleteText;
        sf::Text m_scoreText;
    };

} // namespace halloween

#endif // STATELEVELCOMPLETE_HPP_INCLUDED
