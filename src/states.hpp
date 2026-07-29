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
        virtual void update(const Context & t_context, const float t_frameTimeSec) = 0;
        virtual bool handleEvent(const Context & t_context, const sf::Event & t_event) = 0;
        virtual void draw(
            const Context & t_context,
            sf::RenderTarget & t_target,
            sf::RenderStates & t_states) const = 0;

        virtual void onEnter(const Context & t_context) = 0;
        virtual void onExit(const Context & t_context) = 0;

      protected:
        virtual bool changeToNextState(const Context & t_context) = 0;

        virtual bool
            willIgnoreEvent(const Context & t_context, const sf::Event & t_event) const = 0;

        // returns true if the event was a 'quit' event and a state changed is pending
        virtual bool handleQuitEvents(const Context & t_context, const sf::Event & t_event) = 0;
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
        void update(const Context & t_context, const float t_frameTimeSec) override;
        bool handleEvent(const Context & t_context, const sf::Event & t_event) override;

        void draw(
            const Context & t_context,
            sf::RenderTarget & t_target,
            sf::RenderStates & t_states) const override;

        void onEnter(const Context &) override {}
        void onExit(const Context &) override {}

      protected:
        inline bool hasMinTimeElapsed() const
        {
            return (!(m_minDurationSec > 0.0f) || (m_elapsedTimeSec > m_minDurationSec));
        }

        bool changeToNextState(const Context & t_context) override;
        bool willIgnoreEvent(const Context & t_context, const sf::Event & t_event) const override;
        bool handleQuitEvents(const Context & t_context, const sf::Event & t_event) override;
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

    struct StartState final : public StateBase
    {
        StartState()
            : StateBase{ State::Start, State::Title }
        {}

        ~StartState() final = default;

        void onEnter(const Context & t_context) final;
        void onExit(const Context & t_context) final;
        void update(const Context &, const float) final {}
        bool handleEvent(const Context &, const sf::Event &) final { return false; }
        void draw(const Context &, sf::RenderTarget &, sf::RenderStates &) const final {}
    };

    //

    struct QuitState final : public StateBase
    {
        QuitState()
            : StateBase{ State::Quit, State::Quit }
        {}

        ~QuitState() final = default;

        void onEnter(const Context &) final {}
        void update(const Context &, const float) final {}
        bool handleEvent(const Context &, const sf::Event &) final { return false; }
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

        void update(const Context & t_context, const float t_frameTimeSec) override;
        bool handleEvent(const Context & t_context, const sf::Event & t_event) override;

      protected:
        bool m_hasMouseClickedOrKeyPressed{ false };
    };

    //

    struct TitleState final : public TimedMessageState
    {
        explicit TitleState(const Context & t_context);
        ~TitleState() final = default;

        void onEnter(const Context & t_context) final;

        void draw(
            const Context & t_context,
            sf::RenderTarget & t_target,
            sf::RenderStates & t_states) const final;

      private:
        sf::Text m_text1;
        sf::Text m_text2;
        sf::Text m_text3;
        sf::Text m_text4;
    };

    //

    struct PauseState final : public TimedMessageState
    {
        explicit PauseState(const Context & t_context);
        ~PauseState() final = default;

        void onEnter(const Context & t_context) final;
        void onExit(const Context & t_context) final;
        void update(const Context & t_context, const float t_frameTimeSec) final;

        void draw(
            const Context & t_context,
            sf::RenderTarget & t_target,
            sf::RenderStates & t_states) const final;

      private:
        PauseScreen screen;
    };

    //

    struct LoseState final : public TimedMessageState
    {
        explicit LoseState(const Context & t_context);
        ~LoseState() final = default;

        void onEnter(const Context & t_context) final;
        void onExit(const Context & t_context) final;

        void draw(
            const Context & t_context,
            sf::RenderTarget & t_target,
            sf::RenderStates & t_states) const final;

      private:
        sf::Text m_scoreText;
    };

    //

    struct WinState final : public TimedMessageState
    {
        explicit WinState(const Context & t_context);
        ~WinState() final = default;

        void onEnter(const Context & t_context) final;
        void onExit(const Context & t_context) final;

        void draw(
            const Context & t_context,
            sf::RenderTarget & t_target,
            sf::RenderStates & t_states) const final;

      private:
        sf::Text m_scoreText;
    };

} // namespace halloween

#endif // STATES_HPP_INCLUDED
