// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "avatar.hpp"

#include "bat.hpp"
#include "check-macros.hpp"
#include "coin.hpp"
#include "context.hpp"
#include "dart.hpp"
#include "fire-spout.hpp"
#include "ghost.hpp"
#include "info-region.hpp"
#include "level-stats.hpp"
#include "level.hpp"
#include "missile.hpp"
#include "mushroom-boss.hpp"
#include "saw.hpp"
#include "screen-regions.hpp"
#include "settings.hpp"
#include "sfml-defaults.hpp"
#include "sfml-util.hpp"
#include "slime.hpp"
#include "sound-player.hpp"
#include "spiked-ball.hpp"
#include "state-machine.hpp"
#include "states.hpp"
#include "texture-loader.hpp"

#include <cassert>

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Window/Keyboard.hpp>

namespace halloween
{

    Avatar::Avatar()
        : m_blood{}
        , m_runAnim{}
        , m_attackAnim{}
        , m_deathAnim{}
        , m_throwAnim{}
        , m_glideAnim{}
        , m_idleAnim{}
        , m_jumpTexture{}
        , m_sprite{ util::SfmlDefaults::instance().texture() }
        , m_velocity{}
        , m_action{ Action::Idle }
        , m_hasLanded{ false }
        , m_isFacingRight{ true }
        , m_deadDelaySec{ 0.0f }
        , m_willDie{ false }
        , m_timeSinceLastThrowSec{ 0.0f }
        , m_collisionRectCache{}
    {
        m_collisionRectCache.reserve(1'000);
    }

    void Avatar::resetForNewLevel()
    {
        m_action = Action::Idle;
        m_idleAnim.restart();
        m_sprite.setTexture(m_idleAnim.texture(), true);
        m_hasLanded = false;
        m_isFacingRight = true;
        m_timeSinceLastThrowSec = 0.0f;
    }

    void Avatar::setup(const Settings & t_settings)
    {
        m_blood.setup(t_settings);

        const auto avatarPath{ t_settings.media_path / "image" / "avatar" };

        m_runAnim.setup(avatarPath, "Run", 10, 0.045f, true);
        m_attackAnim.setup(avatarPath, "Attack", 10, 0.03f, false);
        m_deathAnim.setup(avatarPath, "Dead", 10, 0.05f, false);
        m_throwAnim.setup(avatarPath, "Throw", 10, 0.02f, false);
        m_glideAnim.setup(avatarPath, "Glide", 10, 0.33f, true);
        m_idleAnim.setup(avatarPath, "Idle", 10, 0.1f, true);

        util::TextureLoader::load(m_jumpTexture, (avatarPath / "Jump-6.png"));

        m_idleAnim.restart();
        m_sprite.setTexture(m_idleAnim.texture(), true);
        m_sprite.setScale({ t_settings.avatar_scale, t_settings.avatar_scale });
    }

    void Avatar::draw(sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        t_target.draw(m_sprite, t_states);
        m_blood.draw(t_target, t_states);
    }

    const sf::FloatRect Avatar::collisionRect() const
    {
        // Avatar images for the various actions have different sizes and amounts of empty space
        // on the sides.  I know...free resources online suck.  So this function tweaks the
        // collison rect to compensate for that empty space on all four sides.  For example, the
        // attack sprite includes the sword swinging, but we don't want that sword colliding with
        // the walls and floors.

        sf::FloatRect rect{ m_sprite.getGlobalBounds() };

        if ((Action::Idle == m_action) || (Action::Attack == m_action))
        {
            const float hairVertAdj{ rect.size.y * 0.15f };
            rect.position.y += hairVertAdj;
            rect.size.y -= hairVertAdj;

            rect.size.x *= 0.7f;

            if (m_isFacingRight)
            {
                rect.position.x += (m_sprite.getGlobalBounds().size.x * 0.2f);
            }
            else
            {
                rect.position.x += (m_sprite.getGlobalBounds().size.x * 0.1f);
            }
        }
        else if (Action::Run == m_action)
        {
            const float hairVertAdj{ rect.size.y * 0.18f };
            rect.position.y += hairVertAdj;
            rect.size.y -= hairVertAdj;

            rect.size.x *= 0.5f;

            if (m_isFacingRight)
            {
                rect.position.x += (m_sprite.getGlobalBounds().size.x * 0.2f);
            }
            else
            {
                rect.position.x += (m_sprite.getGlobalBounds().size.x * 0.3f);
            }
        }
        else if (Action::Jump == m_action)
        {
            const float hairVertAdj{ rect.size.y * 0.18f };
            rect.position.y += hairVertAdj;
            rect.size.y -= (1.5f * hairVertAdj);

            rect.size.x *= 0.5f;

            if (m_isFacingRight)
            {
                rect.position.x += (m_sprite.getGlobalBounds().size.x * 0.2f);
            }
            else
            {
                rect.position.x += (m_sprite.getGlobalBounds().size.x * 0.4f);
            }
        }
        else if (Action::Throw == m_action)
        {
            const float hairVertAdj{ rect.size.y * 0.18f };
            rect.position.y += hairVertAdj;
            rect.size.y -= hairVertAdj;

            rect.size.x *= 0.5f;

            if (m_isFacingRight)
            {
                rect.position.x += (m_sprite.getGlobalBounds().size.x * 0.17f);
            }
            else
            {
                rect.position.x += (m_sprite.getGlobalBounds().size.x * 0.33f);
            }
        }
        else if (Action::Glide == m_action)
        {
            const float hairVertAdj{ rect.size.y * 0.18f };
            rect.position.y += hairVertAdj;
            rect.size.y -= hairVertAdj;

            rect.size.x *= 0.8f;

            if (m_isFacingRight)
            {
                rect.position.x += (m_sprite.getGlobalBounds().size.x * 0.15f);
            }
            else
            {
                rect.position.x += (m_sprite.getGlobalBounds().size.x * 0.25f);
            }
        }

        // should only get here if Action is Dead
        return rect;
    }

    const sf::FloatRect Avatar::attackCollisionRect() const
    {
        sf::FloatRect rect{ m_sprite.getGlobalBounds() };

        const float hairVertAdj{ rect.size.y * 0.165f };
        rect.position.y += hairVertAdj;
        rect.size.y -= (1.5f * hairVertAdj);

        rect.size.x *= 0.4f;

        if (m_isFacingRight)
        {
            rect.position.x += (m_sprite.getGlobalBounds().size.x * 0.5f);
        }
        else
        {
            rect.position.x += (m_sprite.getGlobalBounds().size.x * 0.1f);
        }

        return rect;
    }

    void Avatar::setSpawnPosForNewLevel(const sf::FloatRect & t_rect)
    {
        // spawn just above the enterRect so that the player falls a little
        sf::Vector2f position{ util::center(t_rect) };
        position.y = (util::bottom(t_rect) - m_sprite.getGlobalBounds().size.y);
        m_sprite.setPosition(position);
    }

    void Avatar::update(const Context & t_context, const float t_frameTimeSec)
    {
        if (handleDeath(t_context, t_frameTimeSec))
        {
            return;
        }

        const bool isAttacking{ handleAttacking(t_context, t_frameTimeSec) };
        const bool isThrowing{ handleThrowing(t_context, t_frameTimeSec) };
        const bool isGliding{ handleGliding(t_context, t_frameTimeSec) };

        if (!isAttacking)
        {
            handleBossCollisions(t_context);
        }

        if (!isAttacking && !isThrowing && !isGliding)
        {
            sideToSideMotion(t_context, t_frameTimeSec);
            jumping(t_context, t_frameTimeSec);
        }

        if (isGliding)
        {
            m_velocity +=
                ((t_context.settings.gravity_acc * t_context.settings.gravity_soaring_ratio) *
                 t_frameTimeSec);
        }
        else
        {
            m_velocity += (t_context.settings.gravity_acc * t_frameTimeSec);
        }

        m_sprite.move(m_velocity);

        moveMap(t_context);

        preventBacktracking(t_context);
        collisions(t_context);
        killCollisions(t_context);
        acidCollisions(t_context);
        waterCollisions(t_context);
        exitCollisions(t_context);
        handleAttackingEnemies(t_context);

        t_context.managers.collideAllWithAvatar(t_context, collisionRect());

        //
        // The normal collision rect is small, and makes gathering coins unfair.
        // This will increase it so that any slight near touch can gather a coin.
        // So check coin collisions all over again with this new rect.
        t_context.coins.collideWithAvatar(
            t_context, util::scaleRectInPlaceCopy(collisionRect(), { 1.3f, 1.75f }));

        if (t_context.managers.doesAvatarCollideWithAnyAndDie(collisionRect()))
        {
            triggerDeath(t_context);
        }

        killIfOutOfBounds(t_context);

        // this one must come last, after all possible sets to m_action
        handleIdle(t_context, t_frameTimeSec);
    }

    void Avatar::moveMap(const Context & t_context)
    {
        const sf::FloatRect mapRect{ t_context.layout.mapRegion() };
        const sf::Vector2f avatarPos{ util::center(m_sprite.getGlobalBounds()) };

        if (const sf::Vector2f screenMiddle{ t_context.layout.mapRegion().size * 0.5f };
            (m_velocity.x > 0.0f) && (avatarPos.x > screenMiddle.x))
        {
            const sf::Vector2f move{ (screenMiddle.x - avatarPos.x), 0.0f };
            if (t_context.level.move(t_context, move))
            {
                m_sprite.move(move);
            }
        }

        // moving up creates a positive move.y
        if (const float riseMovePoint{ mapRect.size.y * 0.12f };
            (m_velocity.y < 0.0f) && (avatarPos.y < riseMovePoint))
        {
            const sf::Vector2f move{ 0.0f, (riseMovePoint - avatarPos.y) };
            if (t_context.level.move(t_context, move))
            {
                m_sprite.move(move);
            }
        }

        // moving down creates a negative move.y
        if (const float fallMovePoint{ mapRect.size.y * 0.5f };
            (m_velocity.y > 0.0f) && (avatarPos.y > fallMovePoint))
        {
            const sf::Vector2f move{ 0.0f, (fallMovePoint - avatarPos.y) };
            if (t_context.level.move(t_context, move))
            {
                m_sprite.move(move);
            }
        }
    }

    bool Avatar::handleDeath(const Context & t_context, const float t_frameTimeSec)
    {
        if (Action::Dead != m_action)
        {
            return false;
        }

        // Delay a few seconds after death before changing states.
        // This allows the player to see how they died, and for all
        // the various sound effects to finish playing.
        m_deadDelaySec += t_frameTimeSec;
        if (m_deadDelaySec > t_context.settings.death_delay_sec)
        {
            if (t_context.info_region.lives() > 1)
            {
                t_context.info_region.livesAdjust(-1);

                t_context.level.load(t_context);

                m_action = Action::Idle;
                m_hasLanded = true;
                m_deadDelaySec = 0.0f;
                m_velocity = { 0.0f, 0.0f };

                if (!m_isFacingRight)
                {
                    m_isFacingRight = true;
                    m_sprite.scale({ -1.0f, 1.0f });
                }

                t_context.audio.play("respawn");
            }
            else
            {
                t_context.state.setChangePending(State::Lose);
            }
        }

        if (m_deathAnim.update(t_frameTimeSec))
        {
            m_sprite.setTexture(m_deathAnim.texture(), true);
        }

        m_blood.update(t_frameTimeSec);

        return true;
    }

    bool Avatar::handleAttacking(const Context & t_context, const float t_frameTimeSec)
    {
        // first frame
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::F) && (Action::Attack != m_action) &&
            (Action::Throw != m_action))
        {
            t_context.audio.play("swipe");

            // attacking in any way slows walk speed looks right
            if (Action::Run == m_action)
            {
                m_velocity.x *= t_context.settings.walk_speed_attack_reduction_ratio;
            }

            m_action = Action::Attack;

            m_attackAnim.restart();
            m_sprite.setTexture(m_attackAnim.texture(), true);

            return true;
        }

        // all other frames
        if (Action::Attack == m_action)
        {
            if (m_attackAnim.update(t_frameTimeSec))
            {
                m_sprite.setTexture(m_attackAnim.texture(), true);

                if (m_attackAnim.isFinished())
                {
                    m_action = Action::Idle;
                    return false;
                }
            }

            return true;
        }

        return false;
    }

    bool Avatar::handleThrowing(const Context & t_context, const float t_frameTimeSec)
    {
        m_timeSinceLastThrowSec += t_frameTimeSec;

        // first frame
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::D) && (Action::Attack != m_action) &&
            (Action::Throw != m_action) && (t_context.info_region.darts() > 0) &&
            (m_timeSinceLastThrowSec > 0.1f))
        {
            m_timeSinceLastThrowSec = 0.0f;

            t_context.audio.play("throw");

            // attacking in any way slowing walk speed looks right
            if (Action::Run == m_action)
            {
                m_velocity.x *= t_context.settings.walk_speed_throw_reduction_ratio;
            }

            m_action = Action::Throw;

            m_throwAnim.restart();
            m_sprite.setTexture(m_throwAnim.texture(), true);

            const sf::FloatRect avatarRect{ collisionRect() };
            sf::Vector2f missilePosition{ 0.0f, 0.0f };
            missilePosition.x = util::center(avatarRect).x;
            missilePosition.y = (avatarRect.position.y + (avatarRect.size.y * 0.4f));
            t_context.missiles.add(missilePosition, m_isFacingRight);

            t_context.info_region.dartsAdjust(-1);

            if (m_isFacingRight)
            {
                t_context.boss.reactToThrow(t_context);
            }

            return true;
        }

        // all other frames
        if (Action::Throw == m_action)
        {
            if (m_throwAnim.update(t_frameTimeSec))
            {
                m_sprite.setTexture(m_throwAnim.texture(), true);

                if (m_throwAnim.isFinished())
                {
                    m_action = Action::Idle;
                    return false;
                }
            }

            return true;
        }

        return false;
    }

    bool Avatar::handleGliding(const Context & t_context, const float t_frameTimeSec)
    {
        // first frame
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Up) && (Action::Jump == m_action) &&
            (m_velocity.y > 0.0f))
        {
            t_context.audio.play("parachute", 0.5f);
            m_action = Action::Glide;
            m_glideAnim.restart();
            m_sprite.setTexture(m_glideAnim.texture(), true);

            const float maxGlideVertVelocity{ 1.0f };
            if (m_velocity.y > maxGlideVertVelocity)
            {
                m_velocity.y = maxGlideVertVelocity;
            }

            return true;
        }

        // all other frames
        if (Action::Glide == m_action)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Up))
            {
                if (m_glideAnim.update(t_frameTimeSec))
                {
                    m_sprite.setTexture(m_glideAnim.texture(), true);
                }

                return true;
            }
            else
            {
                m_action = Action::Jump;
                m_sprite.setTexture(m_jumpTexture, true);
                return false;
            }
        }

        return false;
    }

    bool Avatar::handleIdle(const Context &, const float t_frameTimeSec)
    {
        if (Action::Idle == m_action)
        {
            if (m_idleAnim.update(t_frameTimeSec))
            {
                m_sprite.setTexture(m_idleAnim.texture(), true);
            }

            return true;
        }
        else
        {
            return false;
        }
    }

    void Avatar::sideToSideMotion(const Context & t_context, const float t_frameTimeSec)
    {
        if (Action::Jump == m_action)
        {
            // Allow moving side-to-side at a reduced rate while in the air.
            // It sounds wrong but feels so right.
            // What the hell, mario did it.
            const float jumpMoveRatio{ t_context.settings.jump_move_ratio };

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Right))
            {
                m_velocity.x += ((t_context.settings.walk_acc * jumpMoveRatio) * t_frameTimeSec);
                if (m_velocity.x > t_context.settings.walk_speed_limit)
                {
                    m_velocity.x = t_context.settings.walk_speed_limit;
                }
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Left))
            {
                m_velocity.x -= ((t_context.settings.walk_acc * jumpMoveRatio) * t_frameTimeSec);
                if (m_velocity.x < -t_context.settings.walk_speed_limit)
                {
                    m_velocity.x = -t_context.settings.walk_speed_limit;
                }
            }
        }
        else
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Right))
            {
                m_velocity.x += (t_context.settings.walk_acc * t_frameTimeSec);
                if (m_velocity.x > t_context.settings.walk_speed_limit)
                {
                    m_velocity.x = t_context.settings.walk_speed_limit;
                }

                if (Action::Run != m_action)
                {
                    m_runAnim.restart();
                }

                m_action = Action::Run;
                t_context.audio.play("walk");

                if (!m_isFacingRight)
                {
                    m_isFacingRight = true;
                    m_sprite.scale({ -1.0f, 1.0f });
                    m_sprite.move({ -m_sprite.getGlobalBounds().size.x, 0.0f });
                }
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Left))
            {
                m_velocity.x -= (t_context.settings.walk_acc * t_frameTimeSec);
                if (m_velocity.x < -t_context.settings.walk_speed_limit)
                {
                    m_velocity.x = -t_context.settings.walk_speed_limit;
                }

                if (Action::Run != m_action)
                {
                    m_runAnim.restart();
                }

                m_action = Action::Run;
                t_context.audio.play("walk");

                if (m_isFacingRight)
                {
                    m_isFacingRight = false;
                    m_sprite.scale({ -1.0f, 1.0f });
                    m_sprite.move({ m_sprite.getGlobalBounds().size.x, 0.0f });
                }
            }
            else
            {
                m_velocity.x = 0.0f;
                m_action = Action::Idle;
                t_context.audio.stop("walk");
            }
        }

        if (Action::Run == m_action)
        {
            if (m_runAnim.update(t_frameTimeSec))
            {
                m_sprite.setTexture(m_runAnim.texture(), true);
            }
        }
    }

    void Avatar::jumping(const Context & t_context, const float t_frameTimeSec)
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Up) && m_hasLanded)
        {
            m_hasLanded = false;
            m_velocity.y -= (t_context.settings.jump_acc * t_frameTimeSec);
            t_context.audio.stop("walk");
            t_context.audio.play("jump");
            m_action = Action::Jump;
            m_sprite.setTexture(m_jumpTexture, true);
        }
    }

    void Avatar::collisions(const Context & t_context)
    {
        const sf::FloatRect avatarRect{ collisionRect() };
        const sf::Vector2f avatarCenter{ util::center(avatarRect) };

        bool detectLanding{ false };

        for (const sf::FloatRect & collRect : t_context.level.walkCollisions())
        {
            const auto intersectionOpt{ avatarRect.findIntersection(collRect) };
            if (intersectionOpt)
            {
                collide(t_context, intersectionOpt.value(), avatarCenter, detectLanding);
            }
        }

        m_collisionRectCache.clear();
        t_context.managers.appendAllCollisions(m_collisionRectCache);
        for (const sf::FloatRect & collRect : m_collisionRectCache)
        {
            const auto intersectionOpt{ avatarRect.findIntersection(collRect) };
            if (intersectionOpt)
            {
                collide(t_context, intersectionOpt.value(), avatarCenter, detectLanding);
            }
        }

        if (!detectLanding)
        {
            m_hasLanded = false;
        }
    }

    void Avatar::collide(
        const Context & t_context,
        const sf::FloatRect & t_intersectionRect,
        const sf::Vector2f & t_avatarCenter,
        bool & t_detectLanding)
    {
        const float tolerance{ 25.0f }; // this magic number brought to you by zTn 2021-8-2
        const sf::Vector2f intersectCenter{ util::center(t_intersectionRect) };

        if ((m_velocity.y < 0.0f) && (t_intersectionRect.size.y < tolerance) &&
            (intersectCenter.y < t_avatarCenter.y))
        {
            // rising and hit something abov
            m_velocity.y = 0.0f;
            m_sprite.move({ 0.0f, t_intersectionRect.size.y });
        }
        else if (
            (m_velocity.y > 0.0f) && (t_intersectionRect.size.y < tolerance) &&
            (intersectCenter.y > t_avatarCenter.y))
        {
            // falling and hit something below

            if (!m_hasLanded)
            {
                t_context.audio.play("land");
                m_action = Action::Idle;
            }

            m_hasLanded = true;
            t_detectLanding = true;
            m_velocity.y = 0.0f;
            m_sprite.move({ 0.0f, -t_intersectionRect.size.y });
        }
        else if (t_intersectionRect.size.x < tolerance)
        {
            // hit something from the side
            m_velocity.x = 0.0f;

            if (intersectCenter.x < t_avatarCenter.x)
            {
                m_sprite.move({ t_intersectionRect.size.x, 0.0f });
            }
            else
            {
                m_sprite.move({ -t_intersectionRect.size.x, 0.0f });
            }
        }
    }

    void Avatar::killCollisions(const Context & t_context)
    {
        const sf::FloatRect avatarRect{ collisionRect() };

        for (const sf::FloatRect & coll : t_context.level.killCollisions())
        {
            if (avatarRect.findIntersection(coll))
            {
                triggerDeath(t_context);
                return;
            }
        }
    }

    void Avatar::exitCollisions(const Context & t_context) const
    {
        // let the player walk a little past the screen edge before exiting
        if (util::right(collisionRect()) > util::right(t_context.level.exitRect()))
        {
            t_context.audio.stopAllLooped();
            t_context.state.setChangePending(State::Level);
        }
    }

    void Avatar::preventBacktracking(const Context & t_context)
    {
        const sf::FloatRect backtrackRect{ { -100.0f, 0.0f },
                                           { 100.0f, t_context.layout.wholeSize().y } };

        const auto intersectionOpt{ collisionRect().findIntersection(backtrackRect) };
        if (intersectionOpt)
        {
            m_velocity.x = 0.0f;
            m_sprite.move({ intersectionOpt->size.x, 0.0f });
        }
    }

    void Avatar::handleAttackingEnemies(const Context & t_context)
    {
        if (Action::Attack != m_action)
        {
            return;
        }

        const sf::FloatRect attackRect{ attackCollisionRect() };
        if (t_context.slimes.attack(t_context, attackRect) ||
            t_context.bats.attack(t_context, attackRect))
        {
            // TODO what is this empty logic block?
        }
        else if (t_context.boss.attack(t_context, attackRect))
        {
            bounceAwayFromBoss(t_context);
        }
    }

    void Avatar::acidCollisions(const Context & t_context)
    {
        // this check prevents playing the sfx repeatedly while the player falls
        if (m_willDie)
        {
            return;
        }

        const sf::FloatRect avatarRect{ collisionRect() };

        for (const sf::FloatRect & coll : t_context.level.acidCollisions())
        {
            if (avatarRect.findIntersection(coll))
            {
                m_willDie = true;
                t_context.audio.play("acid.ogg");
                return;
            }
        }
    }

    void Avatar::waterCollisions(const Context & t_context)
    {
        // this check prevents playing the sfx repeatedly while the player falls
        if (m_willDie)
        {
            return;
        }

        const sf::FloatRect avatarRect{ collisionRect() };

        for (const sf::FloatRect & coll : t_context.level.waterCollisions())
        {
            if (avatarRect.findIntersection(coll))
            {
                m_willDie = true;
                t_context.audio.play("dunk.ogg");
                t_context.audio.play("dunk-bubble.ogg");
                return;
            }
        }
    }

    void Avatar::killIfOutOfBounds(const Context & t_context)
    {
        if (!t_context.layout.mapRegion().findIntersection(collisionRect()))
        {
            triggerDeath(t_context);
        }
    }

    void Avatar::triggerDeath(const Context & t_context)
    {
        if (Action::Dead == m_action)
        {
            return;
        }

        m_blood.start(t_context, m_sprite.getPosition(), m_isFacingRight);
        m_action = Action::Dead;
        t_context.audio.stop("walk");
        t_context.audio.play("scream");
        m_velocity = { 0.0f, 0.0f };
        m_deathAnim.restart();

        t_context.stats.has_player_died = true;
        t_context.stats.enemy_killed = 0;
        t_context.stats.coin_collected = 0;
    }

    void Avatar::handleBossCollisions(const Context & t_context)
    {
        if (!t_context.boss.doesCollide(collisionRect()))
        {
            return;
        }

        t_context.audio.play("hurt");
        bounceAwayFromBoss(t_context);
    }

    void Avatar::bounceAwayFromBoss(const Context & t_context)
    {
        m_sprite.move({ -15.0f, 0.0f });

        if (Action::Glide == m_action)
        {
            m_velocity.x = -(t_context.settings.walk_speed_limit * 1.0f);
            m_velocity.y = -(t_context.settings.walk_speed_limit * 0.25f);
        }
        else if (m_hasLanded)
        {
            m_velocity.x = -(t_context.settings.walk_speed_limit * 2.25f);
            m_velocity.y = -(t_context.settings.walk_speed_limit * 1.5f);
        }
        else
        {
            m_velocity.x = -(t_context.settings.walk_speed_limit * 1.5f);
            m_velocity.y = -(t_context.settings.walk_speed_limit * 0.75f);
        }
    }

    void Avatar::handleHitByBoss(const Context & t_context) { triggerDeath(t_context); }

} // namespace halloween
