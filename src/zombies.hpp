#ifndef ZOMBIES_HPP_INCLUDED
#define ZOMBIES_HPP_INCLUDED
//
// zombies.hpp
//
#include "object-manager.hpp"
#include "zombie.hpp"

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include <vector>

namespace sf
{
    class RenderTarget;
}

namespace halloween
{

    struct Context;

    class ZombieObjectManager final : public IObjectManager
    {
      public:
        ZombieObjectManager();
        ~ZombieObjectManager() final = default;

        void setup(const Context & t_context) final;
        void teardown() final { m_texturesVec.clear(); }
        bool willDrawBeforeMap() const final { return false; }
        void clear() final { m_zombies.clear(); }

        void
            add(const Context & t_context,
                const sf::FloatRect & t_region,
                const std::string & t_details = "") final;

        const Harm attack(const Context & t_context, const sf::FloatRect & t_attackRect);
        void update(const Context & t_context, const float t_frameTimeSec) final;
        void moveWithMap(const sf::Vector2f & t_move) final;
        void collideWithAvatar(const Context &, const sf::FloatRect &) final {}

        bool doesAvatarCollideWithAnyAndDie(
            const Context & t_context, const sf::FloatRect & t_avatarRect) final;

        void appendCollisions(std::vector<sf::FloatRect> & t_rects) const final;

        void draw(const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states)
            const final;

      private:
        std::vector<Zombie> m_zombies;
        std::vector<std::vector<sf::Texture>> m_texturesVec;
    };

} // namespace halloween

#endif // ZOMBIES_HPP_INCLUDED
