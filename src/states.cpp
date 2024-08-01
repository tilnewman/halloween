// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// states.cpp
//
#include "states.hpp"

#include "avatar.hpp"
#include "check-macros.hpp"
#include "coin.hpp"
#include "context.hpp"
#include "floating-spirit.hpp"
#include "ghost.hpp"
#include "info-region.hpp"
#include "level.hpp"
#include "missile.hpp"
#include "pause-screen.hpp"
#include "random.hpp"
#include "resources.hpp"
#include "screen-regions.hpp"
#include "sfml-util.hpp"
#include "slime.hpp"
#include "sound-player.hpp"

#include <sstream>

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

    PlayState::PlayState(const Context & context)
        : StateBase(context, State::Play, State::Play)
    {}

    void PlayState::onEnter(Context & context)
    {
        if (context.level.number != context.level_number)
        {
            if (!context.level.load(context))
            {
                context.state.setChangePending(State::GameOver);
            }

            context.slimes.spawnAll(context);
        }
    }

    void PlayState::update(Context & context, const float frameTimeSec)
    {
        if (context.level_number != context.level.number)
        {
            return;
        }

        StateBase::update(context, frameTimeSec);
        context.spirit.update(context, frameTimeSec);
        context.avatar.update(context, frameTimeSec);
        context.missiles.update(context, frameTimeSec);
        context.coins.update(context, frameTimeSec);
        context.ghosts.update(context, frameTimeSec);
        context.slimes.update(context, frameTimeSec);
    }

    bool PlayState::handleEvent(Context & context, const sf::Event & event)
    {
        if (StateBase::handleEvent(context, event))
        {
            return true;
        }

        if (event.type == sf::Event::KeyPressed)
        {
            if (event.key.code == sf::Keyboard::Space)
            {
                context.state.setChangePending(State::Pause);
            }
            else if (event.key.code == sf::Keyboard::F1)
            {
                sf::Texture texture;
                if (texture.create(context.window.getSize().x, context.window.getSize().y))
                {
                    texture.update(context.window);
                    sf::Image image{ texture.copyToImage() };
                    image.saveToFile("screenshot.png");
                }
            }
        }

        return context.state.isChangePending();
    }

    void PlayState::draw(
        const Context & context, sf::RenderTarget & target, sf::RenderStates & states) const
    {
        if (context.level_number != context.level.number)
        {
            return;
        }

        target.draw(context.media.bg_sprite, states);
        context.spirit.draw(target, states);

        for (const TileLayer & layer : context.level.tiles.layers)
        {
            states.texture = &context.media.tileTexture(layer.image).texture;
            target.draw(&layer.visibleVerts[0], layer.visibleVerts.size(), sf::Quads, states);
            states.texture = nullptr;
        }

        context.ghosts.draw(target, states);
        context.missiles.draw(target, states);
        context.coins.draw(target, states);
        context.avatar.draw(target, states);
        context.slimes.draw(target, states);
        context.info_region.draw(target, states);
    }

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
              context,
              State::Title,
              State::Play,
              "Super Lucky\nNinja Girl\nHalloween\nNightmare!",
              (m_defaultMinDurationSec * 2.0f))
    {}

    //

    PauseState::PauseState(const Context & context)
        : TimedMessageState(context, State::Pause, State::Play, "PAUSE", -1.0f)
    {}

    void PauseState::onEnter(Context & context)
    {
        context.audio.stopAllLooped();
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
        : TimedMessageState(context, State::GameOver, State::Quit, "Game Over\n\n", 4.5f)
    {}

    void GameOverState::onEnter(Context & context) { context.audio.play("game-over"); }

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

    bool LevelCompleteState::handleEvent(Context &, const sf::Event &) { return false; }

    //

    StateMachine::StateMachine()
        : m_stateUPtr()
        , m_changePendingOpt(State::Start)
    {
        reset();
    }

    void StateMachine::reset()
    {
        m_stateUPtr = std::make_unique<StartState>();
        m_changePendingOpt = m_stateUPtr->state();
    }

    void StateMachine::setChangePending(const State state) { m_changePendingOpt = state; }

    void StateMachine::changeIfPending(Context & context)
    {
        if (!m_changePendingOpt)
        {
            return;
        }

        m_stateUPtr->onExit(context);

        m_stateUPtr = makeState(context, m_changePendingOpt.value());
        m_changePendingOpt = std::nullopt;

        m_stateUPtr->onEnter(context);
    }

    IStateUPtr_t StateMachine::makeState(Context & context, const State state)
    {
        // clang-format off
        switch (state)
        {
            case State::Start:    { return std::make_unique<StartState>();                }
            case State::Title:    { return std::make_unique<TitleState>(context);         }
            case State::Play:     { return std::make_unique<PlayState>(context);          }
            case State::Pause:    { return std::make_unique<PauseState>(context);         }
            case State::Level:    { return std::make_unique<LevelCompleteState>(context); }
            case State::GameOver: { return std::make_unique<GameOverState>(context);      }
            case State::Quit:     { return std::make_unique<QuitState>();                 }
            default:
            {
                M_LOG("Asked to make a state that is not implemented.  Til, you forgot"\
                      "to update the makeState() function after adding a new state again...");

                assert(false);
                return std::make_unique<QuitState>();            
            } 
        };
        // clang-format on
    }

} // namespace halloween
