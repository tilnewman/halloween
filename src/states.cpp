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
    StateBase::StateBase(const State state, const State nextState, const float minDurationSec)
        : m_state(state)
        , m_nextState(nextState)
        , m_elapsedTimeSec(0.0f)
        , m_minDurationSec(minDurationSec) // any negative means this value is ignored
        , m_text(util::SfmlDefaults::instance().font())
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
        return ((event.is<sf::Event::KeyReleased>()) ||
                (event.is<sf::Event::MouseMoved>()) ||
                (event.is<sf::Event::MouseEntered>()) ||
                (event.is<sf::Event::MouseLeft>()) ||
                (event.is<sf::Event::MouseButtonReleased>()) ||
                (event.is<sf::Event::FocusGained>()) ||
                (event.is<sf::Event::FocusLost>()) ||
                (event.is<sf::Event::TouchBegan>()) ||
                (event.is<sf::Event::TouchEnded>()) ||
                (event.is<sf::Event::TouchMoved>()));
        // clang-format on
    }

    bool StateBase::handleQuitEvents(Context & context, const sf::Event & event)
    {
        if (event.is<sf::Event::Closed>())
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
        if (const auto * const keyPtr = event.getIf<sf::Event::KeyPressed>())
        {
            if (keyPtr->scancode == sf::Keyboard::Scancode::Q)
            {
                if (state() == State::Play)
                {
                    context.state.setChangePending(State::Lose);
                }
                else
                {
                    context.state.setChangePending(State::Quit);
                }

                return true;
            }

            if (keyPtr->scancode == sf::Keyboard::Scancode::Escape)
            {
                context.state.setChangePending(State::Quit);
                return true;
            }
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

        if (event.is<sf::Event::KeyPressed>() || event.is<sf::Event::MouseButtonPressed>())
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
        , m_text1(util::SfmlDefaults::instance().font())
        , m_text2(util::SfmlDefaults::instance().font())
        , m_text3(util::SfmlDefaults::instance().font())
        , m_text4(util::SfmlDefaults::instance().font())
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

        const float vertPad{ wholeRect.size.y * 0.015f };

        m_text1.setPosition({ (wholeRect.size.x * 0.5f) - (m_text1.getGlobalBounds().size.x * 0.5f),
                              (wholeRect.size.y * 0.25f) });

        m_text2.setPosition({ (wholeRect.size.x * 0.5f) - (m_text2.getGlobalBounds().size.x * 0.5f),
                              (util::bottom(m_text1) + vertPad) });

        m_text3.setPosition({ (wholeRect.size.x * 0.5f) - (m_text3.getGlobalBounds().size.x * 0.5f),
                              (util::bottom(m_text2) + vertPad) });

        m_text4.setPosition({ (wholeRect.size.x * 0.5f) - (m_text4.getGlobalBounds().size.x * 0.5f),
                              (util::bottom(m_text3) + vertPad) });
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

    LoseState::LoseState(const Context & context)
        : TimedMessageState(context, State::Lose, State::Credits, "You Lose\n", 4.5f)
        , m_scoreText(util::SfmlDefaults::instance().font())
    {}

    void LoseState::onEnter(Context & context)
    {
        context.audio.play("game-over");

        m_scoreText = m_text;
        m_scoreText.scale({ 0.35f, 0.35f });

        std::string str("Score: ");
        str += std::to_string(context.info_region.score());
        m_scoreText.setString(str);
        m_scoreText.setFillColor(sf::Color(127, 127, 127));

        util::setOriginToPosition(m_scoreText);

        m_scoreText.setPosition(
            { ((context.layout.wholeSize().x * 0.5f) -
               (m_scoreText.getGlobalBounds().size.x * 0.5f)),
              util::bottom(m_text) - (m_text.getGlobalBounds().size.y * 0.4f) });
    }

    void
        LoseState::draw(const Context &, sf::RenderTarget & target, sf::RenderStates & states) const
    {
        target.draw(m_text, states);
        target.draw(m_scoreText, states);
    }

    void LoseState::onExit(Context & context) { context.audio.stopAll(); }

    //

    WinState::WinState(const Context & context)
        : TimedMessageState(context, State::Win, State::Credits, "You Win\n", 4.5f)
        , m_scoreText(util::SfmlDefaults::instance().font())
    {}

    void WinState::onEnter(Context & context)
    {
        context.audio.play("winner");

        m_scoreText = m_text;
        m_scoreText.scale({ 0.35f, 0.35f });

        std::string str("Score: ");
        str += std::to_string(context.info_region.score());
        m_scoreText.setString(str);
        m_scoreText.setFillColor(sf::Color(127, 127, 127));

        util::setOriginToPosition(m_scoreText);

        m_scoreText.setPosition(
            { ((context.layout.wholeSize().x * 0.5f) -
               (m_scoreText.getGlobalBounds().size.x * 0.5f)),
              util::bottom(m_text) - (m_text.getGlobalBounds().size.y * 0.4f) });
    }

    void WinState::draw(const Context &, sf::RenderTarget & target, sf::RenderStates & states) const
    {
        target.draw(m_text, states);
        target.draw(m_scoreText, states);
    }

    void WinState::onExit(Context & context) { context.audio.stopAll(); }

} // namespace halloween
