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
        : m_texture{}
        , m_velocity{}
        , m_scale{}
        , m_missiles{}
    {
        // anything more than dozens will work here
        m_missiles.reserve(100);
    }

    void Missiles::setup(const Settings & t_settings)
    {
        m_velocity.x = t_settings.dart_speed;
        m_scale = t_settings.dart_scale;

        util::TextureLoader::load(m_texture, (t_settings.media_path / "image" / "kunai.png"), true);
    }

    void Missiles::add(const sf::Vector2f & t_position, const bool t_isMovingRight)
    {
        Missile & dart{ m_missiles.emplace_back(t_isMovingRight, m_texture) };
        dart.sprite.setScale(m_scale);

        if (!dart.is_moving_right)
        {
            dart.sprite.scale({ -1.0f, 1.0f });
        }

        util::setOriginToCenter(dart.sprite);
        dart.sprite.setPosition(t_position);
    }

    void Missiles::update(const Context & t_context, const float t_frameTimeSec)
    {
        bool wereAnyKilled{ false };
        for (Missile & missile : m_missiles)
        {
            if (missile.is_moving_right)
            {
                missile.sprite.move(m_velocity * t_frameTimeSec);
            }
            else
            {
                missile.sprite.move(-m_velocity * t_frameTimeSec);
            }

            const sf::FloatRect missileRect{ missile.sprite.getGlobalBounds() };

            missile.is_alive =
                t_context.layout.mapRegion().findIntersection(missileRect).has_value();

            if (!missile.is_alive)
            {
                continue;
            }

            if (t_context.slimes.attack(t_context, missileRect) ||
                t_context.bats.attack(t_context, missileRect))
            {
                wereAnyKilled = true;
                missile.is_alive = false;
                continue;
            }

            if (t_context.boss.attack(t_context, missileRect))
            {
                wereAnyKilled = true;
                missile.is_alive = false;
                continue;
            }

            for (const sf::FloatRect & collRect : t_context.level.walkCollisions())
            {
                if (missileRect.findIntersection(collRect))
                {
                    wereAnyKilled = true;
                    missile.is_alive = false;
                    t_context.audio.play("metal-miss");
                    break;
                }
            }
        }

        if (wereAnyKilled)
        {
            std::erase_if(
                m_missiles, [](const Missile & t_missile) { return !t_missile.is_alive; });
        }
    }

    void Missiles::draw(sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        for (const Missile & missile : m_missiles)
        {
            t_target.draw(missile.sprite, t_states);
        }
    }

} // namespace halloween
