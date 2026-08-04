// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// zombies.cpp
//
#include "zombies.hpp"

#include "context.hpp"
#include "info-region.hpp"
#include "level-stats.hpp"
#include "settings.hpp"
#include "sound-player.hpp"

namespace halloween
{

    ZombieObjectManager::ZombieObjectManager()
        : m_zombies{}
        , m_textureManager{}
    {}

    void ZombieObjectManager::add(
        const Context & t_context, const sf::FloatRect & t_rect, const std::string &)
    {
        m_zombies.emplace_back(t_context, t_rect, m_textureManager);
    }

    void ZombieObjectManager::update(const Context & t_context, const float t_frameTimeSec)
    {
        for (Zombie & zombie : m_zombies)
        {
            zombie.update(t_context, t_frameTimeSec);
        }
    }

    void ZombieObjectManager::moveWithMap(const sf::Vector2f & t_move)
    {
        for (Zombie & zombie : m_zombies)
        {
            zombie.moveWithMap(t_move);
        }
    }

    void ZombieObjectManager::appendCollisions(std::vector<sf::FloatRect> & t_rects) const
    {
        for (const Zombie & zombie : m_zombies)
        {
            if (zombie.isAlive())
            {
                t_rects.push_back(zombie.collisionRect());
            }
        }
    }

    void ZombieObjectManager::draw(
        const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        for (const Zombie & zombie : m_zombies)
        {
            zombie.draw(t_context, t_target, t_states);
        }
    }

    bool ZombieObjectManager::doesAvatarCollideWithAnyAndDie(
        const Context & t_context, const sf::FloatRect & t_avatarRect)
    {
        for (Zombie & zombie : m_zombies)
        {
            if (zombie.doesAvatarCollideWithAnyAndDie(t_context, t_avatarRect))
            {
                return true;
            }
        }

        return false;
    }

    const Harm
        ZombieObjectManager::attack(const Context & t_context, const sf::FloatRect & t_attackRect)
    {
        Harm harm;

        for (Zombie & zombie : m_zombies)
        {
            if (zombie.isAlive() and zombie.collisionRect().findIntersection(t_attackRect))
            {
                harm.did_hit = true;
                zombie.hit(t_context);
                if (not zombie.isAlive())
                {
                    harm.did_kill = true;

                    ++t_context.stats.enemy_killed;
                    t_context.info_region.scoreAdjust(t_context.settings.kill_zombie_score);
                }
            }
        }

        return harm;
    }

} // namespace halloween
