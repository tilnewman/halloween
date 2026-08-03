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
#include <SFML/Graphics/Text.hpp>

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
            case ZombieAnim::Death:  { return 0.05f;  }
            case ZombieAnim::Hit:
            case ZombieAnim::Count:
            default:                 { return 0.01f; }
        }
        // clang-format on
    }

    //

    enum class ZombieTask
    {
        Stare,
        Wander,
        Chase,
        Attack,
        Hit
    };

    [[nodiscard]] constexpr std::string_view toString(const ZombieTask t_task) noexcept
    {
        // clang-format off
        switch(t_task)
        {
            case ZombieTask::Stare:  { return "Stare";  }
            case ZombieTask::Wander: { return "wander"; }
            case ZombieTask::Chase:  { return "chase";  }
            case ZombieTask::Attack: { return "attack"; }
            case ZombieTask::Hit:
            default:                 { return "hit";    }    
        }
        // clang-format on
    }

    //

    class Zombie
    {
      public:
        Zombie(const Context & t_context, const sf::FloatRect & t_rect);

        [[nodiscard]] const sf::FloatRect collisionRect() const;
        [[nodiscard]] const sf::FloatRect attackRect() const;
        void update(const Context & t_context, const float t_frameTimeSec);
        void moveWithMap(const sf::Vector2f & t_move);
        bool doesAvatarCollideWithAnyAndDie(const sf::FloatRect & t_avatarRect) const;
        [[nodiscard]] constexpr bool isAlive() const noexcept { return (m_hitPoints > 0); }
        void hit(const Context & t_context);

        void draw(const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states)
            const;

        [[nodiscard]] bool isBeingHit() const { return (ZombieAnim::Hit == m_anim); }

      private:
        void turn();
        void setupTask(const ZombieTask t_task, const ZombieAnim t_anim);
        void startWanderingOrStaring(const Context & t_context);
        void startChasing(const Context & t_context);
        [[nodiscard]] float calcTimePerFrame() const;

      private:
        ZombieAnim m_anim;
        ZombieTask m_task;
        sf::Sprite m_sprite;
        sf::FloatRect m_rect;
        bool m_isFacingRight;
        float m_animElpasedSec;
        float m_taskElapsedSec;
        float m_taskDurationSec;
        std::size_t m_frameIndex;
        float m_wanderTarget;
        float m_walkSpeed;
        std::size_t m_hitPoints;
        bool m_hasFinishedDeathAnim;

        mutable sf::Text m_debugText;
    };

} // namespace halloween

#endif // ZOMBIE_HPP_INCLUDED
