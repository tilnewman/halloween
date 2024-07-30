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

#include <SFML/Graphics/RenderTarget.hpp>

namespace halloween
{

    Missiles::Missiles()
        : m_texture()
        , m_velocity(1000.0f, 0.0f)
        , m_missiles()
    {
        // anything more than dozens will work here
        m_missiles.reserve(100);
    }

    void Missiles::setup(const Settings & settings)
    {
        const std::string path = (settings.media_path / "image" / "kunai.png").string();
        m_texture.loadFromFile(path);
        m_texture.setSmooth(true);
    }

    void Missiles::add(const sf::Vector2f & position, const bool isMovingRight)
    {
        m_missiles.emplace_back(position, isMovingRight);
    }

    void Missiles::update(Context & context, const float frameTimeSec)
    {
        sf::Sprite sprite(m_texture);

        for (Missile & missile : m_missiles)
        {
            if (missile.is_moving_right)
            {
                missile.position += (m_velocity * frameTimeSec);
            }
            else
            {
                missile.position -= (m_velocity * frameTimeSec);
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

                const sf::FloatRect missileRect = sprite.getGlobalBounds();
                for (const sf::FloatRect & collRect : context.level.walk_collisions)
                {
                    if (missileRect.intersects(collRect))
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
                [](const Missile & missile) { return !missile.is_alive; }),
            std::end(m_missiles));
    }

    void Missiles::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        sf::Sprite sprite(m_texture);

        for (const Missile & missile : m_missiles)
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

            target.draw(sprite, states);
        }
    }

} // namespace halloween
