// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// slime.cpp
//
#include "slime.hpp"

#include "check-macros.hpp"
#include "context.hpp"
#include "info-region.hpp"
#include "level-stats.hpp"
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

    Slimes::Slimes()
        : m_textures{}
        , m_slimes{}
        , m_timePerTextureSec{ 0.0333f }
        , m_elapsedTimeSec{ 0.0f }
        , m_textureCount{ 30 }
        , m_deathAnims{}
    {
        // probably never more than one dozen of each in a level
        m_slimes.reserve(32);
        m_deathAnims.reserve(32);
    }

    void Slimes::setup(const Settings & t_settings)
    {
        m_textures.resize(m_textureCount);
        for (std::size_t i{ 0 }; i < m_textureCount; ++i)
        {
            std::string str;
            str = (t_settings.media_path / "image" / "slime" / "slime-").string();
            str += std::to_string(i);
            str += ".png";

            util::TextureLoader::load(m_textures.at(i), str, true);
        }
    }

    void Slimes::clear() { m_slimes.clear(); }

    void Slimes::add(const Context & t_context, const sf::FloatRect & t_rect)
    {
        const float speed{ t_context.random.fromTo(20.0f, 75.0f) };

        Slime slime(t_context.random.boolean(), t_rect, speed);

        slime.texture_index = t_context.random.index(m_textures);
        slime.sprite.setTexture(m_textures.at(slime.texture_index), true);
        slime.sprite.setScale({ 0.5f, 0.5f });
        util::setOriginToCenter(slime.sprite);

        const float posX{ t_rect.position.x + (t_rect.size.x / 2.0f) };
        const float posY{ util::bottom(t_rect) - (slime.sprite.getGlobalBounds().size.y * 0.5f) };
        slime.sprite.setPosition({ posX, posY });

        m_slimes.push_back(slime);
    }

    void Slimes::update(const Context &, const float t_frameTimeSec)
    {
        // animate
        m_elapsedTimeSec += t_frameTimeSec;
        if (m_elapsedTimeSec > m_timePerTextureSec)
        {
            for (Slime & slime : m_slimes)
            {
                ++slime.texture_index;
                if (slime.texture_index >= m_textureCount)
                {
                    slime.texture_index = 0;
                }

                slime.sprite.setTexture(m_textures.at(slime.texture_index), true);
            }

            m_elapsedTimeSec -= m_timePerTextureSec;
        }

        // move
        for (Slime & slime : m_slimes)
        {
            const float stride{ slime.speed * t_frameTimeSec };

            if (slime.is_moving_left)
            {
                slime.sprite.move({ -stride, 0.0f });

                if (slime.sprite.getGlobalBounds().position.x < slime.rect.position.x)
                {
                    slime.is_moving_left = false;
                }
            }
            else
            {
                slime.sprite.move({ stride, 0.0f });

                if (util::right(slime.sprite.getGlobalBounds()) > util::right(slime.rect))
                {
                    slime.is_moving_left = true;
                }
            }
        }

        // death animations
        bool areAnyDeathAnimsFinished = false;
        for (SlimeDeathAnim & anim : m_deathAnims)
        {
            anim.sprite.scale({ 0.975f, 0.975f });

            if (anim.sprite.getScale().x < 0.1f)
            {
                anim.is_visible = false;
                areAnyDeathAnimsFinished = true;
            }
        }

        if (areAnyDeathAnimsFinished)
        {
            std::erase_if(
                m_deathAnims, [](const SlimeDeathAnim & t_anim) { return !t_anim.is_visible; });
        }
    }

    void Slimes::draw(
        const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        for (const Slime & slime : m_slimes)
        {
            if (t_context.layout.mapRegion().findIntersection(slime.sprite.getGlobalBounds()))
            {
                t_target.draw(slime.sprite, t_states);
            }
        }

        for (const SlimeDeathAnim & anim : m_deathAnims)
        {
            t_target.draw(anim.sprite, t_states);
        }
    }

    void Slimes::moveWithMap(const sf::Vector2f & t_move)
    {
        for (Slime & slime : m_slimes)
        {
            slime.sprite.move(t_move);
            slime.rect.position.x += t_move.x;
        }

        for (SlimeDeathAnim & anim : m_deathAnims)
        {
            anim.sprite.move(t_move);
        }
    }

    bool Slimes::doesAvatarCollideWithAnyAndDie(const sf::FloatRect & t_avatarRect) const
    {
        for (const Slime & slime : m_slimes)
        {
            if (slime.sprite.getGlobalBounds().findIntersection(t_avatarRect))
            {
                return true;
            }
        }

        return false;
    }

    bool Slimes::attack(const Context & t_context, const sf::FloatRect & t_attackRect)
    {
        bool wereAnyKilled{ false };
        for (Slime & slime : m_slimes)
        {
            if (slime.sprite.getGlobalBounds().findIntersection(t_attackRect))
            {
                slime.is_alive = false;
                wereAnyKilled = true;

                auto & anim{ m_deathAnims.emplace_back(slime.sprite) };
                anim.sprite.setColor(sf::Color::Red);

                t_context.audio.play("squish");
                ++t_context.stats.enemy_killed;
                t_context.info_region.scoreAdjust(t_context.settings.kill_slime_score);

                break;
            }
        }

        // remove any dead
        if (wereAnyKilled)
        {
            std::erase_if(m_slimes, [](const Slime & t_slime) { return !t_slime.is_alive; });
        }

        return wereAnyKilled;
    }

} // namespace halloween
