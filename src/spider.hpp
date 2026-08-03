#ifndef SPIDER_HPP_INCLUDED
#define SPIDER_HPP_INCLUDED
//
// spider.hpp
//
#include "spider-textures.hpp"

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
        Spider(const Context & t_context, const sf::FloatRect & t_rect);

        [[nodiscard]] const sf::FloatRect collisionRect() const;
        [[nodiscard]] const sf::FloatRect attackRect(const SpiderAnim) const;
        void update(const Context & t_context, const float t_frameTimeSec);
        void moveWithMap(const sf::Vector2f & t_move);
        bool doesAvatarCollideWithAnyAndDie(const sf::FloatRect & t_avatarRect) const;
        [[nodiscard]] constexpr bool isAlive() const noexcept { return (m_hitPoints > 0); }
        void hit(const Context & t_context);

        void draw(const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states)
            const;

      private:
        SpiderAnim m_anim;
        SpiderType m_type;
        sf::Sprite m_webSprite;
        sf::Sprite m_spiderSprite;
        float m_animElapsedTimeSec;
        std::size_t m_frameIndex;
        std::size_t m_hitPoints;
        sf::FloatRect m_rect;
    };

} // namespace halloween

#endif // SPIDER_HPP_INCLUDED
