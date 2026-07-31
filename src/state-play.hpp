#ifndef STATEPLAY_HPP_INCLUDED
#define STATEPLAY_HPP_INCLUDED
//
// state-play.hpp
//
#include "background-image.hpp"
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

    class PlayState final : public StateBase
    {
      public:
        PlayState();
        ~PlayState() final = default;

        void onEnter(const Context & t_context) override;
        void onExit(const Context & t_context) override;
        bool handleEvent(const Context & t_context, const sf::Event & t_event) override;
        void update(const Context & t_context, const float t_frameTimeSec) override;

        void draw(const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states)
            const override;

        inline static bool m_willLoadNewLevel{ true };

      private:
        BackgroundImage m_backgroundImage;
    };

} // namespace halloween

#endif // STATEPLAY_HPP_INCLUDED
