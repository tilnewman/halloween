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
        explicit PlayState(const Context & t_context);
        ~PlayState() override = default;

        void onEnter(Context & t_context) override;
        void onExit(Context & t_context) override;
        bool handleEvent(Context & t_context, const sf::Event & t_event) override;
        void update(Context & t_context, const float t_frameTimeSec) override;

        void draw(
            const Context & t_context,
            sf::RenderTarget & t_target,
            sf::RenderStates & t_states) const override;
    };

} // namespace halloween

#endif // STATEPLAY_HPP_INCLUDED
