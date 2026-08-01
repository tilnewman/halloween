// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// bat.cpp
//
#include "bat.hpp"

#include "avatar.hpp"
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

    Bats::Bats()
        : m_batCount{ 5 }
        , m_textures{}
        , m_bats{}
        , m_timePerFrameBeforeSec{ 0.0275f }
        , m_timePerFrameAfterSec{ 0.0175f }
        , m_deathAnims{}
    {
        // probably never more than one dozen in a level
        m_bats.reserve(100);
        m_deathAnims.reserve(100);
    }

    void Bats::setup(const Context & t_context)
    {
        m_textures.resize(m_batCount);

        for (std::size_t batIndex{ 0 }; batIndex < m_textures.size(); ++batIndex)
        {
            BatTextures & textureSet{ m_textures.at(batIndex) };

            const std::string preStr{ (t_context.settings.media_path / "image" / "bat" / "bat")
                                          .string()
                                          .append(std::to_string(batIndex + 1)) };

            const std::size_t flyingFrameCount{ 10 };
            textureSet.flying.resize(flyingFrameCount);
            for (std::size_t textureIndex{ 0 }; textureIndex < flyingFrameCount; ++textureIndex)
            {
                std::string pathStr{ preStr };
                pathStr += "-fly-";
                pathStr += std::to_string(textureIndex);
                pathStr += ".png";

                util::TextureLoader::load(textureSet.flying.at(textureIndex), pathStr);
            }

            const std::size_t dyingFrameCount{ 6 };
            textureSet.dying.resize(dyingFrameCount);
            for (std::size_t textureIndex{ 0 }; textureIndex < dyingFrameCount; ++textureIndex)
            {
                std::string pathStr{ preStr };
                pathStr += "-die-";
                pathStr += std::to_string(textureIndex);
                pathStr += ".png";

                util::TextureLoader::load(textureSet.dying.at(textureIndex), pathStr);
            }
        }
    }

    void Bats::clear()
    {
        m_bats.clear();
        m_deathAnims.clear();
    }

    void Bats::add(const Context & t_context, const sf::FloatRect & t_rect, const std::string &)
    {
        Bat & bat{ m_bats.emplace_back(
            t_context.random.index(m_textures),
            t_context.random.boolean(),
            t_rect,
            t_context.random.fromTo(20.0f, 75.0f)) };

        // bats are always flying
        auto & textures{ m_textures.at(bat.bat_index).flying };
        bat.texture_index = t_context.random.index(textures);
        bat.sprite.setTexture(textures.at(bat.texture_index), true);
        bat.sprite.setScale({ 1.0f, 1.0f });
        util::setOriginToCenter(bat.sprite);

        const float posX{ t_rect.position.x + (t_rect.size.x / 2.0f) };
        const float posY{ util::bottom(t_rect) - (bat.sprite.getGlobalBounds().size.y * 0.5f) };
        bat.sprite.setPosition({ posX, posY });

        if (!bat.is_moving_left)
        {
            bat.sprite.scale({ -1.0f, 1.0f });
        }
    }

    void Bats::update(const Context & t_context, const float t_frameTimeSec)
    {
        const sf::FloatRect avatarRect{ t_context.avatar.collisionRect() };

        // move bats
        for (Bat & bat : m_bats)
        {
            bat.elpased_time_sec += t_frameTimeSec;

            const float timePerFrameSec{ (bat.has_spotted_player) ? m_timePerFrameAfterSec
                                                                  : m_timePerFrameBeforeSec };

            if (bat.elpased_time_sec > timePerFrameSec)
            {
                bat.elpased_time_sec -= timePerFrameSec;

                if (++bat.texture_index >= m_textures.at(bat.bat_index).flying.size())
                {
                    bat.texture_index = 0;
                }

                bat.sprite.setTexture(m_textures.at(bat.bat_index).flying.at(bat.texture_index));
            }

            if (!bat.has_spotted_player)
            {
                if (avatarRect.findIntersection(bat.rect))
                {
                    bat.has_spotted_player = true;

                    const bool isAvatarLeft{ util::center(avatarRect).x <
                                              util::center(bat.sprite.getGlobalBounds()).x };

                    if (isAvatarLeft != bat.is_moving_left)
                    {
                        bat.is_moving_left = !bat.is_moving_left;
                        bat.sprite.scale({ -1.0f, 1.0f });
                    }

                    t_context.audio.play("bat-sees-player");
                }
            }

            const float stride{ bat.speed * t_frameTimeSec *
                                ((bat.has_spotted_player) ? 2.0f : 1.0f) };

            if (bat.is_moving_left)
            {
                bat.sprite.move({ -stride, 0.0f });

                if (bat.sprite.getGlobalBounds().position.x < bat.rect.position.x)
                {
                    bat.is_moving_left = false;
                    bat.sprite.scale({ -1.0f, 1.0f });
                }
            }
            else
            {
                bat.sprite.move({ stride, 0.0f });

                if (util::right(bat.sprite.getGlobalBounds()) > util::right(bat.rect))
                {
                    bat.is_moving_left = true;
                    bat.sprite.scale({ -1.0f, 1.0f });
                }
            }
        }

        // death animations
        bool areAnyDeathAnimsFinished = false;
        for (BatDeathAnim & anim : m_deathAnims)
        {
            anim.death_elapsed_time_sec += t_frameTimeSec;
            if (anim.death_elapsed_time_sec > (m_timePerFrameBeforeSec * 5.0f))
            {
                anim.death_elapsed_time_sec -= (m_timePerFrameBeforeSec * 5.0f);

                if (++anim.texture_index < m_textures.at(anim.bat_index).dying.size())
                {
                    anim.sprite.setTexture(
                        m_textures.at(anim.bat_index).dying.at(anim.texture_index));
                }
            }

            anim.scale *= anim.scale;
            anim.sprite.scale({ anim.scale, anim.scale });

            const sf::Vector2f size{ anim.sprite.getGlobalBounds().size };
            if ((size.x * size.y) < 1.0f)
            {
                anim.is_visible = false;
                areAnyDeathAnimsFinished = true;
            }
        }

        if (areAnyDeathAnimsFinished)
        {
            std::erase_if(
                m_deathAnims, [](const BatDeathAnim & t_anim) { return !t_anim.is_visible; });
        }
    }

    void Bats::draw(
        const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        const sf::FloatRect mapRect{ t_context.layout.mapRegion() };

        for (const Bat & bat : m_bats)
        {
            if (mapRect.findIntersection(bat.sprite.getGlobalBounds()))
            {
                t_target.draw(bat.sprite, t_states);
            }
        }

        for (const BatDeathAnim & anim : m_deathAnims)
        {
            t_target.draw(anim.sprite, t_states);
        }
    }

    void Bats::moveWithMap(const sf::Vector2f & t_move)
    {
        for (Bat & bat : m_bats)
        {
            bat.sprite.move(t_move);
            bat.rect.position += t_move;
        }

        for (BatDeathAnim & anim : m_deathAnims)
        {
            anim.sprite.move(t_move);
        }
    }

    bool Bats::doesAvatarCollideWithAnyAndDie(const sf::FloatRect & t_avatarRect) const
    {
        for (const Bat & bat : m_bats)
        {
            const sf::FloatRect batCollRect{ util::scaleRectInPlaceCopy(
                bat.sprite.getGlobalBounds(), { 0.45f, 0.7f }) };

            if (batCollRect.findIntersection(t_avatarRect))
            {
                return true;
            }
        }

        return false;
    }

    bool Bats::attack(const Context & t_context, const sf::FloatRect & t_attackRect)
    {
        bool wereAnyKilled{ false };
        for (Bat & bat : m_bats)
        {
            if (bat.sprite.getGlobalBounds().findIntersection(t_attackRect))
            {
                bat.is_alive = false;
                wereAnyKilled = true;

                bat.sprite.setColor(sf::Color::Red);
                m_deathAnims.emplace_back(bat.bat_index, bat.sprite);

                ++t_context.stats.enemy_killed;
                t_context.info_region.scoreAdjust(t_context.settings.kill_bat_score);
            }
        }

        // remove any dead
        if (wereAnyKilled)
        {
            t_context.audio.play("metal-hit");
            t_context.audio.play("bat-death");
            std::erase_if(m_bats, [](const Bat & t_bat) { return !t_bat.is_alive; });
        }

        return wereAnyKilled;
    }

} // namespace halloween
