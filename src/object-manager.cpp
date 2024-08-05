// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// object-manager.cpp
//
#include "object-manager.hpp"

#include "context.hpp"
#include "settings.hpp"

namespace halloween
{

    ObjectManagerList::ObjectManagerList()
        : m_managers()
    {
        // probably no more than a dozen in one map
        m_managers.reserve(100);
    }

    void ObjectManagerList::add(IObjectManager & manager) { m_managers.push_back(manager); }

    void ObjectManagerList::clearAll()
    {
        for (auto & manager : m_managers)
        {
            manager.get().clear();
        }
    }

    void ObjectManagerList::setupAll(const Settings & settings)
    {
        for (auto & manager : m_managers)
        {
            manager.get().setup(settings);
        }
    }

    void ObjectManagerList::updateAll(Context & context, const float frameTimeSec)
    {
        for (auto & manager : m_managers)
        {
            manager.get().update(context, frameTimeSec);
        }
    }

    void ObjectManagerList::drawAllBeforeMap(
        const Context & context, sf::RenderTarget & target, sf::RenderStates states) const
    {
        for (auto & manager : m_managers)
        {
            if (manager.get().willDrawBeforeMap())
            {
                manager.get().draw(context, target, states);
            }
        }
    }

    void ObjectManagerList::drawAllAfterMap(
        const Context & context, sf::RenderTarget & target, sf::RenderStates states) const
    {
        for (auto & manager : m_managers)
        {
            if (!manager.get().willDrawBeforeMap())
            {
                manager.get().draw(context, target, states);
            }
        }
    }

    void ObjectManagerList::moveAllWithMap(const sf::Vector2f & move)
    {
        for (auto & manager : m_managers)
        {
            manager.get().moveWithMap(move);
        }
    }

    void
        ObjectManagerList::collideAllWithAvatar(Context & context, const sf::FloatRect & avatarRect)
    {
        for (auto & manager : m_managers)
        {
            manager.get().collideWithAvatar(context, avatarRect);
        }
    }

    bool ObjectManagerList::doesAvatarCollideWithAnyAndDie(const sf::FloatRect & avatarRect) const
    {
        bool didAnyCollideAndCauseDeath = false;
        for (auto & manager : m_managers)
        {
            if (manager.get().doesAvatarCollideWithAnyAndDie(avatarRect))
            {
                didAnyCollideAndCauseDeath = true;
            }
        }

        return didAnyCollideAndCauseDeath;
    }

    void ObjectManagerList::appendAllCollisions(std::vector<sf::FloatRect> & rects) const
    {
        for (auto & manager : m_managers)
        {
            manager.get().appendCollisions(rects);
        }
    }

} // namespace halloween
