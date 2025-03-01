// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// missile.cpp
//
#include "missile.hpp"

#include "bat.hpp"
#include "context.hpp"
#include "info-region.hpp"
#include "level-stats.hpp"
#include "level.hpp"
#include "mushroom-boss.hpp"
#include "screen-regions.hpp"
#include "settings.hpp"
#include "sfml-util.hpp"
#include "slime.hpp"
#include "sound-player.hpp"
#include "texture-loader.hpp"

#include <algorithm>

#include <SFML/Graphics/RenderTarget.hpp>

namespace halloween
{

    Missiles::Missiles()
        : m_texture()
        , m_velocity(0.0f, 0.0f)
        , m_scale(0.0f, 0.0f)
        , m_missiles()
    {
        // anything more than dozens will work here
        m_missiles.reserve(100);
    }

    void Missiles::setup(const Settings & settings)
    {
        m_velocity.x = settings.dart_speed;
        m_scale = settings.dart_scale;

        util::TextureLoader::load(m_texture, (settings.media_path / "image" / "kunai.png"), true);
    }

    void Missiles::add(const sf::Vector2f & position, const bool isMovingRight)
    {
        Missile & dart = m_missiles.emplace_back(isMovingRight, m_texture);
        dart.sprite.setScale(m_scale);

        if (!dart.is_moving_right)
        {
            dart.sprite.scale({ -1.0f, 1.0f });
        }

        util::setOriginToCenter(dart.sprite);
        dart.sprite.setPosition(position);
    }

    void Missiles::update(Context & context, const float frameTimeSec)
    {
        bool wereAnyKilled = false;
        for (Missile & missile : m_missiles)
        {
            if (missile.is_moving_right)
            {
                missile.sprite.move(m_velocity * frameTimeSec);
            }
            else
            {
                missile.sprite.move(-m_velocity * frameTimeSec);
            }

            const sf::FloatRect missileRect = missile.sprite.getGlobalBounds();

            missile.is_alive = context.layout.mapRegion().findIntersection(missileRect).has_value();
            if (!missile.is_alive)
            {
                continue;
            }

            if (context.slimes.attack(context, missileRect) ||
                context.bats.attack(context, missileRect))
            {
                wereAnyKilled = true;
                missile.is_alive = false;
                continue;
            }

            if (context.boss.attack(context, missileRect))
            {
                wereAnyKilled = true;
                missile.is_alive = false;
                continue;
            }

            for (const sf::FloatRect & collRect : context.level.walk_collisions)
            {
                if (missileRect.findIntersection(collRect))
                {
                    wereAnyKilled = true;
                    missile.is_alive = false;
                    context.audio.play("metal-miss");
                    break;
                }
            }
        }

        if (wereAnyKilled)
        {
            m_missiles.erase(
                std::remove_if(
                    std::begin(m_missiles),
                    std::end(m_missiles),
                    [](const Missile & missile) { return !missile.is_alive; }),
                std::end(m_missiles));
        }
    }

    void Missiles::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        for (const Missile & missile : m_missiles)
        {
            target.draw(missile.sprite, states);
        }
    }

} // namespace halloween
