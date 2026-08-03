#ifndef FROG_HPP_INLCUDED
#define FROG_HPP_INLCUDED
//
// frog.hpp
//
#include "frog-textures.hpp"

#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

namespace halloween
{

    struct Context;

    //

    enum class FrogTask : unsigned char
    {
        Idle,
        Roar,
        Attack,
        Chase
    };

    [[nodiscard]] constexpr std::string_view toString(const FrogTask t_task) noexcept
    {
        // clang-format off
        switch (t_task)
        {
            case FrogTask::Idle:   { return "idle";   }
            case FrogTask::Roar:   { return "roar";   }
            case FrogTask::Chase:  { return "chase";  }
            case FrogTask::Attack:
            default:               { return "attack"; }
        }
        // clang-format on
    }

    //

    [[nodiscard]] constexpr float timePerFrame(const FrogAnim t_anim) noexcept
    {
        // clang-format off
        switch(t_anim)
        {
            case FrogAnim::Hop:          { return 0.06f;  }
            case FrogAnim::AttackTounge: { return 0.06f;  }
            case FrogAnim::AttackBite:   { return 0.065f; }
            case FrogAnim::Death:        { return 0.085f; }
            case FrogAnim::Hit:          { return 0.1f;   }
            case FrogAnim::Roar:         { return 0.045f;  }
            case FrogAnim::Dizzy:
            case FrogAnim::Eating:
            case FrogAnim::Idle:
            case FrogAnim::Count:
            default:                     { return 0.07f;  }
        }
        // clang-format on
    }

    //

    class Frog
    {
      public:
        Frog(const Context & t_context, const sf::FloatRect & t_rect);

        [[nodiscard]] const sf::FloatRect collisionRect() const;
        [[nodiscard]] const sf::FloatRect attackRect() const;

        void update(const Context & t_context, const float t_frameTimeSec);
        void moveWithMap(const sf::Vector2f & t_move);
        void collideWithAvatar(const Context & t_context, const sf::FloatRect & t_avatarRect);

        void draw(const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states)
            const;

      private:
        void turn();
        void setupTask(const FrogTask t_task, const FrogAnim t_anim);
        void turnToFace(const sf::Vector2f & t_position);

      private:
        FrogTask m_task;
        FrogAnim m_anim;
        sf::Sprite m_sprite;
        float m_animElapsedSec;
        std::size_t m_frameIndex;
        float m_taskElapsedSec;
        sf::FloatRect m_rect;
        bool m_isFacingRight;
        std::size_t m_taskRepeatAnimCount;

        mutable sf::Text m_debugText;
    };

} // namespace halloween

#endif // FROG_HPP_INLCUDED
