#ifndef FLY_HPP_INCLUDED
#define FLY_HPP_INCLUDED
//
// fly.hpp
//
#include "fly-textures.hpp"
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

    class Fly
    {
      public:
        Fly(const Context & t_context, const sf::FloatRect & t_rect);

        void update(const Context & t_context, const float m_frameTimeSec);
        void moveWithMap(const sf::Vector2f & t_move);

        [[nodiscard]] bool doesAvatarCollideWithAnyAndDie(
            const Context & t_context, const sf::FloatRect & t_avatarRect);

        [[nodiscard]] bool isAlive() const noexcept { return (m_hitPoints > 0); }
        [[nodiscard]] const sf::FloatRect collisionRect() const;

        void draw(const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states)
            const;

        const Harm hit(const Context & t_context);

      private:
        void turn();
        void setupTask(const FlyTask t_task, const FlyAnim t_anim);
        void turnToFacePlayer(const Context & t_context);

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
    };
} // namespace halloween

#endif // FLY_HPP_INCLUDED
