// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// flys.cpp
//
#include "flys.hpp"

#include "context.hpp"
#include "info-region.hpp"
#include "level-stats.hpp"
#include "settings.hpp"

namespace halloween
{

    FlyObjectManager::FlyObjectManager()
        : m_flys{}
    {}

    void FlyObjectManager::add(
        const Context & t_context, const sf::FloatRect & t_region, const std::string &)
    {
        m_flys.emplace_back(t_context, t_region, m_textureManager);
    }

    void FlyObjectManager::update(const Context & t_context, const float t_frameTimeSec)
    {
        for (Fly & fly : m_flys)
        {
            fly.update(t_context, t_frameTimeSec);
        }
    }

    void FlyObjectManager::moveWithMap(const sf::Vector2f & t_move)
    {
        for (Fly & fly : m_flys)
        {
            fly.moveWithMap(t_move);
        }
    }

    bool FlyObjectManager::doesAvatarCollideWithAnyAndDie(
        const Context & t_context, const sf::FloatRect & t_avatarRect) 
    {
        for (Fly & fly : m_flys)
        {
            if (fly.doesAvatarCollideWithAnyAndDie(t_context, t_avatarRect))
            {
                return true;
            }
        }

        return false;
    }

    void FlyObjectManager::appendCollisions(std::vector<sf::FloatRect> & t_rects) const
    {
        for (const Fly & fly : m_flys)
        {
            if (fly.isAlive())
            {
                t_rects.push_back(fly.collisionRect());
            }
        }
    }

    void FlyObjectManager::draw(
        const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        for (const Fly & fly : m_flys)
        {
            fly.draw(t_context, t_target, t_states);
        }
    }

    const Harm
        FlyObjectManager::attack(const Context & t_context, const sf::FloatRect & t_attackRect)
    {
        Harm harm;

        for (Fly & fly : m_flys)
        {
            if (fly.isAlive() and fly.collisionRect().findIntersection(t_attackRect))
            {
                harm.did_hit = true;
                fly.hit(t_context);

                if (not fly.isAlive())
                {
                    harm.did_kill = true;

                    ++t_context.stats.enemy_killed;
                    t_context.info_region.scoreAdjust(t_context.settings.kill_fly_score);
                }
            }
        }

        return harm;
    }

} // namespace halloween
