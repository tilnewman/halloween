#ifndef FROG_HPP_INLCUDED
#define FROG_HPP_INLCUDED
//
// frog.hpp
//
#include "sliders.hpp"

#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <vector>

namespace halloween
{

    struct Context;

    //

    enum class FrogAnim : unsigned char
    {
        Idle = 0,
        Hop,
        AttackTounge,
        AttackBite,
        Death,
        Hit,
        Dizzy,
        Eating,
        Roar,
        Count
    };

    [[nodiscard]] constexpr std::string_view toString(const FrogAnim t_anim) noexcept
    {
        // clang-format off
        switch(t_anim)
        {
            case FrogAnim::Idle:         { return "idle";          }
            case FrogAnim::Hop:          { return "hop";           }
            case FrogAnim::AttackTounge: { return "attack-tounge"; }
            case FrogAnim::AttackBite:   { return "attack-bite";   }
            case FrogAnim::Death:        { return "death";         }
            case FrogAnim::Hit:          { return "hit";           }
            case FrogAnim::Dizzy:        { return "dizzy";         }
            case FrogAnim::Eating:       { return "eating";        }
            case FrogAnim::Roar:         { return "roar";          }
            case FrogAnim::Count:    
            default:                  { return "unknown_froganim"; }
        }
        // clang-format on
    }

    //

    enum class FrogTask : unsigned char
    {
        Idle,
        Roar,
        Attack,
        Chase,
        Hit
    };

    [[nodiscard]] constexpr std::string_view toString(const FrogTask t_task) noexcept
    {
        // clang-format off
        switch (t_task)
        {
            case FrogTask::Idle:   { return "idle";   }
            case FrogTask::Roar:   { return "roar";   }
            case FrogTask::Chase:  { return "chase";  }
            case FrogTask::Hit:    { return "hit";  }
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
            case FrogAnim::AttackBite:   { return 0.06f; }
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
        Frog(
            const Context & t_context,
            const sf::FloatRect & t_rect,
            const std::vector<std::vector<sf::Texture>> & t_texturesVec);

        [[nodiscard]] const sf::FloatRect collisionRect() const;
        [[nodiscard]] const sf::FloatRect attackRect(const FrogAnim t_anim) const;
        void update(const Context & t_context, const float t_frameTimeSec);
        void moveWithMap(const sf::Vector2f & t_move);
        bool doesAvatarCollideWithAnyAndDie(const sf::FloatRect & t_avatarRect) const;
        [[nodiscard]] constexpr bool isAlive() const noexcept { return (m_hitPoints > 0); }
        void hit(const Context & t_context);

        void draw(const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states)
            const;

      private:
        void turn();

        void setupTask(
            const FrogTask t_task, const FrogAnim t_anim, const std::size_t t_animRepeatCount);

        bool turnToFace(const sf::Vector2f & t_position);
        [[nodiscard]] const std::vector<sf::Texture> & getTextures(const FrogAnim t_action) const;

      private:
        FrogTask m_task;
        FrogAnim m_anim;
        sf::Sprite m_sprite;
        float m_animElapsedSec;
        std::size_t m_frameIndex;
        float m_taskElapsedSec;
        sf::FloatRect m_rect;
        bool m_isFacingRight;
        std::size_t m_animRepeatCount;
        std::size_t m_hitPoints;
        bool m_hasFinishedDeathAnim;
        const std::vector<std::vector<sf::Texture>> & m_texturesVec;
    };

} // namespace halloween

#endif // FROG_HPP_INLCUDED
