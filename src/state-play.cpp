// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// state-play.cpp
//
#include "state-play.hpp"

#include "avatar.hpp"
#include "bat.hpp"
#include "coin.hpp"
#include "context.hpp"
#include "info-region.hpp"
#include "level-stats.hpp"
#include "level.hpp"
#include "missile.hpp"
#include "music-player.hpp"
#include "owl-calls.hpp"
#include "pause-screen.hpp"
#include "resources.hpp"
#include "screen-regions.hpp"
#include "sfml-util.hpp"
#include "slime.hpp"
#include "sound-player.hpp"
#include "spiked-ball.hpp"
#include "state-machine.hpp"

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Window.hpp>

#include <iostream>

namespace halloween
{

    PlayState::PlayState()
        : StateBase{ State::Play, State::Play }
    {}

    void PlayState::onEnter(const Context & t_context)
    {
        StateBase::onEnter(t_context);

        if (m_willLoadNewLevel)
        {
            m_willLoadNewLevel = false;
            t_context.level.numberIncrement();
            if (t_context.level.load(t_context))
            {
                t_context.stats = LevelStats();
                t_context.stats.coin_total = t_context.coins.count();
                t_context.stats.enemy_total = (t_context.slimes.count() + t_context.bats.count());
            }
            else
            {
                // if we fail to load it is because there are no more levels to play
                t_context.state.setChangePending(State::Win);
                return;
            }
        }

        t_context.owl_calls.start(t_context);
        t_context.music.start("crickets.ogg");
    }

    void PlayState::onExit(const Context & t_context)
    {
        t_context.audio.stopAll();
        t_context.owl_calls.stop();
        t_context.music.stop("crickets.ogg");
    }

    void PlayState::update(const Context & t_context, const float t_frameTimeSec)
    {
        StateBase::update(t_context, t_frameTimeSec);

        t_context.info_region.update(t_context, t_frameTimeSec);
        t_context.missiles.update(t_context, t_frameTimeSec);
        t_context.owl_calls.update(t_context, t_frameTimeSec);
        t_context.managers.updateAll(t_context, t_frameTimeSec);
        t_context.avatar.update(t_context, t_frameTimeSec);
    }

    bool PlayState::handleEvent(const Context & t_context, const sf::Event & t_event)
    {
        if (StateBase::handleEvent(t_context, t_event))
        {
            return true;
        }

        if (const auto * keyPtr = t_event.getIf<sf::Event::KeyPressed>())
        {
            if (keyPtr->scancode == sf::Keyboard::Scancode::Space)
            {
                t_context.state.setChangePending(State::Pause);
            }
            else if (keyPtr->scancode == sf::Keyboard::Scancode::S)
            {
                sf::Texture texture;
                if (texture.resize(t_context.window.getSize()))
                {
                    texture.update(t_context.window);
                    sf::Image image{ texture.copyToImage() };
                    if (!image.saveToFile("screenshot.png"))
                    {
                        std::cout << "Failed to save screenshot.png\n";
                    }
                }
            }
        }

        return t_context.state.isChangePending();
    }

    void PlayState::draw(
        const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates & t_states) const
    {
        t_target.draw(t_context.media.backgroundSprite(), t_states);

        t_context.managers.drawAllBeforeMap(t_context, t_target, t_states);

        for (const TileLayer & layer : t_context.level.tileLayers())
        {
            if (layer.visibleVerts.empty())
            {
                continue;
            }

            t_states.texture = &t_context.media.tileTexture(layer.image).texture;

            t_target.draw(
                &layer.visibleVerts[0],
                layer.visibleVerts.size(),
                sf::PrimitiveType::Triangles,
                t_states);
            
            t_states.texture = nullptr;
        }

        t_context.managers.drawAllAfterMap(t_context, t_target, t_states);
        t_context.missiles.draw(t_target, t_states);
        t_context.info_region.draw(t_target, t_states);
        t_context.avatar.draw(t_target, t_states);
    }

} // namespace halloween
