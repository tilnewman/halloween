// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// frogs.cpp
//
#include "frogs.hpp"

#include "context.hpp"
#include "info-region.hpp"
#include "level-stats.hpp"
#include "settings.hpp"

namespace halloween
{
    FrogObjectManager::FrogObjectManager()
        : m_frogs{}
    {}

    void FrogObjectManager::setup(const Context &) {}

    void FrogObjectManager::add(
        const Context & t_context, const sf::FloatRect & t_region, const std::string &)
    {
        m_frogs.emplace_back(t_context, t_region);
    }

    void FrogObjectManager::update(const Context & t_context, const float t_frameTimeSec)
    {
        for (Frog & frog : m_frogs)
        {
            frog.update(t_context, t_frameTimeSec);
        }
    }

    void FrogObjectManager::moveWithMap(const sf::Vector2f & t_move)
    {
        for (Frog & frog : m_frogs)
        {
            frog.moveWithMap(t_move);
        }
    }

    bool FrogObjectManager::doesAvatarCollideWithAnyAndDie(const sf::FloatRect & t_avatarRect) const
    {
        for (const Frog & frog : m_frogs)
        {
            if (frog.doesAvatarCollideWithAnyAndDie(t_avatarRect))
            {
                return true;
            }
        }

        return false;
    }

    void FrogObjectManager::appendCollisions(std::vector<sf::FloatRect> & t_rects) const
    {
        for (const Frog & frog : m_frogs)
        {
            t_rects.push_back(frog.collisionRect());
        }
    }

    void FrogObjectManager::draw(
        const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        for (const Frog & frog : m_frogs)
        {
            frog.draw(t_context, t_target, t_states);
        }
    }

    const Harm
        FrogObjectManager::attack(const Context & t_context, const sf::FloatRect & t_attackRect)
    {
        Harm harm;

        for (Frog & frog : m_frogs)
        {
            if (frog.isAlive() and frog.collisionRect().findIntersection(t_attackRect))
            {
                harm.did_hit = true;
                frog.hit(t_context);

                if (not frog.isAlive())
                {
                    harm.did_kill = true;

                    ++t_context.stats.enemy_killed;
                    t_context.info_region.scoreAdjust(t_context.settings.kill_frog_score);
                }
            }
        }

        return harm;
    }

} // namespace halloween
