// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// states.cpp
//
#include "states.hpp"

#include "coin.hpp"
#include "context.hpp"
#include "filesystem-util.hpp"
#include "fonts.hpp"
#include "ghost.hpp"
#include "info-region.hpp"
#include "missile.hpp"
#include "pause-screen.hpp"
#include "screen-regions.hpp"
#include "settings.hpp"
#include "sfml-defaults.hpp"
#include "sfml-util.hpp"
#include "slime.hpp"
#include "sound-player.hpp"
#include "state-machine.hpp"
#include "texture-loader.hpp"

#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Window.hpp>

#include <filesystem>

namespace halloween
{
    StateBase::StateBase(
        const State t_state,
        const State t_nextState,
        const std::string & t_message,
        const float t_minDurationSec)
        : m_state{ t_state }
        , m_nextState{ t_nextState }
        , m_elapsedTimeSec{ 0.0f }
        , m_minDurationSec{ t_minDurationSec } // any negative means this value is ignored
        , m_message{ t_message }
        , m_text{ util::SfmlDefaults::instance().font() }
    {}

    void StateBase::onEnter(const Context & t_context) { setupText(t_context, m_message); }

    void StateBase::setupText(const Context & t_context, const std::string & t_message)
    {
        m_text = t_context.fonts.makeText(99, t_message, m_textColorDefault);
    }

    void StateBase::update(const Context &, const float t_frameTimeSec)
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

    bool StateBase::handleQuitEvents(const Context & t_context, const sf::Event & t_event)
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

    bool StateBase::handleEvent(const Context & t_context, const sf::Event & t_event)
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
        const Context &, sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        t_target.draw(m_text, t_states);
    }

    //

    void StartState::onEnter(const Context & t_context) { changeToNextState(t_context); }

    void StartState::onExit(const Context & t_context) { t_context.audio.play("respawn"); }

    //

    TimedMessageState::TimedMessageState(
        const State t_state,
        const State t_nextState,
        const std::string & t_message,
        const float t_minDurationSec)
        : StateBase{ t_state, t_nextState, t_message, t_minDurationSec }
        , m_strawMatTexture{}
        , m_strawMatSprite{ m_strawMatTexture }
        , m_panelTexture{}
        , m_panelSprite{ m_panelTexture }
    {}

    void TimedMessageState::onEnter(const Context & t_context)
    {
        StateBase::onEnter(t_context);

        //
        const sf::FloatRect wholeRect{ t_context.layout.wholeRegion() };

        util::TextureLoader::load(
            m_strawMatTexture, (t_context.settings.media_path / "image" / "straw-mat.png"), true);

        m_strawMatTexture.setRepeated(true);

        m_strawMatSprite.setTexture(m_strawMatTexture, true);
        m_strawMatSprite.setTextureRect({ { 0, 0 }, sf::Vector2i{ wholeRect.size } });
        m_strawMatSprite.setColor(sf::Color(255, 255, 255, 127));

        //
        util::TextureLoader::load(
            m_panelTexture, (t_context.settings.media_path / "image" / "wood-panel.png"), true);

        m_panelTexture.setRepeated(true);

        m_panelSprite.setTexture(m_panelTexture, true);

        sf::IntRect panelRect;
        panelRect.position.x = 0;
        panelRect.position.y = 0;
        panelRect.size.x = static_cast<int>(wholeRect.size.x);
        panelRect.size.y = static_cast<int>(m_panelTexture.getSize().y);
        m_panelSprite.setTextureRect(panelRect);

        m_panelSprite.setPosition({ 0.0f, static_cast<float>(m_panelTexture.getSize().y * 2) });

        //
        m_text.setPosition(
            { (util::center(wholeRect).x - util::center(m_text.getGlobalBounds()).x),
              (util::center(m_panelSprite).y - util::center(m_text.getGlobalBounds()).y) });

        //
        m_panelContentRect.position.x = 0.0f;
        m_panelContentRect.position.y = (m_panelSprite.getPosition().y + 30.0f);
        m_panelContentRect.size.x = wholeRect.size.x;
        m_panelContentRect.size.y = 210.0f;
    }

    void TimedMessageState::draw(
        const Context &, sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        t_target.draw(m_strawMatSprite, t_states);
        t_target.draw(m_panelSprite, t_states);
        t_target.draw(m_text, t_states);
    }

    bool TimedMessageState::handleEvent(const Context & t_context, const sf::Event & t_event)
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

    void TimedMessageState::update(const Context & t_context, const float t_frameTimeSec)
    {
        StateBase::update(t_context, t_frameTimeSec);

        if (hasMinTimeElapsed() || m_hasMouseClickedOrKeyPressed)
        {
            changeToNextState(t_context);
        }
    }

    //

    TitleState::TitleState()
        : TimedMessageState{ State::Title,
                             State::Play,
                             "Super Lucky Ninja Girl Halloween Nightmare",
                             (m_defaultMinDurationSec * 3.0f) }
    {}

    void TitleState::onEnter(const Context & t_context)
    {
        TimedMessageState::onEnter(t_context);
    }

    void TitleState::draw(
        const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        TimedMessageState::draw(t_context, t_target, t_states);
    }

    //

    PauseState::PauseState()
        : TimedMessageState{ State::Pause, State::Play, "PAUSE", -1.0f }
        , m_screen{}
    {}

    void PauseState::onEnter(const Context & t_context)
    {
        TimedMessageState::onEnter(t_context);
        t_context.audio.play("pause");
        m_screen.setup(t_context);
    }

    void PauseState::onExit(const Context & t_context) { t_context.audio.play("pause"); }

    void PauseState::update(const Context & t_context, const float t_frameTimeSec)
    {
        m_elapsedTimeSec += t_frameTimeSec;

        if (m_hasMouseClickedOrKeyPressed)
        {
            changeToNextState(t_context);
        }
    }

    void PauseState::draw(
        const Context & t_context, sf::RenderTarget & target, sf::RenderStates states) const
    {
        StateBase::draw(t_context, target, states);
        m_screen.draw(target, states);
    }

    //

    LoseState::LoseState()
        : TimedMessageState{ State::Lose, State::Credits, "You Lose\n", 4.5f }
        , m_scoreText{ util::SfmlDefaults::instance().font() }
        , m_accentTexture1{}
        , m_accentTexture2{}
        , m_accentSprite1{ m_accentTexture1 }
        , m_accentSprite2{ m_accentTexture2 }
    {}

    void LoseState::onEnter(const Context & t_context)
    {
        TimedMessageState::onEnter(t_context);

        t_context.audio.play("game-over");

        const sf::FloatRect wholeRect{ t_context.layout.wholeRegion() };

        // score text
        m_scoreText = m_text;

        std::string str{ "Score: " };
        str += std::to_string(t_context.info_region.score());
        m_scoreText.setString(str);

        m_scoreText.scale({ 0.35f, 0.35f });
        util::setOriginToPosition(m_scoreText);
        m_scoreText.setFillColor(sf::Color(127, 127, 127));

        m_scoreText.setPosition(
            { ((wholeRect.size.x * 0.5f) - (m_scoreText.getGlobalBounds().size.x * 0.5f)),
              util::bottom(m_text) - (m_text.getGlobalBounds().size.y * 0.5f) });

        // accent image
        const auto accentImageDirPath{ t_context.settings.media_path / "image" / "accent" };
        const auto accentImagePaths{ util::findFilesInDirectory(accentImageDirPath, ".png") };

        util::TextureLoader::load(m_accentTexture1, t_context.random.from(accentImagePaths), true);
        util::TextureLoader::load(m_accentTexture2, t_context.random.from(accentImagePaths), true);

        m_accentSprite1.setTexture(m_accentTexture1, true);
        m_accentSprite1.setColor(sf::Color(255, 255, 255, 192));

        m_accentSprite2.setTexture(m_accentTexture2, true);
        m_accentSprite2.setColor(sf::Color(255, 255, 255, 192));

        const sf::FloatRect panelRect{ panelContentRect() };

        util::fitAndCenterInside(m_accentSprite1, panelRect);
        util::fitAndCenterInside(m_accentSprite2, panelRect);

        m_accentSprite1.setPosition({ 20.0f, m_accentSprite1.getPosition().y });

        m_accentSprite2.setPosition(
            { (wholeRect.size.x - (m_accentSprite2.getGlobalBounds().size.x + 20.0f)),
              m_accentSprite2.getPosition().y });
    }

    void LoseState::draw(
        const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        TimedMessageState::draw(t_context, t_target, t_states);
        t_target.draw(m_scoreText, t_states);
        t_target.draw(m_accentSprite1, t_states);
        t_target.draw(m_accentSprite2, t_states);
    }

    void LoseState::onExit(const Context & t_context) { t_context.audio.stopAll(); }

    //

    WinState::WinState()
        : TimedMessageState{ State::Win, State::Credits, "You Win\n", 4.5f }
        , m_scoreText{ util::SfmlDefaults::instance().font() }
    {}

    void WinState::onEnter(const Context & t_context)
    {
        TimedMessageState::onEnter(t_context);

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
        const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        TimedMessageState::draw(t_context, t_target, t_states);
        t_target.draw(m_text, t_states);
        t_target.draw(m_scoreText, t_states);
    }

    void WinState::onExit(const Context & t_context) { t_context.audio.stopAll(); }

} // namespace halloween
