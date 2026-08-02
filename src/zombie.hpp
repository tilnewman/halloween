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

    [[nodiscard]] constexpr float timePerFrame(const ZombieAnim t_action) noexcept
    {
        // clang-format off
        switch(t_action)
        {
            case ZombieAnim::Idle:   { return 0.09f;  }
            case ZombieAnim::Walk:   { return 0.05f;  }
            case ZombieAnim::Attack: { return 0.03f;  }
            case ZombieAnim::Die:    { return 0.05f;  }
            case ZombieAnim::Hit:      
            case ZombieAnim::Count:    
            default:                 { return 0.025f; }
        }
        // clang-format on
    }

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
