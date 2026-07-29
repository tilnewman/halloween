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
#include "sfml-defaults.hpp"
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
    StateBase::StateBase(const State t_state, const State t_nextState, const float t_minDurationSec)
        : m_state{ t_state }
        , m_nextState{ t_nextState }
        , m_elapsedTimeSec{ 0.0f }
        , m_minDurationSec{ t_minDurationSec } // any negative means this value is ignored
        , m_text{ util::SfmlDefaults::instance().font() }
    {}

    StateBase::StateBase(
        const Context & t_context,
        const State t_state,
        const State t_nextState,
        const std::string & t_message,
        const float t_minDurationSec)
        : StateBase{ t_state, t_nextState, t_minDurationSec }
    {
        setupText(t_context, t_message);
    }

    void StateBase::setupText(const Context & t_context, const std::string & t_message)
    {
        m_text.setString(t_message);
        m_text.setCharacterSize(99);
        m_text.setFont(t_context.media.font);
        m_text.setFillColor(m_textColorDefault);

        util::fitAndCenterInside(
            m_text, util::scaleRectInPlaceCopy(t_context.layout.wholeRegion(), 0.25f));
    }

    void StateBase::update(Context &, const float t_frameTimeSec)
    {
        m_elapsedTimeSec += t_frameTimeSec;
    }

    bool StateBase::changeToNextState(const Context & t_context)
    {
        if (state() == m_nextState)
        {
            return false;
        }

        t_context.state.setChangePending(nextState());
        return true;
    }

    bool StateBase::willIgnoreEvent(const Context & t_context, const sf::Event & t_event) const
    {
        // all events should be ignored after a state change is scheduled
        if (t_context.state.isChangePending())
        {
            return true;
        }

        // clang-format off
        return ((t_event.is<sf::Event::KeyReleased>()) ||
                (t_event.is<sf::Event::MouseMoved>()) ||
                (t_event.is<sf::Event::MouseEntered>()) ||
                (t_event.is<sf::Event::MouseLeft>()) ||
                (t_event.is<sf::Event::MouseButtonReleased>()) ||
                (t_event.is<sf::Event::FocusGained>()) ||
                (t_event.is<sf::Event::FocusLost>()) ||
                (t_event.is<sf::Event::TouchBegan>()) ||
                (t_event.is<sf::Event::TouchEnded>()) ||
                (t_event.is<sf::Event::TouchMoved>()));
        // clang-format on
    }

    bool StateBase::handleQuitEvents(Context & t_context, const sf::Event & t_event)
    {
        if (t_event.is<sf::Event::Closed>())
        {
            t_context.state.setChangePending(State::Quit);
            return true;
        }

        if (state() == State::Pause)
        {
            t_context.state.setChangePending(State::Play);
            return true;
        }

        // all that remain are keystrokes
        if (const auto * const keyPtr = t_event.getIf<sf::Event::KeyPressed>())
        {
            if (keyPtr->scancode == sf::Keyboard::Scancode::Q)
            {
                if (state() == State::Play)
                {
                    t_context.state.setChangePending(State::Lose);
                }
                else
                {
                    t_context.state.setChangePending(State::Quit);
                }

                return true;
            }

            if (keyPtr->scancode == sf::Keyboard::Scancode::Escape)
            {
                t_context.state.setChangePending(State::Quit);
                return true;
            }
        }

        return false;
    }

    bool StateBase::handleEvent(Context & t_context, const sf::Event & t_event)
    {
        if (willIgnoreEvent(t_context, t_event))
        {
            return true;
        }

        if (handleQuitEvents(t_context, t_event))
        {
            return true;
        }

        return t_context.state.isChangePending();
    }

    void StateBase::draw(
        const Context &, sf::RenderTarget & t_target, sf::RenderStates & t_states) const
    {
        t_target.draw(m_text, t_states);
    }

    //

    void StartState::onEnter(Context & t_context) { changeToNextState(t_context); }

    void StartState::onExit(Context & t_context) { t_context.audio.play("respawn"); }

    //

    TimedMessageState::TimedMessageState(
        const Context & t_context,
        const State t_state,
        const State t_nextState,
        const std::string & t_message,
        const float t_minDurationSec)
        : StateBase{ t_context, t_state, t_nextState, t_message, t_minDurationSec }
    {
        const sf::FloatRect textBounds{ util::scaleRectInPlaceCopy(
            t_context.layout.mapRegion(), 0.9f) };

        util::centerInside(m_text, textBounds);
    }

    bool TimedMessageState::handleEvent(Context & t_context, const sf::Event & t_event)
    {
        if (StateBase::handleEvent(t_context, t_event))
        {
            return true;
        }

        if (t_event.is<sf::Event::KeyPressed>() || t_event.is<sf::Event::MouseButtonPressed>())
        {
            m_hasMouseClickedOrKeyPressed = true;
        }

        if (!hasMinTimeElapsed())
        {
            return false;
        }

        if (m_hasMouseClickedOrKeyPressed)
        {
            changeToNextState(t_context);
            return true;
        }

        return false;
    }

    void TimedMessageState::update(Context & t_context, const float t_frameTimeSec)
    {
        StateBase::update(t_context, t_frameTimeSec);

        if (hasMinTimeElapsed() || m_hasMouseClickedOrKeyPressed)
        {
            changeToNextState(t_context);
        }
    }

    //

    TitleState::TitleState(const Context & t_context)
        : TimedMessageState{
            t_context, State::Title, State::Play, "", (m_defaultMinDurationSec * 2.0f)
        }
        , m_text1(util::SfmlDefaults::instance().font())
        , m_text2(util::SfmlDefaults::instance().font())
        , m_text3(util::SfmlDefaults::instance().font())
        , m_text4(util::SfmlDefaults::instance().font())
    {}

    void TitleState::onEnter(Context & t_context)
    {
        m_text1.setFont(t_context.media.font);
        m_text1.setFillColor(sf::Color::White);
        m_text1.setCharacterSize(99);
        m_text1.setString("Super Lucky");

        const sf::FloatRect wholeRect{ t_context.layout.wholeRegion() };

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

        const float vertPad{ wholeRect.size.y * 0.015f };

        m_text1.setPosition(
            { (wholeRect.size.x * 0.5f) - (m_text1.getGlobalBounds().size.x * 0.5f),
              (wholeRect.size.y * 0.25f) });

        m_text2.setPosition(
            { (wholeRect.size.x * 0.5f) - (m_text2.getGlobalBounds().size.x * 0.5f),
              (util::bottom(m_text1) + vertPad) });

        m_text3.setPosition(
            { (wholeRect.size.x * 0.5f) - (m_text3.getGlobalBounds().size.x * 0.5f),
              (util::bottom(m_text2) + vertPad) });

        m_text4.setPosition(
            { (wholeRect.size.x * 0.5f) - (m_text4.getGlobalBounds().size.x * 0.5f),
              (util::bottom(m_text3) + vertPad) });
    }

    void TitleState::draw(
        const Context &, sf::RenderTarget & t_target, sf::RenderStates & t_states) const
    {
        t_target.draw(m_text1, t_states);
        t_target.draw(m_text2, t_states);
        t_target.draw(m_text3, t_states);
        t_target.draw(m_text4, t_states);
    }

    //

    PauseState::PauseState(const Context & t_context)
        : TimedMessageState{ t_context, State::Pause, State::Play, "PAUSE", -1.0f }
        , screen{}
    {}

    void PauseState::onEnter(Context & t_context)
    {
        t_context.audio.play("pause");
        screen.setup(t_context);
    }

    void PauseState::onExit(Context & t_context) { t_context.audio.play("pause"); }

    void PauseState::update(Context & t_context, const float t_frameTimeSec)
    {
        m_elapsedTimeSec += t_frameTimeSec;

        if (m_hasMouseClickedOrKeyPressed)
        {
            changeToNextState(t_context);
        }
    }

    void PauseState::draw(
        const Context & t_context, sf::RenderTarget & target, sf::RenderStates & states) const
    {
        StateBase::draw(t_context, target, states);
        screen.draw(target, states);
    }

    //

    LoseState::LoseState(const Context & t_context)
        : TimedMessageState{ t_context, State::Lose, State::Credits, "You Lose\n", 4.5f }
        , m_scoreText{ util::SfmlDefaults::instance().font() }
    {}

    void LoseState::onEnter(Context & t_context)
    {
        t_context.audio.play("game-over");

        m_scoreText = m_text;
        m_scoreText.scale({ 0.35f, 0.35f });

        std::string str{ "Score: " };
        str += std::to_string(t_context.info_region.score());
        m_scoreText.setString(str);
        m_scoreText.setFillColor(sf::Color(127, 127, 127));

        util::setOriginToPosition(m_scoreText);

        m_scoreText.setPosition(
            { ((t_context.layout.wholeSize().x * 0.5f) -
               (m_scoreText.getGlobalBounds().size.x * 0.5f)),
              util::bottom(m_text) - (m_text.getGlobalBounds().size.y * 0.4f) });
    }

    void LoseState::draw(
        const Context &, sf::RenderTarget & t_target, sf::RenderStates & t_states) const
    {
        t_target.draw(m_text, t_states);
        t_target.draw(m_scoreText, t_states);
    }

    void LoseState::onExit(Context & t_context) { t_context.audio.stopAll(); }

    //

    WinState::WinState(const Context & t_context)
        : TimedMessageState{ t_context, State::Win, State::Credits, "You Win\n", 4.5f }
        , m_scoreText{ util::SfmlDefaults::instance().font() }
    {}

    void WinState::onEnter(Context & t_context)
    {
        t_context.audio.play("winner");

        m_scoreText = m_text;
        m_scoreText.scale({ 0.35f, 0.35f });

        std::string str{ "Score: " };
        str += std::to_string(t_context.info_region.score());
        m_scoreText.setString(str);
        m_scoreText.setFillColor(sf::Color(127, 127, 127));

        util::setOriginToPosition(m_scoreText);

        m_scoreText.setPosition(
            { ((t_context.layout.wholeSize().x * 0.5f) -
               (m_scoreText.getGlobalBounds().size.x * 0.5f)),
              util::bottom(m_text) - (m_text.getGlobalBounds().size.y * 0.4f) });
    }

    void WinState::draw(
        const Context &, sf::RenderTarget & t_target, sf::RenderStates & t_states) const
    {
        t_target.draw(m_text, t_states);
        t_target.draw(m_scoreText, t_states);
    }

    void WinState::onExit(Context & t_context) { t_context.audio.stopAll(); }

} // namespace halloween
