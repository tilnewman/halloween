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

    PlayState::PlayState(const Context & context)
        : StateBase(context, State::Play, State::Play)
    {}

    void PlayState::onEnter(Context & context)
    {
        if (context.level.number != context.level_number)
        {
            if (context.level.load(context))
            {
                context.stats = LevelStats();
                context.stats.coin_total = context.coins.count();
                context.stats.enemy_total = (context.slimes.count() + context.bats.count());
            }
            else
            {
                // if we fail to load it is because there are no more levels to play
                context.state.setChangePending(State::Win);
            }
        }

        context.owl_calls.start(context);
        context.music.start("crickets.ogg");
    }

    void PlayState::onExit(Context & context)
    {
        context.audio.stopAll();
        context.owl_calls.stop();
        context.music.stop("crickets.ogg");
    }

    void PlayState::update(Context & context, const float frameTimeSec)
    {
        if (context.level_number != context.level.number)
        {
            return;
        }

        StateBase::update(context, frameTimeSec);

        context.info_region.update(context, frameTimeSec);

        context.missiles.update(context, frameTimeSec);
        context.owl_calls.update(context, frameTimeSec);

        context.managers.updateAll(context, frameTimeSec);

        context.avatar.update(context, frameTimeSec);
    }

    bool PlayState::handleEvent(Context & context, const sf::Event & event)
    {
        if (StateBase::handleEvent(context, event))
        {
            return true;
        }

        if (const auto * keyPtr = event.getIf<sf::Event::KeyPressed>())
        {
            if (keyPtr->scancode == sf::Keyboard::Scancode::Space)
            {
                context.state.setChangePending(State::Pause);
            }
            else if (keyPtr->scancode == sf::Keyboard::Scancode::S)
            {
                sf::Texture texture;
                if (texture.resize(context.window.getSize()))
                {
                    texture.update(context.window);
                    sf::Image image{ texture.copyToImage() };
                    if (!image.saveToFile("screenshot.png"))
                    {
                        std::cout << "Failed to save screenshot.png\n";
                    }
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

        context.managers.drawAllBeforeMap(context, target, states);

        for (const TileLayer & layer : context.level.tiles.layers)
        {
            if (layer.visibleVerts.empty())
            {
                continue;
            }

            states.texture = &context.media.tileTexture(layer.image).texture;

            target.draw(
                &layer.visibleVerts[0],
                layer.visibleVerts.size(),
                sf::PrimitiveType::Triangles,
                states);
            
            states.texture = nullptr;
        }

        context.managers.drawAllAfterMap(context, target, states);

        context.missiles.draw(target, states);
        context.info_region.draw(target, states);
        context.avatar.draw(target, states);
    }

} // namespace halloween
