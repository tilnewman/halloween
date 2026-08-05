// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// object-manager.cpp
//
#include "object-manager.hpp"

#include "context.hpp"
#include "settings.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

namespace halloween
{

    ObjectManagerList::ObjectManagerList()
        : m_managers{}
    {
        // probably no more than a dozen in one map
        m_managers.reserve(100);
    }

    void ObjectManagerList::add(IObjectManager & t_manager) { m_managers.push_back(t_manager); }

    void ObjectManagerList::clear()
    {
        for (auto & manager : m_managers)
        {
            manager.get().clear();
        }
    }

    void ObjectManagerList::setup(const Context & t_context)
    {
        for (auto & manager : m_managers)
        {
            manager.get().setup(t_context);
        }
    }

    void ObjectManagerList::teardown()
    {
        for (auto & manager : m_managers)
        {
            manager.get().teardown();
        }
    }

    void ObjectManagerList::update(const Context & t_context, const float t_frameTimeSec)
    {
        for (auto & manager : m_managers)
        {
            manager.get().update(t_context, t_frameTimeSec);
        }
    }

    void ObjectManagerList::drawBeforeMap(
        const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        for (auto & manager : m_managers)
        {
            if (manager.get().willDrawBeforeMap())
            {
                manager.get().draw(t_context, t_target, t_states);
            }
        }
    }

    void ObjectManagerList::drawAfterMap(
        const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        for (auto & manager : m_managers)
        {
            if (!manager.get().willDrawBeforeMap())
            {
                manager.get().draw(t_context, t_target, t_states);
            }
        }
    }

    void ObjectManagerList::moveWithMap(const sf::Vector2f & t_move)
    {
        for (auto & manager : m_managers)
        {
            manager.get().moveWithMap(t_move);
        }
    }

    void ObjectManagerList::collideWithAvatar(
        const Context & t_context, const sf::FloatRect & t_avatarRect)
    {
        for (auto & manager : m_managers)
        {
            manager.get().collideWithAvatar(t_context, t_avatarRect);
        }
    }

    bool ObjectManagerList::doesAvatarCollideWithAnyAndDie(
        const Context & t_context, const sf::FloatRect & t_avatarRect)
    {
        bool didAnyCollideAndCauseDeath{ false };
        for (auto & manager : m_managers)
        {
            if (manager.get().doesAvatarCollideWithAnyAndDie(t_context, t_avatarRect))
            {
                didAnyCollideAndCauseDeath = true;
            }
        }

        return didAnyCollideAndCauseDeath;
    }

    void ObjectManagerList::appendCollisions(std::vector<sf::FloatRect> & t_rects) const
    {
        for (auto & manager : m_managers)
        {
            manager.get().appendCollisions(t_rects);
        }
    }

} // namespace halloween
