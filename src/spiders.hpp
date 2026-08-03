#ifndef SPIDERS_HPP_INCLUDED
#define SPIDERS_HPP_INCLUDED
//
// spiders.hpp
//
#include "spider-textures.hpp"
// #include "spider.hpp"
#include "object-manager.hpp"

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include <vector>

namespace halloween
{

    struct Context;

    //

    class Spider
    {
      public:
        Spider(const Context &, const sf::FloatRect &) {}

        [[nodiscard]] const sf::FloatRect collisionRect() const { return {}; }
        [[nodiscard]] const sf::FloatRect attackRect(const SpiderAnim) const { return {}; }
        void update(const Context &, const float) {}
        void moveWithMap(const sf::Vector2f &) {}
        bool doesAvatarCollideWithAnyAndDie(const sf::FloatRect &) const { return false; }
        [[nodiscard]] constexpr bool isAlive() const noexcept { return (m_hitPoints > 0); }
        void hit(const Context &) {}
        void draw(const Context &, sf::RenderTarget &, sf::RenderStates) const {}

      private:
        std::size_t m_hitPoints;
    };

    //

    class SpiderObjectManager final : public IObjectManager
    {
      public:
        SpiderObjectManager();
        ~SpiderObjectManager() override = default;

        bool willDrawBeforeMap() const final { return false; }
        void clear() final { m_spiders.clear(); }
        void setup(const Context & t_context) final;

        void
            add(const Context & t_context,
                const sf::FloatRect & t_region,
                const std::string & t_details = "") final;

        void update(const Context & t_context, const float t_frameTimeSec) final;
        void moveWithMap(const sf::Vector2f & t_move) final;
        void collideWithAvatar(const Context &, const sf::FloatRect &) final {}
        bool doesAvatarCollideWithAnyAndDie(const sf::FloatRect & t_avatarRect) const final;
        void appendCollisions(std::vector<sf::FloatRect> & t_rects) const final;
        const Harm attack(const Context & t_context, const sf::FloatRect & t_attackRect);

        void draw(const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states)
            const final;

      private:
        std::vector<Spider> m_spiders;
    };

} // namespace halloween

#endif // SPIDERS_HPP_INCLUDED
