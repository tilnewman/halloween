// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// ghost.cpp
//
#include "ghost.hpp"

#include "context.hpp"
#include "info-region.hpp"
#include "level.hpp"
#include "screen-regions.hpp"
#include "settings.hpp"
#include "sfml-util.hpp"
#include "sound-player.hpp"

#include <algorithm>

#include <SFML/Graphics/RenderTarget.hpp>

namespace halloween
{
    Ghost::Ghost(
        const Context & context,
        const sf::Texture & texture,
        const float spd,
        const sf::Vector2f & position)
        : is_alive(true)
        , speed(spd)
        , sprite(texture)
    {
        sprite.setScale(context.settings.ghost_scale);
        util::setOriginToCenter(sprite);
        sprite.setPosition(position);
    }

    //

    Ghosts::Ghosts()
        : m_texture1()
        , m_texture2()
        , m_texture3()
        , m_ghosts()
    {
        // not likely to be more than a dozen on screen at once
        m_ghosts.reserve(100);
    }

    void Ghosts::setup(const Context & context)
    {
        m_texture1.loadFromFile(
            (context.settings.media_path / "image/ghost" / "ghost-1.png").string());

        m_texture2.loadFromFile(
            (context.settings.media_path / "image/ghost" / "ghost-2.png").string());

        m_texture3.loadFromFile(
            (context.settings.media_path / "image/ghost" / "ghost-3.png").string());

        m_texture1.setSmooth(true);
        m_texture2.setSmooth(true);
        m_texture3.setSmooth(true);
    }

    void Ghosts::clear() { m_ghosts.clear(); }

    void Ghosts::update(const Context &, const float frameTimeSec)
    {
        for (Ghost & ghost : m_ghosts)
        {
            ghost.sprite.move(0.0f, -(ghost.speed * frameTimeSec));
        }
    }

    void Ghosts::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        states.blendMode = sf::BlendAdd;

        for (const Ghost & ghost : m_ghosts)
        {
            if (!ghost.is_alive)
            {
                continue;
            }

            target.draw(ghost.sprite, states);
        }
    }

    void Ghosts::move(const sf::Vector2f & move)
    {
        for (Ghost & ghost : m_ghosts)
        {
            ghost.sprite.move(move);
        }
    }

} // namespace halloween
