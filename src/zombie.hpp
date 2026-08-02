#ifndef ZOMBIE_HPP_INCLUDED
#define ZOMBIE_HPP_INCLUDED
//
// zombie.hpp
//
#include "zombie-textures.hpp"

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>

namespace halloween
{

    struct Context;

    //

    class Zombie
    {
      public:
        Zombie(const Context & t_context, const sf::FloatRect & t_rect);

        const sf::FloatRect collisionRect() const;
        void update(const Context & t_context, const float t_frameTimeSec);
        void moveWithMap(const sf::Vector2f & t_move);

        void draw(const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states)
            const;

      private:
        sf::Sprite m_sprite;
        ZombieAnim m_anim;
        sf::FloatRect m_rect;
        bool m_isFacingRight;
        float m_animElpasedTimeSec;
        std::size_t m_frameIndex;
    };

} // namespace halloween

#endif // ZOMBIE_HPP_INCLUDED
