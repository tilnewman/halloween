// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// states.cpp
//
#include "states.hpp"

#include "coin.hpp"
#include "context.hpp"
#include "ghost.hpp"
#include "info-region.hpp"
#include "missile.hpp"
#include "pause-screen.hpp"
#include "resources.hpp"
#include "screen-regions.hpp"
#include "sfml-util.hpp"
#include "slime.hpp"
#include "sound-player.hpp"
#include "state-machine.hpp"

#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Window.hpp>

namespace halloween
{
    StateBase::StateBase(const State state, const State nextState, const float minDurationSec)
        : m_state(state)
        , m_nextState(nextState)
        , m_elapsedTimeSec(0.0f)
        , m_minDurationSec(minDurationSec) // any negative means this value is ignored
        , m_text()
    {}

    StateBase::StateBase(
        const Context & context,
        const State state,
        const State nextState,
        const std::string & message,
        const float minDurationSec)
        : StateBase(state, nextState, minDurationSec)
    {
        setupText(context, message);
    }

    void StateBase::setupText(const Context & context, const std::string & message)
    {
        m_text.setString(message);
        m_text.setCharacterSize(99);
        m_text.setFont(context.media.font);
        m_text.setFillColor(m_textColorDefault);

        util::fitAndCenterInside(
            m_text, util::scaleRectInPlaceCopy(context.layout.wholeRegion(), 0.25f));
    }

    void StateBase::update(Context &, const float frameTimeSec)
    {
        m_elapsedTimeSec += frameTimeSec;
    }

    bool StateBase::changeToNextState(const Context & context)
    {
        if (state() == m_nextState)
        {
            return false;
        }

        context.state.setChangePending(nextState());
        return true;
    }

    bool StateBase::willIgnoreEvent(const Context & context, const sf::Event & event) const
    {
        // all events should be ignored after a state change is scheduled
        if (context.state.isChangePending())
        {
            return true;
        }

        // clang-format off
        return ((sf::Event::KeyReleased == event.type) ||
                (sf::Event::MouseMoved == event.type) ||
                (sf::Event::MouseEntered == event.type) ||
                (sf::Event::MouseLeft == event.type) ||
                (sf::Event::MouseButtonReleased == event.type) ||
                (sf::Event::GainedFocus == event.type) ||
                (sf::Event::LostFocus == event.type) ||
                (sf::Event::TouchBegan == event.type) ||
                (sf::Event::TouchEnded == event.type) ||
                (sf::Event::TouchMoved == event.type));
        // clang-format on
    }

    bool StateBase::handleQuitEvents(Context & context, const sf::Event & event)
    {
        if (sf::Event::Closed == event.type)
        {
            context.state.setChangePending(State::Quit);
            return true;
        }

        if (state() == State::Pause)
        {
            context.state.setChangePending(State::Play);
            return true;
        }

        // all that remain are keystrokes
        if (sf::Event::KeyPressed != event.type)
        {
            return false;
        }

        if (sf::Keyboard::Q == event.key.code)
        {
            if (state() == State::Play)
            {
                context.state.setChangePending(State::GameOver);
            }
            else
            {
                context.state.setChangePending(State::Quit);
            }

            return true;
        }

        if (sf::Keyboard::Escape == event.key.code)
        {
            context.state.setChangePending(State::Quit);
            return true;
        }

        return false;
    }

    bool StateBase::handleEvent(Context & context, const sf::Event & event)
    {
        if (willIgnoreEvent(context, event))
        {
            return true;
        }

        if (handleQuitEvents(context, event))
        {
            return true;
        }

        return context.state.isChangePending();
    }

    void
        StateBase::draw(const Context &, sf::RenderTarget & target, sf::RenderStates & states) const
    {
        target.draw(m_text, states);
    }

    //

    void StartState::onEnter(Context & context) { changeToNextState(context); }

    void StartState::onExit(Context & context) { context.audio.play("respawn"); }

    //

    void QuitState::onEnter(Context & context) { context.will_quit = true; }

    //

    TimedMessageState::TimedMessageState(
        const Context & context,
        const State state,
        const State nextState,
        const std::string & message,
        const float minDurationSec)
        : StateBase(context, state, nextState, message, minDurationSec)
    {
        const sf::FloatRect textBounds{ util::scaleRectInPlaceCopy(
            context.layout.mapRegion(), 0.9f) };

        util::centerInside(m_text, textBounds);
    }

    bool TimedMessageState::handleEvent(Context & context, const sf::Event & event)
    {
        if (StateBase::handleEvent(context, event))
        {
            return true;
        }

        if ((event.type == sf::Event::KeyPressed) || (event.type == sf::Event::MouseButtonPressed))
        {
            m_hasMouseClickedOrKeyPressed = true;
        }

        if (!hasMinTimeElapsed())
        {
            return false;
        }

        if (m_hasMouseClickedOrKeyPressed)
        {
            changeToNextState(context);
            return true;
        }

        return false;
    }

    void TimedMessageState::update(Context & context, const float frameTimeSec)
    {
        StateBase::update(context, frameTimeSec);

        if (hasMinTimeElapsed() || m_hasMouseClickedOrKeyPressed)
        {
            changeToNextState(context);
        }
    }

    //

    TitleState::TitleState(const Context & context)
        : TimedMessageState(
              context, State::Title, State::Play, "", (m_defaultMinDurationSec * 2.0f))
        , m_text1()
        , m_text2()
        , m_text3()
        , m_text4()
    {}

    void TitleState::onEnter(Context & context)
    {
        m_text1.setFont(context.media.font);
        m_text1.setFillColor(sf::Color::White);
        m_text1.setCharacterSize(99);
        m_text1.setString("Super Lucky");

        const sf::FloatRect wholeRect = context.layout.wholeRegion();

        util::fitAndCenterInside(m_text1, util::scaleRectInPlaceCopy(wholeRect, 0.25f));

        m_text2 = m_text1;
        m_text2.setString("Ninja Girl");
        util::setOriginToPosition(m_text2);

        m_text3 = m_text1;
        m_text3.setString("Halloween");
        util::setOriginToPosition(m_text3);

        m_text4 = m_text1;
        m_text4.setString("Nightmare");
        util::setOriginToPosition(m_text4);

        const float vertPad{ wholeRect.height * 0.005f };

        m_text1.setPosition(
            (wholeRect.width * 0.5f) - (m_text1.getGlobalBounds().width * 0.5f),
            (wholeRect.height * 0.25f));

        m_text2.setPosition(
            (wholeRect.width * 0.5f) - (m_text2.getGlobalBounds().width * 0.5f),
            (util::bottom(m_text1) + vertPad));

        m_text3.setPosition(
            (wholeRect.width * 0.5f) - (m_text3.getGlobalBounds().width * 0.5f),
            (util::bottom(m_text2) + vertPad));

        m_text4.setPosition(
            (wholeRect.width * 0.5f) - (m_text4.getGlobalBounds().width * 0.5f),
            (util::bottom(m_text3) + vertPad));
    }

    void TitleState::draw(
        const Context &, sf::RenderTarget & target, sf::RenderStates & states) const
    {
        target.draw(m_text1, states);
        target.draw(m_text2, states);
        target.draw(m_text3, states);
        target.draw(m_text4, states);
    }

    //

    PauseState::PauseState(const Context & context)
        : TimedMessageState(context, State::Pause, State::Play, "PAUSE", -1.0f)
    {}

    void PauseState::onEnter(Context & context)
    {
        context.audio.play("pause");

        // update pause screen texture
        context.pause_screen.update(context.window);
    }

    void PauseState::onExit(Context & context) { context.audio.play("pause"); }

    void PauseState::update(Context & context, const float frameTimeSec)
    {
        m_elapsedTimeSec += frameTimeSec;

        if (m_hasMouseClickedOrKeyPressed)
        {
            changeToNextState(context);
        }
    }

    void PauseState::draw(
        const Context & context, sf::RenderTarget & target, sf::RenderStates & states) const
    {
        StateBase::draw(context, target, states);
        context.pause_screen.draw(target);
    }

    //

    GameOverState::GameOverState(const Context & context)
        : TimedMessageState(context, State::GameOver, State::Credits, "Game Over\n\n", 4.5f)
    {}

    void GameOverState::onEnter(Context & context) { context.audio.play("game-over"); }

    void GameOverState::onExit(Context & context) { context.audio.stopAll(); }

    //

    LevelCompleteState::LevelCompleteState(const Context & context)
        : TimedMessageState(context, State::Level, State::Play, "Level Complete!\n\n", 3.0f)
    {}

    void LevelCompleteState::onEnter(Context & context) { context.audio.play("level-complete"); }

    void LevelCompleteState::onExit(Context & context)
    {
        ++context.level_number;
        context.ghosts.clear();
        context.slimes.clear();
        context.missiles.clear();
        context.coins.clear();
    }

    bool LevelCompleteState::handleEvent(Context &, const sf::Event &)
    {
        // always returning false prevents the player from quiting the state early
        return false;
    }

} // namespace halloween
