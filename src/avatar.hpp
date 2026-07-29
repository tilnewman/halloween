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

        void setup(const Settings & t_settings);
        void draw(sf::RenderTarget & t_target, sf::RenderStates t_states) const;
        void setSpawnPosForNewLevel(const sf::FloatRect & t_rect);
        void update(Context & t_context, const float t_frameTimeSec);
        inline const sf::FloatRect bounds() const { return m_sprite.getGlobalBounds(); }
        void handleHitByBoss(Context & t_context);
        constexpr bool isDead() const noexcept { return (Action::Dead == m_action); }

      private:
        const sf::FloatRect collisionRect() const;
        const sf::FloatRect attackCollisionRect() const;
        void moveMap(Context & t_context);
        bool handleDeath(Context & t_context, const float t_frameTimeSec);
        bool handleAttacking(Context & t_context, const float t_frameTimeSec);
        bool handleThrowing(Context & t_context, const float t_frameTimeSec);
        bool handleGliding(Context & t_context, const float t_frameTimeSec);
        bool handleIdle(Context & t_context, const float t_frameTimeSec);
        void sideToSideMotion(Context & t_context, const float t_frameTimeSec);
        void jumping(Context & t_context, const float t_frameTimeSec);
        void collisions(Context & t_context);
        void killCollisions(Context & t_context);
        void exitCollisions(Context & t_context) const;
        void handleAttackingEnemies(Context & t_context);
        void acidCollisions(Context & t_context);
        void waterCollisions(Context & t_context);
        void preventBacktracking(const Context & t_context);
        void killIfOutOfBounds(Context & t_context);
        void triggerDeath(Context & t_context);
        void handleBossCollisions(Context & t_context);
        void bounceAwayFromBoss(const Context & t_context);

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
        float m_timeSinceLastThrowSec;
    };

} // namespace halloween

#endif // AVATAR_HPP_INCLUDED
