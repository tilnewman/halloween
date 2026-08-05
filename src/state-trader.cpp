//
// state-trader.cpp
//
#include "state-trader.hpp"

#include "check-macros.hpp"
#include "context.hpp"
#include "fonts.hpp"
#include "settings.hpp"
#include "sfml-util.hpp"
#include "sound-player.hpp"
#include "state-machine.hpp"
#include "texture-loader.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

namespace halloween
{

    TraderState::TraderState()
        : TimedMessageState{ State::Trader, State::Play, "", -1.0f }
        , m_backgroundTexture{}
        , m_backgroundSprite{ m_backgroundTexture }
        , m_diaglogTexture{}
        , m_dialogSprite{ m_diaglogTexture }
        , m_elapsedSec{ 0.0f }
        , m_timeBeforeDialog{ 1.0f }
    {}

    void TraderState::onEnter(const Context & t_context)
    {
        util::TextureLoader::load(
            m_diaglogTexture, (t_context.settings.media_path / "image" / "dialog.png"), true);

        m_dialogSprite.setTexture(m_diaglogTexture, true);
        m_dialogSprite.scale({ 0.4f, 0.4f });

        m_dialogSprite.setPosition(
            { (util::center(m_traderRect).x - util::center(m_dialogSprite.getGlobalBounds()).x),
              (m_traderRect.position.y - m_dialogSprite.getGlobalBounds().size.y) });

        if (m_backgroundTexture.resize(t_context.window.getSize()))
        {
            m_backgroundTexture.update(t_context.window);
            m_backgroundSprite.setTexture(m_backgroundTexture, true);
            m_backgroundSprite.setColor(t_context.settings.trade_screen_bg_color);
        }
        else
        {
            M_LOG("Your video card sucks so the trade state won't look very good.");
        }
    }

    void TraderState::onExit(const Context &) {}

    void TraderState::update(const Context &, const float m_frameTimeSec)
    {
        m_elapsedSec += m_frameTimeSec;
    }

    bool TraderState::handleEvent(const Context & t_context, const sf::Event & t_event)
    {
        if (const auto * const keyPtr = t_event.getIf<sf::Event::KeyPressed>())
        {
            if (keyPtr->scancode == sf::Keyboard::Scancode::Escape)
            {
                t_context.state.setChangePending(State::Play);
                return true;
            }
        }

        return false;
    }

    void TraderState::draw(
        const Context &, sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        t_target.draw(m_backgroundSprite, t_states);

        if (m_elapsedSec > m_timeBeforeDialog)
        {
            t_target.draw(m_dialogSprite, t_states);
        }
    }

} // namespace halloween
