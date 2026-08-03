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

    enum class FrogTask
    {
        Idle,
        Attack
    };

    [[nodiscard]] constexpr std::string_view toString(const FrogTask t_task) noexcept
    {
        // clang-format off
        switch (t_task)
        {
            case FrogTask::Idle:   { return "idle"; }
            case FrogTask::Attack:
            default:             { return "attack"; }
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

      private:
        FrogTask m_task;
        FrogAnim m_anim;
        sf::Sprite m_sprite;
        float m_animElapsedSec;
        std::size_t m_frameIndex;
        float m_taskElapsedSec;
        sf::FloatRect m_rect;
        bool m_isFacingRight;

        mutable sf::Text m_debugText;
    };

} // namespace halloween

#endif // FROG_HPP_INLCUDED
