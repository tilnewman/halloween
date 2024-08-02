#ifndef STATES_HPP_INCLUDED
#define STATES_HPP_INCLUDED
//
// states.hpp
//
#include <cassert>
#include <memory>
#include <optional>
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
        virtual void update(Context & context, const float frameTimeSec) = 0;
        virtual bool handleEvent(Context & context, const sf::Event & event) = 0;
        virtual void draw(const Context & c, sf::RenderTarget & t, sf::RenderStates & s) const = 0;
        virtual void onEnter(Context & context) = 0;
        virtual void onExit(Context & context) = 0;

      protected:
        virtual bool changeToNextState(const Context & context) = 0;
        virtual bool willIgnoreEvent(const Context & context, const sf::Event & event) const = 0;

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

        State state() const final { return m_state; }
        State nextState() const final { return m_nextState; }
        void update(Context & context, const float frameTimeSec) override;
        bool handleEvent(Context & context, const sf::Event & event) override;
        void draw(const Context & c, sf::RenderTarget & t, sf::RenderStates & s) const override;
        void onEnter(Context &) override {}
        void onExit(Context &) override {}

      protected:
        inline bool hasMinTimeElapsed() const
        {
            return (!(m_minDurationSec > 0.0f) || (m_elapsedTimeSec > m_minDurationSec));
        }

        bool changeToNextState(const Context & context) override;
        bool willIgnoreEvent(const Context & context, const sf::Event & event) const override;
        bool handleQuitEvents(Context & context, const sf::Event & event) override;
        void setupText(const Context & context, const std::string & text);

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
            : StateBase(State::Start, State::Title)
        {}

        ~StartState() override = default;

        void onEnter(Context & context) override;
        void onExit(Context & context) override;
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

        void onEnter(Context & context) override;
        void update(Context &, const float) final {}
        bool handleEvent(Context &, const sf::Event &) final { return false; }
        void draw(const Context &, sf::RenderTarget &, sf::RenderStates &) const final {}
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

        void update(Context & context, const float frameTimeSec) override;
        bool handleEvent(Context & context, const sf::Event & event) override;

      protected:
        bool m_hasMouseClickedOrKeyPressed{ false };
    };

    //

    struct TitleState : public TimedMessageState
    {
        explicit TitleState(const Context & context);
        ~TitleState() override = default;

        void onEnter(Context & context) override;
        void draw(const Context & c, sf::RenderTarget & t, sf::RenderStates & s) const override;

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

        void onEnter(Context & context) override;
        void onExit(Context & context) override;
        void update(Context & context, const float frameTimeSec) override;
        void draw(const Context & c, sf::RenderTarget & t, sf::RenderStates & s) const override;
    };

    //

    struct GameOverState : public TimedMessageState
    {
        explicit GameOverState(const Context & context);
        ~GameOverState() override = default;

        void onEnter(Context & context) override;
        void onExit(Context & context) override;
        void draw(const Context & c, sf::RenderTarget & t, sf::RenderStates & s) const override;

      private:
        sf::Text m_scoreText;
    };

    //

    struct LevelCompleteState : public TimedMessageState
    {
        explicit LevelCompleteState(const Context & context);
        ~LevelCompleteState() override = default;

        void onEnter(Context & context) override;
        void onExit(Context & context) override;
        bool handleEvent(Context & context, const sf::Event & event) override;
    };

} // namespace halloween

#endif // STATES_HPP_INCLUDED
