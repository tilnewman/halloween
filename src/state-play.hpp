#ifndef STATEPLAY_HPP_INCLUDED
#define STATEPLAY_HPP_INCLUDED
//
// state-play.hpp
//
#include "states.hpp"

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

    class PlayState : public StateBase
    {
      public:
        explicit PlayState(const Context & context);
        ~PlayState() override = default;

        void onEnter(Context & context) override;
        void onExit(Context & context) override;
        bool handleEvent(Context & context, const sf::Event &) override;
        void update(Context & context, const float frameTimeSec) override;
        void draw(const Context & c, sf::RenderTarget & t, sf::RenderStates & s) const override;
    };

} // namespace halloween

#endif // STATEPLAY_HPP_INCLUDED
