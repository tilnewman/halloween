// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// spiders.cpp
//
#include "spiders.hpp"

#include "context.hpp"
#include "info-region.hpp"
#include "level-stats.hpp"
#include "settings.hpp"

namespace halloween
{

    SpiderObjectManager::SpiderObjectManager()
        : m_spiders{}
    {}

    void SpiderObjectManager::setup(const Context &) {}

    void SpiderObjectManager::add(
        const Context & t_context, const sf::FloatRect & t_region, const std::string &)
    {
        m_spiders.emplace_back(t_context, t_region);
    }

    void SpiderObjectManager::update(const Context & t_context, const float t_frameTimeSec)
    {
        for (Spider & spider : m_spiders)
        {
            spider.update(t_context, t_frameTimeSec);
        }
    }

    void SpiderObjectManager::moveWithMap(const sf::Vector2f & t_move)
    {
        for (Spider & spider : m_spiders)
        {
            spider.moveWithMap(t_move);
        }
    }

    bool SpiderObjectManager::doesAvatarCollideWithAnyAndDie(const sf::FloatRect & t_avatarRect) const
    {
        for (const Spider & spider : m_spiders)
        {
            if (spider.doesAvatarCollideWithAnyAndDie(t_avatarRect))
            {
                return true;
            }
        }

        return false;
    }

    void SpiderObjectManager::appendCollisions(std::vector<sf::FloatRect> & t_rects) const
    {
        for (const Spider & spider : m_spiders)
        {
            t_rects.push_back(spider.collisionRect());
        }
    }

    void SpiderObjectManager::draw(
        const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        for (const Spider & spider : m_spiders)
        {
            spider.draw(t_context, t_target, t_states);
        }
    }

    const Harm
        SpiderObjectManager::attack(const Context & t_context, const sf::FloatRect & t_attackRect)
    {
        Harm harm;

        for (Spider & spider : m_spiders)
        {
            if (spider.isAlive() and spider.collisionRect().findIntersection(t_attackRect))
            {
                harm.did_hit = true;
                spider.hit(t_context);

                if (not spider.isAlive())
                {
                    harm.did_kill = true;

                    ++t_context.stats.enemy_killed;
                    t_context.info_region.scoreAdjust(t_context.settings.kill_spider_score);
                }
            }
        }

        return harm;
    }

} // namespace halloween
