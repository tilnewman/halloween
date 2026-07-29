#ifndef STATES_HPP_INCLUDED
#define STATES_HPP_INCLUDED
//
// states.hpp
//
#include "pause-screen.hpp"

#include <cassert>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
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

    enum class State : unsigned char
    {
        Start,
        Title,
        Play,
        Pause,
        Level,
        Lose,
        Win,
        Credits,
        Quit
    };

    using StateOpt_t = std::optional<State>;

    //

    namespace state // TODO we don't need this anymore right?
    {
        inline constexpr std::string_view toString(const State t_state) noexcept
        {
            // clang-format off
            switch (t_state)
            {
                case State::Start:     { return "Start";           }
                case State::Title:     { return "Title";           }
                case State::Play:      { return "Play";            }
                case State::Pause:     { return "Pause";           }
                case State::Level:     { return "Level";           }
                case State::Lose:      { return "Lose";            }
                case State::Win:       { return "Win";             }
                case State::Credits:   { return "Credits";         }
                case State::Quit:      { return "Quit";            }
                default:               { return "Unknown_State";   }
            }
            // clang-format on
        }
    } // namespace state

    inline std::ostream & operator<<(std::ostream & t_os, const State t_state)
    {
        t_os << state::toString(t_state);
        return t_os;
    }

    //

    struct IState
    {
        virtual ~IState() = default;

        virtual State state() const = 0;
        virtual State nextState() const = 0;
        virtual void update(Context & t_context, const float t_frameTimeSec) = 0;
        virtual bool handleEvent(Context & t_context, const sf::Event & t_event) = 0;
        virtual void draw(
            const Context & t_context,
            sf::RenderTarget & t_target,
            sf::RenderStates & t_states) const = 0;

        virtual void onEnter(Context & t_context) = 0;
        virtual void onExit(Context & t_context) = 0;

      protected:
        virtual bool changeToNextState(const Context & t_context) = 0;

        virtual bool
            willIgnoreEvent(const Context & t_context, const sf::Event & t_event) const = 0;

        // returns true if the event was a 'quit' event and a state changed is pending
        virtual bool handleQuitEvents(Context & t_context, const sf::Event & t_event) = 0;
    };

    using IStateUPtr_t = std::unique_ptr<IState>;

    //

    class StateBase : public IState
    {
      protected:
        StateBase(
            const State t_state, const State t_nextState, const float t_minDurationSec = -1.0f);

        StateBase(
            const Context & t_context,
            const State t_state,
            const State t_nextState,
            const std::string & t_message = {},
            const float t_minDurationSec = -1.0f);

      public:
        ~StateBase() override = default;

        // prevent all copy and assignment
        StateBase(const StateBase &) = delete;
        StateBase(StateBase &&) = delete;
        //
        StateBase & operator=(const StateBase &) = delete;
        StateBase & operator=(StateBase &&) = delete;

        State state() const final { return m_state; }
        State nextState() const final { return m_nextState; }
        void update(Context & t_context, const float t_frameTimeSec) override;
        bool handleEvent(Context & t_context, const sf::Event & t_event) override;

        void draw(
            const Context & t_context,
            sf::RenderTarget & t_target,
            sf::RenderStates & t_states) const override;

        void onEnter(Context &) override {}
        void onExit(Context &) override {}

      protected:
        inline bool hasMinTimeElapsed() const
        {
            return (!(m_minDurationSec > 0.0f) || (m_elapsedTimeSec > m_minDurationSec));
        }

        bool changeToNextState(const Context & t_context) override;
        bool willIgnoreEvent(const Context & t_context, const sf::Event & t_event) const override;
        bool handleQuitEvents(Context & t_context, const sf::Event & t_event) override;
        void setupText(const Context & t_context, const std::string & t_text);

      protected:
        State m_state;
        State m_nextState;
        float m_elapsedTimeSec;
        float m_minDurationSec; // any negative means this value is ignored
        sf::Text m_text;

        static inline const sf::Color m_textColorDefault{ sf::Color(220, 220, 220) };
        static inline const float m_defaultMinDurationSec{ 1.5f };
    };

    //

    struct StartState : public StateBase
    {
        StartState()
            : StateBase{ State::Start, State::Title }
        {}

        ~StartState() override = default;

        void onEnter(Context & t_context) override;
        void onExit(Context & t_context) override;
        void update(Context &, const float) final {}
        bool handleEvent(Context &, const sf::Event &) final { return false; }
        void draw(const Context &, sf::RenderTarget &, sf::RenderStates &) const final {}
    };

    //

    struct QuitState : public StateBase
    {
        QuitState()
            : StateBase{ State::Quit, State::Quit }
        {}

        ~QuitState() override = default;

        void onEnter(Context &) final {}
        void update(Context &, const float) final {}
        bool handleEvent(Context &, const sf::Event &) final { return false; }
        void draw(const Context &, sf::RenderTarget &, sf::RenderStates &) const final {}
    };

    //

    struct TimedMessageState : public StateBase
    {
        TimedMessageState(
            const Context & t_context,
            const State t_state,
            const State t_nextState,
            const std::string & t_message,
            const float t_minDurationSec = StateBase::m_defaultMinDurationSec);

        ~TimedMessageState() override = default;

        void update(Context & t_context, const float t_frameTimeSec) override;
        bool handleEvent(Context & t_context, const sf::Event & t_event) override;

      protected:
        bool m_hasMouseClickedOrKeyPressed{ false };
    };

    //

    struct TitleState : public TimedMessageState
    {
        explicit TitleState(const Context & t_context);
        ~TitleState() override = default;

        void onEnter(Context & t_context) override;

        void draw(
            const Context & t_context,
            sf::RenderTarget & t_target,
            sf::RenderStates & t_states) const override;

      private:
        sf::Text m_text1;
        sf::Text m_text2;
        sf::Text m_text3;
        sf::Text m_text4;
    };

    //

    struct PauseState : public TimedMessageState
    {
        explicit PauseState(const Context & t_context);
        ~PauseState() override = default;

        void onEnter(Context & t_context) override;
        void onExit(Context & t_context) override;
        void update(Context & t_context, const float t_frameTimeSec) override;

        void draw(
            const Context & t_context,
            sf::RenderTarget & t_target,
            sf::RenderStates & t_states) const override;

        PauseScreen screen;
    };

    //

    struct LoseState : public TimedMessageState
    {
        explicit LoseState(const Context & t_context);
        ~LoseState() override = default;

        void onEnter(Context & t_context) override;
        void onExit(Context & t_context) override;

        void draw(
            const Context & t_context,
            sf::RenderTarget & t_target,
            sf::RenderStates & t_states) const override;

      private:
        sf::Text m_scoreText;
    };

    //

    struct WinState : public TimedMessageState
    {
        explicit WinState(const Context & t_context);
        ~WinState() override = default;

        void onEnter(Context & t_context) override;
        void onExit(Context & t_context) override;

        void draw(
            const Context & t_context,
            sf::RenderTarget & t_target,
            sf::RenderStates & t_states) const override;

      private:
        sf::Text m_scoreText;
    };

} // namespace halloween

#endif // STATES_HPP_INCLUDED
