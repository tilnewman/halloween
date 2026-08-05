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
        : m_texture1{}
        , m_texture2{}
        , m_texture3{}
        , m_spawnPoints{}
        , m_ghosts{}
        , m_spawnMinTimeSec{ 6.0f }
        , m_spawnMaxTimeSec{ 12.0f }
        , m_floatSpeedMin{ 20.0f }
        , m_floatSpeedMax{ 80.0f }
    {
        // probably never more than a hundred ghost spawn points in a level
        m_spawnPoints.reserve(100);

        // not likely to be more than a dozen on screen at once
        m_ghosts.reserve(100);
    }

    void Ghosts::setup(const Context & t_context)
    {
        util::TextureLoader::load(
            m_texture1, (t_context.settings.media_path / "image" / "ghost" / "ghost-1.png"));

        util::TextureLoader::load(
            m_texture2, (t_context.settings.media_path / "image" / "ghost" / "ghost-2.png"));

        util::TextureLoader::load(
            m_texture3, (t_context.settings.media_path / "image" / "ghost" / "ghost-3.png"));

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

    void Ghosts::add(const Context & t_context, const sf::FloatRect & t_region, const std::string &)
    {
        m_spawnPoints.emplace_back(
            util::center(t_region), t_context.random.fromTo(m_spawnMinTimeSec, m_spawnMaxTimeSec));
    }

    void Ghosts::update(const Context & t_context, const float t_frameTimeSec)
    {
        const sf::FloatRect mapRect{ t_context.layout.mapRegion() };
        for (GhostSpawnPoint & spawnPoint : m_spawnPoints)
        {
            spawnPoint.time_remaining_sec -= t_frameTimeSec;

            if ((spawnPoint.time_remaining_sec > 0.0f) || !mapRect.contains(spawnPoint.position))
            {
                continue;
            }

            spawnPoint.time_remaining_sec =
                t_context.random.fromTo(m_spawnMinTimeSec, m_spawnMaxTimeSec);

            Ghost & newGhost{ m_ghosts.emplace_back(
                t_context.random.fromTo(m_floatSpeedMin, m_floatSpeedMax)) };

            const int selection{ t_context.random.fromTo(1, 3) };
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

            const float scale{ t_context.random.fromTo(0.3f, 0.5f) };
            newGhost.sprite.setScale({ scale, scale });
            util::setOriginToCenter(newGhost.sprite);
            newGhost.sprite.setPosition(spawnPoint.position);
            newGhost.sprite.setColor(sf::Color(255, 255, 255, 0));
        }

        for (Ghost & ghost : m_ghosts)
        {
            ghost.sprite.move({ 0.0f, -(ghost.speed * t_frameTimeSec) });

            std::uint8_t alpha{ ghost.sprite.getColor().a };

            if (ghost.is_fading_in)
            {
                if (alpha < 95u)
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
                if (alpha >= 5u)
                {
                    alpha -= 5u;
                }
                else
                {
                    ghost.is_alive = false;
                }
            }

            ghost.sprite.setColor(sf::Color(255, 255, 255, alpha));
        }

        std::erase_if(m_ghosts, [](const Ghost & t_ghost) { return !t_ghost.is_alive; });
    }

    void Ghosts::draw(
        const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        if (m_ghosts.empty())
        {
            return;
        }

        const sf::FloatRect mapRect{ t_context.layout.mapRegion() };

        t_states.blendMode = sf::BlendAdd;

        for (const Ghost & ghost : m_ghosts)
        {
            if (mapRect.findIntersection(ghost.sprite.getGlobalBounds()))
            {
                t_target.draw(ghost.sprite, t_states);
            }
        }
    }

    void Ghosts::moveWithMap(const sf::Vector2f & t_move)
    {
        for (GhostSpawnPoint & spawnPoint : m_spawnPoints)
        {
            spawnPoint.position += t_move;
        }

        for (Ghost & ghost : m_ghosts)
        {
            ghost.sprite.move(t_move);
        }
    }

} // namespace halloween
