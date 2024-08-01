// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// slime.cpp
//
#include "slime.hpp"

#include "check-macros.hpp"
#include "context.hpp"
#include "info-region.hpp"
#include "level.hpp"
#include "screen-regions.hpp"
#include "settings.hpp"
#include "sfml-util.hpp"
#include "sound-player.hpp"

#include <algorithm>
#include <iostream>

#include <SFML/Graphics/RenderTarget.hpp>

namespace halloween
{

    Slimes::Slimes()
        : m_textures()
        , m_rects()
        , m_slimes()
        , m_timePerTextureSec(0.0333f)
        , m_elapsedTimeSec(0.0f)
        , m_textureCount(30)
        , m_deathAnims()
    {
        // probably never more than one dozen of each in a level
        m_rects.reserve(100);
        m_slimes.reserve(100);
        m_deathAnims.reserve(100);
    }

    void Slimes::setup(const Settings & settings)
    {
        m_textures.resize(m_textureCount);
        for (std::size_t i(0); i < m_textureCount; ++i)
        {
            std::string str;
            str = (settings.media_path / "image/slime" / "slime-").string();
            str += std::to_string(i);
            str += ".png";

            m_textures.at(i).loadFromFile(str);
            m_textures.at(i).setSmooth(true);
        }
    }

    void Slimes::spawnAll(const Context & context)
    {
        for (const sf::FloatRect & rect : m_rects)
        {
            const float speed{ context.random.fromTo(20.0f, 50.0f) };

            Slime slime(context.random.boolean(), rect, speed);

            slime.texture_index = context.random.index(m_textures);
            slime.sprite.setTexture(m_textures.at(slime.texture_index));
            slime.sprite.setScale({ 0.5f, 0.5f });

            const float posX{ rect.left + (rect.width / 2.0f) };
            const float posY{ (rect.top + rect.height) - slime.sprite.getGlobalBounds().height };
            slime.sprite.setPosition(posX, posY);

            m_slimes.push_back(slime);
        }
    }

    void Slimes::update(const Context &, const float frameTimeSec)
    {
        // animate
        m_elapsedTimeSec += frameTimeSec;
        if (m_elapsedTimeSec > m_timePerTextureSec)
        {
            for (Slime & slime : m_slimes)
            {
                ++slime.texture_index;
                if (slime.texture_index >= m_textureCount)
                {
                    slime.texture_index = 0;
                }

                slime.sprite.setTexture(m_textures.at(slime.texture_index));
            }

            m_elapsedTimeSec -= m_timePerTextureSec;
        }

        // move
        for (Slime & slime : m_slimes)
        {
            const float stride{ slime.speed * frameTimeSec };

            if (slime.is_moving_left)
            {
                slime.sprite.move(-stride, 0.0f);

                if (slime.sprite.getGlobalBounds().left < slime.rect.left)
                {
                    slime.is_moving_left = !slime.is_moving_left;
                }
            }
            else
            {
                slime.sprite.move(stride, 0.0f);

                if (util::right(slime.sprite.getGlobalBounds()) > util::right(slime.rect))
                {
                    slime.is_moving_left = !slime.is_moving_left;
                }
            }
        }

        // death animations
        bool areAnyDeathAnimsFinished = false;
        for (SlimeDeathAnim & anim : m_deathAnims)
        {
            anim.sprite.scale(0.8f, 0.8f);

            if (anim.sprite.getScale().x < 0.01f)
            {
                anim.is_visible = false;
                areAnyDeathAnimsFinished = true;
            }
        }

        if (areAnyDeathAnimsFinished)
        {
            m_deathAnims.erase(
                std::remove_if(
                    std::begin(m_deathAnims),
                    std::end(m_deathAnims),
                    [](const SlimeDeathAnim & anim) { return !anim.is_visible; }),
                std::end(m_deathAnims));
        }
    }

    void Slimes::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        for (const Slime & slime : m_slimes)
        {
            target.draw(slime.sprite, states);
        }

        for (const SlimeDeathAnim & anim : m_deathAnims)
        {
            target.draw(anim.sprite, states);
        }
    }

    void Slimes::move(const sf::Vector2f & move)
    {
        for (sf::FloatRect & rect : m_rects)
        {
            rect.left += move.x;
        }

        for (Slime & slime : m_slimes)
        {
            slime.sprite.move(move);
            slime.rect.left += move.x;
        }
    }

    bool Slimes::doesCollideWithAny(const sf::FloatRect & rect) const
    {
        for (const Slime & slime : m_slimes)
        {
            if (slime.sprite.getGlobalBounds().intersects(rect))
            {
                return true;
            }
        }

        return false;
    }

    bool Slimes::attack(const sf::FloatRect & attackRect)
    {
        bool wereAnyKilled = false;
        for (Slime & slime : m_slimes)
        {
            if (slime.sprite.getGlobalBounds().intersects(attackRect))
            {
                slime.is_alive = false;
                wereAnyKilled = true;
                m_deathAnims.emplace_back(slime.sprite);
            }
        }

        // remove any dead
        if (wereAnyKilled)
        {
            m_slimes.erase(
                std::remove_if(
                    std::begin(m_slimes),
                    std::end(m_slimes),
                    [](const Slime & slime) { return !slime.is_alive; }),
                std::end(m_slimes));
        }

        return wereAnyKilled;
    }

} // namespace halloween
