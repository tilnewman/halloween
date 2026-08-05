#ifndef AVATAR_HPP_INCLUDED
#define AVATAR_HPP_INCLUDED
//
// avatar.hpp
//
#include "avatar-anim.hpp"
#include "blood.hpp"
#include "moving-platforms.hpp"

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

        void resetForNewLevel(const sf::FloatRect & t_rect);
        void setup(const Settings & t_settings);
        void draw(sf::RenderTarget & t_target, sf::RenderStates t_states) const;
        void update(const Context & t_context, const float t_frameTimeSec);
        inline const sf::FloatRect bounds() const { return m_sprite.getGlobalBounds(); }
        void handleHitByBoss(const Context & t_context);
        constexpr bool isDead() const noexcept { return (Action::Dead == m_action); }
        const sf::FloatRect collisionRect() const;
        const sf::FloatRect attackCollisionRect() const;
        void moveWithPlatforms(const MoveIdVec_t & t_moveIDs);

      private:
        void moveMap(const Context & t_context);
        bool handleDeath(const Context & t_context, const float t_frameTimeSec);
        bool handleAttacking(const Context & t_context, const float t_frameTimeSec);
        bool handleThrowing(const Context & t_context, const float t_frameTimeSec);
        bool handleGliding(const Context & t_context, const float t_frameTimeSec);
        bool handleIdle(const Context & t_context, const float t_frameTimeSec);
        void sideToSideMotion(const Context & t_context, const float t_frameTimeSec);
        void jumping(const Context & t_context, const float t_frameTimeSec);
        void collisions(const Context & t_context);
        void killCollisions(const Context & t_context);
        void exitCollisions(const Context & t_context) const;
        void handleAttackingEnemies(const Context & t_context);
        void acidCollisions(const Context & t_context);
        void waterCollisions(const Context & t_context);
        void preventBacktracking(const Context & t_context);
        void killIfOutOfBounds(const Context & t_context);
        void triggerDeath(const Context & t_context);
        void handleBossCollisions(const Context & t_context);
        void bounceAwayFromBoss(const Context & t_context);

        void collide(
            const Context & t_context,
            const sf::FloatRect & t_intersectionRect,
            const sf::Vector2f & t_avatarCenter,
            bool & t_detectLanding);

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
        bool m_hasAttackedAlready;
        std::vector<sf::FloatRect> m_collisionRectCache;
        RectIdVec_t m_platformCollisionRectCache;
        std::size_t m_platformLandedOnId;
    };

} // namespace halloween

#endif // AVATAR_HPP_INCLUDED
