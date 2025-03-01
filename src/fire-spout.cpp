// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// fire-spout.cpp
//
#include "fire-spout.hpp"

#include "context.hpp"
#include "info-region.hpp"
#include "level.hpp"
#include "screen-regions.hpp"
#include "settings.hpp"
#include "sfml-util.hpp"
#include "sound-player.hpp"
#include "texture-loader.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

namespace halloween
{

    FireSpouts::FireSpouts()
        : m_spoutTexture()
        , m_timePerFrame(0.1f)
        , m_fireSpouts()
        , m_fireTextures()
        , m_timebetweenSpurtsMinSec(0.5f)
        , m_timebetweenSpurtsMaxSec(3.5f)
    {
        // probably no more than a dozen in any given map
        m_fireSpouts.reserve(100);
    }

    void FireSpouts::setup(const Settings & settings)
    {
        util::TextureLoader::load(
            m_spoutTexture, (settings.media_path / "image/fire-spouts.png"), true);

        const std::size_t fireTextureCount{ 14 };
        m_fireTextures.reserve(fireTextureCount);
        for (std::size_t i(1); i <= fireTextureCount; ++i)
        {
            std::string filePath = (settings.media_path / "image/fire/" / "fire-").string();
            filePath += std::to_string(i);
            filePath += ".png";

            sf::Texture & texture = m_fireTextures.emplace_back();
            util::TextureLoader::load(texture, filePath, true);
        }
    }

    void FireSpouts::add(Context & context, const sf::FloatRect & region)
    {
        FireSpout & spout = m_fireSpouts.emplace_back(m_spoutTexture, m_fireTextures.at(0));

        spout.spout_sprite.setTextureRect({ { 0, 10 }, { 35, 22 } });
        spout.spout_sprite.setScale({ 2.0f, 2.0f });

        const sf::FloatRect spoutBounds = spout.spout_sprite.getGlobalBounds();

        spout.spout_sprite.setPosition({ (util::center(region).x - (spoutBounds.size.x * 0.5f)),
                                         (util::bottom(region) - spoutBounds.size.y) });

        spout.fire_sprite.setScale({ 2.0f, 2.0f });

        const sf::FloatRect fireBounds = spout.fire_sprite.getGlobalBounds();

        spout.fire_sprite.setPosition({ (util::center(spoutBounds).x - (fireBounds.size.x * 0.5f)),
                                        (spout.spout_sprite.getPosition().y - fireBounds.size.y) });

        spout.is_spurting = false;
        spout.texture_index = 0;

        spout.time_between_spurts_sec =
            context.random.fromTo(m_timebetweenSpurtsMinSec, m_timebetweenSpurtsMaxSec);
    }

    void FireSpouts::clear() { m_fireSpouts.clear(); }

    void FireSpouts::update(Context &, const float frameTimeSec)
    {
        for (FireSpout & spout : m_fireSpouts)
        {
            if (spout.is_spurting)
            {
                spout.elapsed_time_sec += frameTimeSec;
                if (spout.elapsed_time_sec < m_timePerFrame)
                {
                    continue;
                }

                spout.elapsed_time_sec -= m_timePerFrame;

                ++spout.texture_index;
                if (spout.texture_index >= m_fireTextures.size())
                {
                    spout.texture_index = 0;
                    spout.elapsed_time_sec = 0.0f;
                    spout.is_spurting = false;
                }

                // not all the fire spurting textures are the same size, so reposition every frame
                spout.fire_sprite.setTexture(m_fireTextures.at(spout.texture_index), true);

                const sf::FloatRect fireBounds = spout.fire_sprite.getGlobalBounds();

                spout.fire_sprite.setPosition(
                    { (util::center(spout.spout_sprite.getGlobalBounds()).x -
                       (fireBounds.size.x * 0.5f)),
                      (spout.spout_sprite.getPosition().y - fireBounds.size.y) });
            }
            else
            {
                spout.elapsed_time_sec += frameTimeSec;
                if (spout.elapsed_time_sec < spout.time_between_spurts_sec)
                {
                    continue;
                }

                spout.elapsed_time_sec = 0.0f;
                spout.is_spurting = true;
            }
        }
    }

    void FireSpouts::draw(
        const Context & context, sf::RenderTarget & target, sf::RenderStates states) const
    {
        for (const FireSpout & spout : m_fireSpouts)
        {
            target.draw(spout.spout_sprite, states);

            if (spout.is_spurting)
            {
                if (context.layout.mapRegion().findIntersection(
                        spout.fire_sprite.getGlobalBounds()))
                {
                    target.draw(spout.fire_sprite, states);
                }
            }
        }
    }

    void FireSpouts::moveWithMap(const sf::Vector2f & move)
    {
        for (FireSpout & spout : m_fireSpouts)
        {
            spout.spout_sprite.move(move);
            spout.fire_sprite.move(move);
        }
    }

    bool FireSpouts::doesAvatarCollideWithAnyAndDie(const sf::FloatRect & avatarRect) const
    {
        for (const FireSpout & spout : m_fireSpouts)
        {
            if (!spout.is_spurting)
            {
                continue;
            }

            const sf::FloatRect spoutRect = spout.fire_sprite.getGlobalBounds();
            if (avatarRect.findIntersection(spoutRect))
            {
                return true;
            }
        }

        return false;
    }

    void FireSpouts::appendCollisions(std::vector<sf::FloatRect> & rects) const
    {
        for (const FireSpout & spout : m_fireSpouts)
        {
            rects.push_back(spout.spout_sprite.getGlobalBounds());
        }
    }

} // namespace halloween
