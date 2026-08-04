#ifndef FLY_HPP_INCLUDED
#define FLY_HPP_INCLUDED
//
// fly.hpp
//
#include "object-manager.hpp"

#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>

namespace halloween
{

    struct Context;

    //

    enum class FlyTask : unsigned char
    {
        Wander,
        Chase,
        Death
    };

    //

    enum class FlyType : unsigned char
    {
        Beholder = 0,
        Chomp,
        Face,
        Horn,
        Peek,
        Count
    };

    [[nodiscard]] constexpr std::string_view toString(const FlyType t_type) noexcept
    {
        // clang-format off
        switch(t_type)
        {
            case FlyType::Beholder: { return "beholder"; }
            case FlyType::Chomp:    { return "chomp";    }
            case FlyType::Face:     { return "face";     }
            case FlyType::Horn:     { return "horn";     }
            case FlyType::Peek:     { return "Peek";     }
            case FlyType::Count:    
            default:         { return "unknown_flytype"; }
        }
        // clang-format on
    }

    //

    enum class FlyAnim : unsigned char
    {
        Fly = 0,
        Hit,
        Death,
        Count
    };

    [[nodiscard]] constexpr std::string_view toString(const FlyAnim t_anim) noexcept
    {
        // clang-format off
        switch(t_anim)
        {
            case FlyAnim::Fly:   { return "fly";   }
            case FlyAnim::Hit:   { return "hit";   }
            case FlyAnim::Death: { return "death"; }
            case FlyAnim::Count:    
            default:   { return "unknown_flyanim"; }
        }
        // clang-format on
    }

    //

    class Fly
    {
      public:
        Fly(const Context & t_context,
            const sf::FloatRect & t_rect,
            const std::vector<std::vector<std::vector<sf::Texture>>> & t_texturesVecVec);

        void update(const Context & t_context, const float m_frameTimeSec);
        void moveWithMap(const sf::Vector2f & t_move);
        [[nodiscard]] bool isAlive() const noexcept { return (m_hitPoints > 0); }
        [[nodiscard]] const sf::FloatRect collisionRect() const;
        void hit(const Context & t_context);

        [[nodiscard]] bool doesAvatarCollideWithAnyAndDie(
            const Context & t_context, const sf::FloatRect & t_avatarRect);

        void draw(const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states)
            const;

      private:
        void turn();
        void setupTask(const FlyTask t_task, const FlyAnim t_anim);
        void turnToFacePlayer(const Context & t_context);

        [[nodiscard]] const std::vector<sf::Texture> &
            getTextures(const FlyType t_type, const FlyAnim t_action) const;

      private:
        FlyType m_type;
        FlyAnim m_anim;
        FlyTask m_task;
        sf::Sprite m_sprite;
        float m_animElapsedSec;
        sf::FloatRect m_rect;
        std::size_t m_frameIndex;
        std::size_t m_hitPoints;
        bool m_isFacingRight;
        bool m_hasDeathAnimFinished;
        const std::vector<std::vector<std::vector<sf::Texture>>> & m_texturesVecVec;
    };
} // namespace halloween

#endif // FLY_HPP_INCLUDED
