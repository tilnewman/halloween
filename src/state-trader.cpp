//
// state-trader.cpp
//
#include "state-trader.hpp"

#include "check-macros.hpp"
#include "context.hpp"
#include "info-region.hpp"
#include "level-stats.hpp"
#include "settings.hpp"
#include "sfml-defaults.hpp"
#include "sfml-util.hpp"
#include "sound-player.hpp"
#include "state-machine.hpp"
#include "texture-loader.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

#include <string>

namespace halloween
{

    TraderState::TraderState()
        : TimedMessageState{ State::Trader, State::Play, "", -1.0f }
        , m_backgroundTexture{}
        , m_backgroundSprite{ m_backgroundTexture }
        , m_diaglogTexture{}
        , m_dialogSprite{ m_diaglogTexture }
        , m_elapsedSec{ 0.0f }
        , m_dialogRect{}
        , m_dialogTextDetails{}
        , m_dialogTextPack{}
        , m_dartsToGive{ 0 }
        , m_coinsToTake{ 0 }
        , m_offerMessage{}
        , m_traderDialogPos{}
        , m_playerDialogPos{}
        , m_phase{ TraderPhase::InitailDelay }
    {}

    void TraderState::onEnter(const Context & t_context)
    {
        //
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

        //
        util::TextureLoader::load(
            m_diaglogTexture, (t_context.settings.media_path / "image" / "dialog.png"), true);

        m_dialogSprite.setTexture(m_diaglogTexture, true);
        m_dialogSprite.scale({ 0.4f, 0.4f });

        m_dialogSprite.setPosition(
            { (util::center(m_traderRect).x - util::center(m_dialogSprite.getGlobalBounds()).x),
              (m_traderRect.position.y - m_dialogSprite.getGlobalBounds().size.y) });

        m_dialogSprite.move({ (m_traderRect.size.x * 0.75f), 0.0f });
        m_traderDialogPos = m_dialogSprite.getPosition();

        m_dialogSprite.move({ -(m_traderRect.size.x * 2.0f), 0.0f });
        m_playerDialogPos = m_dialogSprite.getPosition();

        m_dialogSprite.setPosition(m_traderDialogPos);

        //
        m_dartsToGive = (t_context.info_region.coins() / t_context.settings.dart_coin_cost);
        m_coinsToTake = (m_dartsToGive * t_context.settings.dart_coin_cost);

        if (m_dartsToGive > 0)
        {
            m_offerMessage = "Wanna buy ";
            m_offerMessage += std::to_string(m_dartsToGive);
            m_offerMessage += " darts for ";
            m_offerMessage += std::to_string(m_coinsToTake);
            m_offerMessage += " coins?";
        }
        else
        {
            m_offerMessage = " If you collect more coins I can sell you darts.";
        }

        //
        updateDialogRect();
        m_dialogTextDetails = TextDetails(Font::General, 30u, sf::Color::Black);
        m_dialogTextPack = TextLayout::typeset(t_context, "", m_dialogRect, m_dialogTextDetails);
    }

    void TraderState::updateDialogRect()
    {
        m_dialogRect =
            util::scaleRectInPlaceCopy(m_dialogSprite.getGlobalBounds(), { 0.9f, 0.65f });

        m_dialogRect.position.y -= (m_dialogRect.size.y * 0.1f);
    }

    void TraderState::onExit(const Context &) {}

    void TraderState::update(const Context & t_context, const float m_frameTimeSec)
    {
        m_elapsedSec += m_frameTimeSec;

        if ((TraderPhase::InitailDelay == m_phase) and (m_elapsedSec > 1.0f))
        {
            m_elapsedSec = 0.0f;
            m_phase = TraderPhase::TraderHey;

            m_dialogTextPack =
                TextLayout::typeset(t_context, "Hey", m_dialogRect, m_dialogTextDetails);
        }
        else if ((TraderPhase::TraderHey == m_phase) and (m_elapsedSec > 0.75f))
        {
            m_elapsedSec = 0.0f;
            m_phase = TraderPhase::TraderHeyDelay;
        }
        else if ((TraderPhase::TraderHeyDelay == m_phase) and (m_elapsedSec > 0.75f))
        {
            m_elapsedSec = 0.0f;
            m_phase = TraderPhase::PlayerHey;
            m_dialogSprite.setPosition(m_playerDialogPos);
            updateDialogRect();

            m_dialogTextPack =
                TextLayout::typeset(t_context, "Hey", m_dialogRect, m_dialogTextDetails);
        }
        else if ((TraderPhase::PlayerHey == m_phase) and (m_elapsedSec > 0.75f))
        {
            m_elapsedSec = 0.0f;
            m_phase = TraderPhase::PlayerHeyDelay;
        }
        else if ((TraderPhase::PlayerHeyDelay == m_phase) and (m_elapsedSec > 0.75f))
        {
            m_elapsedSec = 0.0f;
            m_phase = TraderPhase::TraderOffer;
            m_dialogSprite.setPosition(m_traderDialogPos);
            updateDialogRect();

            m_dialogTextPack =
                TextLayout::typeset(t_context, m_offerMessage, m_dialogRect, m_dialogTextDetails);
        }
        else if ((TraderPhase::TraderHint == m_phase) and (m_elapsedSec > 2.0f))
        {
            m_elapsedSec = 0.0f;
            m_phase = TraderPhase::TraderOffer;

            m_dialogTextPack =
                TextLayout::typeset(t_context, m_offerMessage, m_dialogRect, m_dialogTextDetails);
        }
    }

    bool TraderState::handleEvent(const Context & t_context, const sf::Event & t_event)
    {
        if (const auto * const keyPtr = t_event.getIf<sf::Event::KeyPressed>())
        {
            if ((TraderPhase::TraderOffer == m_phase) and (m_dartsToGive > 0))
            {
                if (keyPtr->scancode == sf::Keyboard::Scancode::Y)
                {
                    t_context.info_region.dartsAdjust(m_dartsToGive);
                    t_context.info_region.coinsAdjust(-m_coinsToTake);
                    t_context.state.setChangePending(State::Play);
                }
                else if (keyPtr->scancode == sf::Keyboard::Scancode::N)
                {
                    t_context.state.setChangePending(State::Play);
                }
                else if (
                    (keyPtr->scancode != sf::Keyboard::Scancode::Y) and
                    (keyPtr->scancode != sf::Keyboard::Scancode::N))
                {
                    m_elapsedSec = 0.0f;
                    m_phase = TraderPhase::TraderHint;

                    m_dialogTextPack = TextLayout::typeset(
                        t_context, "Press Y or N to answer.", m_dialogRect, m_dialogTextDetails);
                }
            }
            else if (
                (TraderPhase::TraderOffer == m_phase) and (0 == m_dartsToGive) and
                (keyPtr->scancode == sf::Keyboard::Scancode::N))
            {
                t_context.state.setChangePending(State::Play);
            }
        }

        return false;
    }

    void TraderState::draw(
        const Context &, sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        t_target.draw(m_backgroundSprite, t_states);

        if ((TraderPhase::TraderHey == m_phase) or (TraderPhase::PlayerHey == m_phase) or
            (TraderPhase::TraderOffer == m_phase) or (TraderPhase::TraderHint == m_phase))
        {
            t_target.draw(m_dialogSprite, t_states);

            for (const sf::Text & text : m_dialogTextPack.texts)
            {
                t_target.draw(text, t_states);
            }
        }
    }

} // namespace halloween
