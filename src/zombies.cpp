// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// zombies.cpp
//
#include "zombies.hpp"

#include "context.hpp"

namespace halloween
{

    ZombieObjectManager::ZombieObjectManager()
        : m_zombies{}
    {}

    void ZombieObjectManager::setup(const Context &) {}

    void ZombieObjectManager::add(const Context &, const sf::FloatRect &, const std::string &)
    {
        m_zombies.emplace_back();
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
            t_rects.push_back(zombie.collisionRect());
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

} // namespace halloween
