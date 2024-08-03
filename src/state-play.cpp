// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// state-play.cpp
//
#include "state-play.hpp"

#include "avatar.hpp"
#include "coin.hpp"
#include "context.hpp"
#include "dart.hpp"
#include "floating-spirit.hpp"
#include "ghost.hpp"
#include "info-region.hpp"
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
#include "state-machine.hpp"

#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Window.hpp>

namespace halloween
{

    PlayState::PlayState(const Context & context)
        : StateBase(context, State::Play, State::Play)
    {}

    void PlayState::onEnter(Context & context)
    {
        if (context.level.number != context.level_number)
        {
            context.darts.clear();
            context.ghosts.clear();
            context.ghosts.clearSpawnPoints();
            context.slimes.clear();
            context.slimes.clearRects();

            if (context.level.load(context))
            {
                context.slimes.spawnAll(context);
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
        context.spirit.update(context, frameTimeSec);
        context.avatar.update(context, frameTimeSec);
        context.missiles.update(context, frameTimeSec);
        context.coins.update(frameTimeSec);
        context.darts.update(frameTimeSec);
        context.ghosts.update(context, frameTimeSec);
        context.slimes.update(frameTimeSec);
        context.owl_calls.update(context, frameTimeSec);
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
            else if (event.key.code == sf::Keyboard::S)
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
        context.spirit.draw(context, target, states);

        for (const TileLayer & layer : context.level.tiles.layers)
        {
            states.texture = &context.media.tileTexture(layer.image).texture;
            target.draw(&layer.visibleVerts[0], layer.visibleVerts.size(), sf::Quads, states);
            states.texture = nullptr;
        }

        context.ghosts.draw(target, states);
        context.missiles.draw(target, states);
        context.coins.draw(target, states);
        context.darts.draw(target, states);
        context.avatar.draw(target, states);
        context.slimes.draw(target, states);
        context.info_region.draw(target, states);
    }

} // namespace halloween
