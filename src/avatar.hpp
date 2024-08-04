#ifndef AVATAR_HPP_INCLUDED
#define AVATAR_HPP_INCLUDED
//
// avatar.hpp
//
#include "avatar-anim.hpp"
#include "blood.hpp"

#include <vector>

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace halloween
{
    struct Settings;
    struct Context;

    enum class Action
    {
        Idle,
        Attack,
        Run,
        Jump,
        Throw,
        Glide,
        Dead
    };

    class Avatar
    {
      public:
        Avatar();

        void setup(const Settings & settings);
        void draw(sf::RenderTarget & target, sf::RenderStates states) const;
        const sf::FloatRect collisionRect() const;
        const sf::FloatRect attackCollisionRect() const;
        void setPosition(const sf::FloatRect & rect);
        void update(Context & context, const float frameTimeSec);

      private:
        void moveMap(Context & context);
        bool handleDeath(Context & context, const float frameTimeSec);
        bool handleAttacking(Context & context, const float frameTimeSec);
        bool handleThrowing(Context & context, const float frameTimeSec);
        bool handleGliding(Context & context, const float frameTimeSec);
        bool handleIdle(Context & context, const float frameTimeSec);
        void sideToSideMotion(Context & context, const float frameTimeSec);
        void jumping(Context & context, const float frameTimeSec);
        void collisions(Context & context);
        void killCollisions(Context & context);
        void exitCollisions(Context & context) const;
        void coinCollisions(Context & context) const;
        void slimeCollisions(Context & context, const bool isAttacking);
        void acidCollisions(Context & context);
        void waterCollisions(Context & context);
        void preventBacktracking(const Context & context);
        void killIfOutOfBounds(Context & context);
        void triggerDeath(Context & context);

      private:
        Blood m_blood;
        AvatarAnim m_runAnim;
        AvatarAnim m_attackAnim;
        AvatarAnim m_deathAnim;
        AvatarAnim m_throwAnim;
        AvatarAnim m_glideAnim;
        AvatarAnim m_idleAnim;
        sf::Texture m_jumpTexture;
        sf::Sprite m_sprite;
        sf::Vector2f m_velocity;
        Action m_action;
        bool m_hasLanded;
        bool m_isFacingRight;
        float m_deadDelaySec;
        bool m_willDie;
    };

} // namespace halloween

#endif // AVATAR_HPP_INCLUDED
