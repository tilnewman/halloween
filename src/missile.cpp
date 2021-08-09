// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// missile.cpp
//
#include "missile.hpp"

#include "context.hpp"
#include "level.hpp"
#include "screen-regions.hpp"
#include "settings.hpp"
#include "sound-player.hpp"
#include "util.hpp"

#include <algorithm>

namespace halloween
{

    Missiles::Missiles()
        : m_texture()
        , m_velocity(1000.0f, 0.0f)
        , m_missiles()
    {
        // anything more than "dozens" will work here
        m_missiles.reserve(100);
    }

    void Missiles::setup(const Settings & settings)
    {
        const std::string PATH = (settings.media_path / "image" / "kunai.png").string();
        m_texture.loadFromFile(PATH);
        m_texture.setSmooth(true);
    }

    void Missiles::add(const sf::Vector2f & POSITION, const bool IS_MOVING_RIGHT)
    {
        m_missiles.emplace_back(POSITION, IS_MOVING_RIGHT);
    }

    void Missiles::update(Context & context, const float FRAME_TIME_SEC)
    {
        sf::Sprite sprite(m_texture);

        for (Missile & missile : m_missiles)
        {
            if (missile.is_moving_right)
            {
                missile.position += (m_velocity * FRAME_TIME_SEC);
            }
            else
            {
                missile.position -= (m_velocity * FRAME_TIME_SEC);
            }

            missile.is_alive = context.layout.mapRegion().contains(missile.position);

            if (missile.is_alive)
            {
                if (missile.is_moving_right)
                {
                    sprite.setScale(0.333f, 0.5f);
                }
                else
                {
                    sprite.setScale(-0.333f, 0.5f);
                }

                sprite.setPosition(missile.position);

                const sf::FloatRect MISSILE_RECT = sprite.getGlobalBounds();
                for (const sf::FloatRect & COLL_RECT : context.level.walk_collisions)
                {
                    if (MISSILE_RECT.intersects(COLL_RECT))
                    {
                        missile.is_alive = false;
                        context.audio.play("metal-miss");
                        break;
                    }
                }
            }
        }

        m_missiles.erase(
            std::remove_if(
                std::begin(m_missiles),
                std::end(m_missiles),
                [](const Missile & MISSILE) { return !MISSILE.is_alive; }),
            std::end(m_missiles));
    }

    void Missiles::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        sf::Sprite sprite(m_texture);

        for (const Missile & MISSILE : m_missiles)
        {
            if (MISSILE.is_moving_right)
            {
                sprite.setScale(0.333f, 0.5f);
            }
            else
            {
                sprite.setScale(-0.333f, 0.5f);
            }

            sprite.setPosition(MISSILE.position);

            target.draw(sprite, states);
        }
    }

} // namespace halloween
