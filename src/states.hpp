#ifndef STATES_HPP_INCLUDED
#define STATES_HPP_INCLUDED
//
// states.hpp
//
#include "check-macros.hpp"
#include "context.hpp"

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <SFML/Graphics/Text.hpp>

namespace sf
{
    class Event;
    class RenderTarget;
    class RenderStates;
} // namespace sf

namespace halloween
{
    struct Context;

    //

    enum class State
    {
        Start = 0,
        Title,
        Play,
        Pause,
        Level,
        GameOver,
        Credits,
        Quit
    };

    using StateOpt_t = std::optional<State>;

    //

    namespace state
    {
        inline std::string toString(const State state)
        {
            // clang-format off
            switch (state)
            {
                case State::Start:     { return "Start";           }
                case State::Title:     { return "Title";           }
                case State::Play:      { return "Play";            }
                case State::Pause:     { return "Pause";           }
                case State::Level:     { return "Level";           }
                case State::GameOver:  { return "GameOver";        }
                case State::Credits:   { return "Credits";         }
                case State::Quit:      { return "Quit";            }
                default:               { assert(false); return ""; }
            }
            // clang-format on
        }
    } // namespace state

    inline std::ostream & operator<<(std::ostream & os, const State state)
    {
        os << state::toString(state);
        return os;
    }

    //

    struct IState
    {
        virtual ~IState() = default;

        virtual State state() const = 0;
        virtual State nextState() const = 0;
        virtual void update(Context &, const float frameTimeSec) = 0;
        virtual bool handleEvent(Context & context, const sf::Event & event) = 0;
        virtual void draw(const Context &, sf::RenderTarget &, sf::RenderStates &) const = 0;
        virtual void onEnter(Context &) = 0;
        virtual void onExit(Context &) = 0;

      protected:
        virtual bool changeToNextState(const Context &) = 0;
        virtual bool willIgnoreEvent(const Context &, const sf::Event & event) const = 0;

        // returns true if the event was a 'quit' event and a state changed is pending
        virtual bool handleQuitEvents(Context & context, const sf::Event & event) = 0;
    };

    using IStateUPtr_t = std::unique_ptr<IState>;

    //

    class StateBase : public IState
    {
      protected:
        StateBase(const State state, const State nextState, const float minDurationSec = -1.0f);

        StateBase(
            const Context & context,
            const State state,
            const State nextState,
            const std::string & message = {},
            const float minDurationSec = -1.0f);

      public:
        ~StateBase() override = default;

        // prevent all copy and assignment
        StateBase(const StateBase &) = delete;
        StateBase(StateBase &&) = delete;
        //
        StateBase & operator=(const StateBase &) = delete;
        StateBase & operator=(StateBase &&) = delete;

        inline State state() const final { return m_state; }
        inline State nextState() const final { return m_nextState; }
        void update(Context &, const float frameTimeSec) override;
        bool handleEvent(Context &, const sf::Event &) override;
        void draw(const Context &, sf::RenderTarget &, sf::RenderStates &) const override;
        void onEnter(Context &) override {}
        void onExit(Context &) override {}

      protected:
        inline bool hasMinTimeElapsed() const
        {
            return (!(m_minDurationSec > 0.0f) || (m_elapsedTimeSec > m_minDurationSec));
        }

        bool changeToNextState(const Context &) override;
        bool willIgnoreEvent(const Context &, const sf::Event &) const override;
        bool handleQuitEvents(Context &, const sf::Event &) override;
        void setupText(const Context &, const std::string &);

      protected:
        State m_state;
        State m_nextState;
        float m_elapsedTimeSec;
        float m_minDurationSec; // any negative means this value is ignored
        sf::Text m_text;

        static inline const sf::Color m_textColorDefault{ sf::Color(200, 200, 200) };
        static inline const float m_defaultMinDurationSec{ 1.5f };
    };

    //

    struct StartState : public StateBase
    {
        StartState()
            : StateBase(State::Start, State::Title)
        {}

        ~StartState() override = default;

        void onEnter(Context &) override;
        void onExit(Context &) override;
        void update(Context &, const float) final {}
        bool handleEvent(Context &, const sf::Event &) final { return false; }
        void draw(const Context &, sf::RenderTarget &, sf::RenderStates &) const final {}
    };

    //

    struct QuitState : public StateBase
    {
        QuitState()
            : StateBase(State::Quit, State::Quit)
        {}

        ~QuitState() override = default;

        void onEnter(Context &) override;
        void update(Context &, const float) final {}
        bool handleEvent(Context &, const sf::Event &) final { return false; }
        void draw(const Context &, sf::RenderTarget &, sf::RenderStates &) const final {}
    };

    //

    class PlayState : public StateBase
    {
      public:
        explicit PlayState(const Context & context);
        ~PlayState() override = default;

        void onEnter(Context &) override;
        void onExit(Context &) override;
        bool handleEvent(Context &, const sf::Event &) override;
        void update(Context &, const float FRAME_TIME_SEC) override;
        void draw(const Context &, sf::RenderTarget &, sf::RenderStates &) const override;
    };

    //

    struct TimedMessageState : public StateBase
    {
        TimedMessageState(
            const Context & context,
            const State state,
            const State nextState,
            const std::string & message,
            const float minDurationSec = StateBase::m_defaultMinDurationSec);

        ~TimedMessageState() override = default;

        void update(Context &, const float FRAME_TIME_SEC) override;
        bool handleEvent(Context &, const sf::Event & event) override;

      protected:
        bool m_hasMouseClickedOrKeyPressed{ false };
    };

    //

    struct TitleState : public TimedMessageState
    {
        explicit TitleState(const Context & context);
        ~TitleState() override = default;

        void onEnter(Context & context) override;
        void draw(const Context & con, sf::RenderTarget & rt, sf::RenderStates & st) const override;

      private:
        sf::Text m_text1;
        sf::Text m_text2;
        sf::Text m_text3;
        sf::Text m_text4;
    };

    //

    struct PauseState : public TimedMessageState
    {
        explicit PauseState(const Context & context);
        ~PauseState() override = default;

        void onEnter(Context &) override;
        void onExit(Context &) override;
        void update(Context &, const float FRAME_TIME_SEC) override;
        void draw(const Context &, sf::RenderTarget &, sf::RenderStates &) const override;
    };

    //

    struct GameOverState : public TimedMessageState
    {
        explicit GameOverState(const Context & context);
        ~GameOverState() override = default;

        void onEnter(Context &) override;
    };

    //

    struct LevelCompleteState : public TimedMessageState
    {
        explicit LevelCompleteState(const Context & context);
        ~LevelCompleteState() override = default;

        void onEnter(Context &) override;
        void onExit(Context &) override;
        bool handleEvent(Context &, const sf::Event & event) override;
    };

    //

    struct IStatesPending
    {
        virtual ~IStatesPending() = default;

        virtual bool isChangePending() const = 0;
        virtual StateOpt_t getChangePending() const = 0;
        virtual void setChangePending(const State state) = 0;
    };

    //

    class StateMachine : public IStatesPending
    {
      public:
        StateMachine();
        ~StateMachine() override = default;

        // prevent all copy and assignment
        StateMachine(const StateMachine &) = delete;
        StateMachine(StateMachine &&) = delete;
        //
        StateMachine & operator=(const StateMachine &) = delete;
        StateMachine & operator=(StateMachine &&) = delete;

        void reset();

        inline State stateEnum() const { return m_stateUPtr->state(); }

        inline IState & state() { return *m_stateUPtr; }
        inline const IState & state() const { return *m_stateUPtr; }

        inline bool isChangePending() const override { return m_changePendingOpt.has_value(); }
        inline StateOpt_t getChangePending() const override { return m_changePendingOpt; }
        void setChangePending(const State state) override;
        void changeIfPending(Context & context);

      private:
        static IStateUPtr_t makeState(Context & context, const State state);

      private:
        IStateUPtr_t m_stateUPtr;
        StateOpt_t m_changePendingOpt;
    };

} // namespace halloween

#endif // STATES_HPP_INCLUDED
