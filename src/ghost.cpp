// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// ghost.cpp
//
#include "ghost.hpp"

#include "check-macros.hpp"
#include "context.hpp"
#include "info-region.hpp"
#include "level.hpp"
#include "screen-regions.hpp"
#include "settings.hpp"
#include "sfml-util.hpp"
#include "sound-player.hpp"
#include "texture-loader.hpp"

#include <algorithm>
#include <iostream>

#include <SFML/Graphics/RenderTarget.hpp>

namespace halloween
{

    Ghosts::Ghosts()
        : m_texture1()
        , m_texture2()
        , m_texture3()
        , m_spawnPoints()
        , m_ghosts()
        , m_spawnMinTimeSec(6.0f)
        , m_spawnMaxTimeSec(12.0f)
        , m_floatSpeedMin(50.0f)
        , m_floatSpeedMax(100.0f)
    {
        // probably never more than a hundred ghost spawn points in a level
        m_spawnPoints.reserve(100);

        // not likely to be more than a dozen on screen at once
        m_ghosts.reserve(100);
    }

    void Ghosts::setup(const Settings & settings)
    {
        util::TextureLoader::load(
            m_texture1, (settings.media_path / "image/ghost/ghost-1.png"), true);

        util::TextureLoader::load(
            m_texture2, (settings.media_path / "image/ghost/ghost-2.png"), true);
        
        util::TextureLoader::load(
            m_texture3, (settings.media_path / "image/ghost/ghost-3.png"), true);

        M_CHECK(
            (m_spawnMaxTimeSec > m_spawnMinTimeSec),
            "Ghosts::m_spawnMaxTimeSec was not less than Ghosts::m_spawnMinTimeSec.");

        M_CHECK(
            (m_floatSpeedMax > m_floatSpeedMin),
            "Ghosts::m_floatSpeedMax was not less than Ghosts::m_floatSpeedMin.");
    }

    void Ghosts::clear()
    {
        m_spawnPoints.clear();
        m_ghosts.clear();
    }

    void Ghosts::add(Context & context, const sf::FloatRect & region)
    {
        m_spawnPoints.emplace_back(
            util::center(region), context.random.fromTo(m_spawnMinTimeSec, m_spawnMaxTimeSec));
    }

    void Ghosts::update(Context & context, const float frameTimeSec)
    {
        for (GhostSpawnPoint & spawnPoint : m_spawnPoints)
        {
            spawnPoint.time_remaining_sec -= frameTimeSec;

            if ((spawnPoint.time_remaining_sec > 0.0f) ||
                !context.layout.mapRegion().contains(spawnPoint.position))
            {
                continue;
            }

            spawnPoint.time_remaining_sec =
                context.random.fromTo(m_spawnMinTimeSec, m_spawnMaxTimeSec);

            Ghost & newGhost =
                m_ghosts.emplace_back(context.random.fromTo(m_floatSpeedMin, m_floatSpeedMax));

            const int selection = context.random.fromTo(1, 3);
            if (selection == 1)
            {
                newGhost.sprite.setTexture(m_texture1, true);
            }
            else if (selection == 2)
            {
                newGhost.sprite.setTexture(m_texture2, true);
            }
            else
            {
                newGhost.sprite.setTexture(m_texture3, true);
            }

            newGhost.sprite.setScale(context.settings.ghost_scale);
            util::setOriginToCenter(newGhost.sprite);
            newGhost.sprite.setPosition(spawnPoint.position);
            newGhost.sprite.setColor(sf::Color(255, 255, 255, 0));
        }

        for (Ghost & ghost : m_ghosts)
        {
            ghost.sprite.move({ 0.0f, -(ghost.speed * frameTimeSec) });

            std::uint8_t alpha = ghost.sprite.getColor().a;

            if (ghost.is_fading_in)
            {
                if (alpha < 95)
                {
                    ++alpha;
                }
                else
                {
                    ghost.is_fading_in = false;
                }
            }
            else
            {
                if (alpha >= 5)
                {
                    alpha -= 5;
                }
                else
                {
                    ghost.is_alive = false;
                }
            }

            ghost.sprite.setColor(sf::Color(255, 255, 255, alpha));
        }

        m_ghosts.erase(
            std::remove_if(
                std::begin(m_ghosts),
                std::end(m_ghosts),
                [](const Ghost & ghost) { return !ghost.is_alive; }),
            std::end(m_ghosts));
    }

    void Ghosts::draw(
        const Context & context, sf::RenderTarget & target, sf::RenderStates states) const
    {
        states.blendMode = sf::BlendAdd;

        for (const Ghost & ghost : m_ghosts)
        {
            if (context.layout.mapRegion().findIntersection(ghost.sprite.getGlobalBounds()))
            {
                target.draw(ghost.sprite, states);
            }
        }
    }

    void Ghosts::moveWithMap(const sf::Vector2f & move)
    {
        for (GhostSpawnPoint & spawnPoint : m_spawnPoints)
        {
            spawnPoint.position += move;
        }

        for (Ghost & ghost : m_ghosts)
        {
            ghost.sprite.move(move);
        }
    }

} // namespace halloween
