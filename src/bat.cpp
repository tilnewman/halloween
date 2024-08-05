// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// bat.cpp
//
#include "bat.hpp"

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

    Bats::Bats()
        : m_batCount(5)
        , m_textures()
        , m_bats()
        , m_timePerTextureSec(0.0333f)
        , m_elapsedTimeSec(0.0f)
        , m_deathAnims()
    {
        // probably never more than one dozen in a level
        m_bats.reserve(100);
        m_deathAnims.reserve(100);
    }

    void Bats::setup(const Settings & settings)
    {
        m_textures.resize(m_batCount);

        for (std::size_t batIndex(0); batIndex < m_textures.size(); ++batIndex)
        {
            BatTextures & textureSet = m_textures.at(batIndex);

            const std::string preStr = (settings.media_path / "image/bat" / "bat")
                                           .string()
                                           .append(std::to_string(batIndex + 1));

            const std::size_t flyingFrameCount{ 10 };
            textureSet.flying.resize(flyingFrameCount);
            for (std::size_t textureIndex(0); textureIndex < flyingFrameCount; ++textureIndex)
            {
                std::string pathStr = preStr;
                pathStr += "-fly-";
                pathStr += std::to_string(textureIndex);
                pathStr += ".png";

                textureSet.flying.at(textureIndex).loadFromFile(pathStr);
                textureSet.flying.at(textureIndex).setSmooth(true);
            }

            const std::size_t dyingFrameCount{ 6 };
            textureSet.dying.resize(dyingFrameCount);
            for (std::size_t textureIndex(0); textureIndex < dyingFrameCount; ++textureIndex)
            {
                std::string pathStr = preStr;
                pathStr += "-die-";
                pathStr += std::to_string(textureIndex);
                pathStr += ".png";

                textureSet.dying.at(textureIndex).loadFromFile(pathStr);
                textureSet.dying.at(textureIndex).setSmooth(true);
            }
        }
    }

    void Bats::clear()
    {
        m_bats.clear();
        m_deathAnims.clear();
    }

    void Bats::add(Context & context, const sf::FloatRect & rect)
    {
        Bat & bat = m_bats.emplace_back(
            context.random.index(m_textures),
            context.random.boolean(),
            rect,
            context.random.fromTo(20.0f, 75.0f));

        // bats are always flying
        auto & textures = m_textures.at(bat.bat_index).flying;
        bat.texture_index = context.random.index(textures);
        bat.sprite.setTexture(textures.at(bat.texture_index));
        bat.sprite.setScale({ 0.25f, 0.25f });
        util::setOriginToCenter(bat.sprite);

        const float posX{ rect.left + (rect.width / 2.0f) };
        const float posY{ util::bottom(rect) - (bat.sprite.getGlobalBounds().height * 0.5f) };
        bat.sprite.setPosition(posX, posY);

        m_bats.push_back(bat);
    }

    void Bats::update(Context &, const float frameTimeSec)
    {
        // animate bats
        m_elapsedTimeSec += frameTimeSec;
        if (m_elapsedTimeSec > m_timePerTextureSec)
        {
            m_elapsedTimeSec -= m_timePerTextureSec;

            for (Bat & bat : m_bats)
            {
                ++bat.texture_index;
                if (bat.texture_index >= m_textures.at(bat.bat_index).flying.size())
                {
                    bat.texture_index = 0;
                }

                bat.sprite.setTexture(m_textures.at(bat.bat_index).flying.at(bat.texture_index));
            }
        }

        // move bats
        for (Bat & bat : m_bats)
        {
            const float stride{ bat.speed * frameTimeSec };

            if (bat.is_moving_left)
            {
                bat.sprite.move(-stride, 0.0f);

                if (bat.sprite.getGlobalBounds().left < bat.rect.left)
                {
                    bat.is_moving_left = false;
                }
            }
            else
            {
                bat.sprite.move(stride, 0.0f);

                if (util::right(bat.sprite.getGlobalBounds()) > util::right(bat.rect))
                {
                    bat.is_moving_left = true;
                }
            }
        }

        // death animations
        bool areAnyDeathAnimsFinished = false;
        for (BatDeathAnim & anim : m_deathAnims)
        {
            anim.sprite.scale(0.85f, 0.85f);

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
                    [](const BatDeathAnim & anim) { return !anim.is_visible; }),
                std::end(m_deathAnims));
        }
    }

    void Bats::draw(const Context &, sf::RenderTarget & target, sf::RenderStates states) const
    {
        for (const Bat & bat : m_bats)
        {
            target.draw(bat.sprite, states);
        }

        for (const BatDeathAnim & anim : m_deathAnims)
        {
            target.draw(anim.sprite, states);
        }
    }

    void Bats::moveWithMap(const sf::Vector2f & move)
    {
        for (Bat & bat : m_bats)
        {
            bat.sprite.move(move);
            bat.rect.left += move.x;
        }
    }

    bool Bats::doesAvatarCollideWithAnyAndDie(const sf::FloatRect & avatarRect) const
    {
        // TODO reduce the collision rect so the wings don't kill the player

        for (const Bat & bat : m_bats)
        {
            if (bat.sprite.getGlobalBounds().intersects(avatarRect))
            {
                return true;
            }
        }

        return false;
    }

    bool Bats::attack(const sf::FloatRect & attackRect)
    {
        bool wereAnyKilled = false;
        for (Bat & bat : m_bats)
        {
            if (bat.sprite.getGlobalBounds().intersects(attackRect))
            {
                bat.is_alive = false;
                wereAnyKilled = true;
                m_deathAnims.emplace_back(bat.sprite);
            }
        }

        // remove any dead
        if (wereAnyKilled)
        {
            m_bats.erase(
                std::remove_if(
                    std::begin(m_bats),
                    std::end(m_bats),
                    [](const Bat & bat) { return !bat.is_alive; }),
                std::end(m_bats));
        }

        return wereAnyKilled;
    }

} // namespace halloween
