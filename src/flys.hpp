#ifndef FLYS_HPP_INCLUDED
#define FLYS_HPP_INCLUDED
//
// flys.hpp
//
#include "fly-textures.hpp"
#include "object-manager.hpp"
// #include "fly.hpp"

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include <vector>

namespace halloween
{

    struct Context;

    //

    class Fly
    {
      public:
        Fly(const Context &, const sf::FloatRect &) {}
        void update(const Context &, const float) {}
        void moveWithMap(const sf::Vector2f &) {}

        bool doesAvatarCollideWithAnyAndDie(const Context &, const sf::FloatRect &)
        {
            return false;
        }

        bool isAlive() const { return false; }
        const sf::FloatRect collisionRect() const { return {}; }
        void draw(const Context &, sf::RenderTarget &, sf::RenderStates) const {}
        const Harm hit(const Context &) { return {}; }
    };

    //

    class FlyObjectManager final : public IObjectManager
    {
      public:
        FlyObjectManager();
        ~FlyObjectManager() override = default;

        bool willDrawBeforeMap() const final { return false; }
        void clear() final { m_flys.clear(); }
        void setup(const Context & t_context) final;

        void
            add(const Context & t_context,
                const sf::FloatRect & t_region,
                const std::string & t_details = "") final;

        void update(const Context & t_context, const float t_frameTimeSec) final;
        void moveWithMap(const sf::Vector2f & t_move) final;
        void collideWithAvatar(const Context &, const sf::FloatRect &) final {}

        bool doesAvatarCollideWithAnyAndDie(
            const Context & t_context, const sf::FloatRect & t_avatarRect) final;

        void appendCollisions(std::vector<sf::FloatRect> & t_rects) const final;
        const Harm attack(const Context & t_context, const sf::FloatRect & t_attackRect);

        void draw(const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states)
            const final;

      private:
        std::vector<Fly> m_flys;
    };

} // namespace halloween

#endif // FLYS_HPP_INCLUDED
