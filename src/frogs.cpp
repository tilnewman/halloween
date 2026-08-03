// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// frogs.cpp
//
#include "frogs.hpp"

#include "context.hpp"
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

    void FrogObjectManager::collideWithAvatar(
        const Context & t_context, const sf::FloatRect & t_avatarRect)
    {
        for (Frog & frog : m_frogs)
        {
            frog.collideWithAvatar(t_context, t_avatarRect);
        }
    }

    bool FrogObjectManager::doesAvatarCollideWithAnyAndDie(const sf::FloatRect &) const
    {
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

} // namespace halloween
