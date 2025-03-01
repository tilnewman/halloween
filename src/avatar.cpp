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
#include "resources.hpp"
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
        : m_blood()
        , m_runAnim()
        , m_attackAnim()
        , m_deathAnim()
        , m_throwAnim()
        , m_glideAnim()
        , m_idleAnim()
        , m_jumpTexture()
        , m_sprite(util::SfmlDefaults::instance().texture())
        , m_velocity()
        , m_action(Action::Idle)
        , m_hasLanded(false)
        , m_isFacingRight(true)
        , m_deadDelaySec(0.0f)
        , m_willDie(false)
        , m_timeSinceLastThrowSec(0.0f)
    {}

    void Avatar::setup(const Settings & settings)
    {
        m_blood.setup(settings);

        m_runAnim.setup((settings.media_path / "image/avatar"), "Run", 10, 0.045f, true);
        m_attackAnim.setup((settings.media_path / "image/avatar"), "Attack", 10, 0.03f, false);
        m_deathAnim.setup((settings.media_path / "image/avatar"), "Dead", 10, 0.05f, false);
        m_throwAnim.setup((settings.media_path / "image/avatar"), "Throw", 10, 0.02f, false);
        m_glideAnim.setup((settings.media_path / "image/avatar"), "Glide", 10, 0.33f, true);
        m_idleAnim.setup((settings.media_path / "image/avatar"), "Idle", 10, 0.1f, true);

        util::TextureLoader::load(
            m_jumpTexture, (settings.media_path / "image/avatar/Jump-6.png"), true);

        m_idleAnim.restart();
        m_sprite.setTexture(m_idleAnim.texture(), true);
        m_sprite.setScale({ settings.avatar_scale, settings.avatar_scale });
    }

    void Avatar::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        target.draw(m_sprite, states);
        m_blood.draw(target, states);
    }

    const sf::FloatRect Avatar::collisionRect() const
    {
        // Avatar images for the various actions have different sizes and amounts of empty space
        // on the sides.  I know...free resources online suck.  So this function tweaks the
        // collison rect to compensate for that empty space on all four sides.  For example, the
        // attack sprite includes the sword swinging, but we don't want that sword colliding with
        // the walls and floors.

        auto rect = m_sprite.getGlobalBounds();

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
        auto rect = m_sprite.getGlobalBounds();

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

    void Avatar::setPosition(const sf::FloatRect & rect)
    {
        sf::Vector2f position = util::center(rect);
        position.y = (util::bottom(rect) - m_sprite.getGlobalBounds().size.y);
        m_sprite.setPosition(position);
    }

    void Avatar::update(Context & context, const float frameTimeSec)
    {
        if (handleDeath(context, frameTimeSec))
        {
            return;
        }

        const bool isAttacking = handleAttacking(context, frameTimeSec);
        const bool isThrowing = handleThrowing(context, frameTimeSec);
        const bool isGliding = handleGliding(context, frameTimeSec);

        if (!isAttacking)
        {
            handleBossCollisions(context);
        }

        if (!isAttacking && !isThrowing && !isGliding)
        {
            sideToSideMotion(context, frameTimeSec);
            jumping(context, frameTimeSec);
        }

        if (isGliding)
        {
            m_velocity +=
                ((context.settings.gravity_acc * sf::Vector2f(1.0f, 0.1f)) * frameTimeSec);
        }
        else
        {
            m_velocity += (context.settings.gravity_acc * frameTimeSec);
        }

        m_sprite.move(m_velocity);

        moveMap(context);

        preventBacktracking(context);
        collisions(context);
        killCollisions(context);
        acidCollisions(context);
        waterCollisions(context);
        exitCollisions(context);
        handleAttackingEnemies(context);

        context.managers.collideAllWithAvatar(context, collisionRect());

        //
        // The normal collision rect is small, and makes gathering coins unfair.
        // This will increase it so that any slight near touch can gather a coin.
        // So check coin collisions all over again with this new rect.
        context.coins.collideWithAvatar(
            context, util::scaleRectInPlaceCopy(collisionRect(), { 1.3f, 1.75f }));

        if (context.managers.doesAvatarCollideWithAnyAndDie(collisionRect()))
        {
            triggerDeath(context);
        }

        killIfOutOfBounds(context);

        // this one must come last, after all possible sets to m_action
        handleIdle(context, frameTimeSec);
    }

    void Avatar::moveMap(Context & context)
    {
        const float posXAfter = util::center(m_sprite.getGlobalBounds()).x;
        const float screenMiddle = (context.layout.mapRegion().size.x * 0.5f);

        if ((m_velocity.x < 0.0f) || (posXAfter < screenMiddle))
        {
            return;
        }

        const float moveX = (screenMiddle - posXAfter);

        if (!context.level.move(context.layout, moveX))
        {
            return;
        }

        const sf::Vector2f move{ moveX, 0.0f };
        m_sprite.move(move);
        context.managers.moveAllWithMap(move);
    }

    bool Avatar::handleDeath(Context & context, const float frameTimeSec)
    {
        if (Action::Dead != m_action)
        {
            return false;
        }

        // Delay a few seconds after death before changing states.
        // This allows the player to see how they died, and for all
        // the various sound effects to finish playing.
        m_deadDelaySec += frameTimeSec;
        if (m_deadDelaySec > context.settings.death_delay_sec)
        {
            if (context.info_region.lives() > 1)
            {
                context.info_region.livesAdjust(-1);

                context.level.load(context);

                m_action = Action::Idle;
                m_hasLanded = true;
                m_deadDelaySec = 0.0f;
                m_velocity = { 0.0f, 0.0f };

                if (!m_isFacingRight)
                {
                    m_isFacingRight = true;
                    m_sprite.scale({ -1.0f, 1.0f }); // sfml trick to flip image
                }

                context.audio.play("respawn");
            }
            else
            {
                context.state.setChangePending(State::Lose);
            }
        }

        if (m_deathAnim.update(frameTimeSec))
        {
            m_sprite.setTexture(m_deathAnim.texture(), true);
        }

        m_blood.update(frameTimeSec);

        return true;
    }

    bool Avatar::handleAttacking(Context & context, const float frameTimeSec)
    {
        // first frame
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::F) && (Action::Attack != m_action) &&
            (Action::Throw != m_action))
        {
            context.audio.play("swipe");

            // attacking in any way slows walk speed looks right
            if (Action::Run == m_action)
            {
                m_velocity.x *= context.settings.walk_speed_attack_reduction_ratio;
            }

            m_action = Action::Attack;

            m_attackAnim.restart();
            m_sprite.setTexture(m_attackAnim.texture(), true);

            return true;
        }

        // all other frames
        if (Action::Attack == m_action)
        {
            if (m_attackAnim.update(frameTimeSec))
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

    bool Avatar::handleThrowing(Context & context, const float frameTimeSec)
    {
        m_timeSinceLastThrowSec += frameTimeSec;

        // first frame
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::D) && (Action::Attack != m_action) &&
            (Action::Throw != m_action) && (context.info_region.darts() > 0) &&
            (m_timeSinceLastThrowSec > 0.1f))
        {
            m_timeSinceLastThrowSec = 0.0f;

            context.audio.play("throw");

            // attacking in any way slowing walk speed looks right
            if (Action::Run == m_action)
            {
                m_velocity.x *= context.settings.walk_speed_throw_reduction_ratio;
            }

            m_action = Action::Throw;

            m_throwAnim.restart();
            m_sprite.setTexture(m_throwAnim.texture(), true);

            const sf::FloatRect avatarRect = collisionRect();
            sf::Vector2f missilePosition{ 0.0f, 0.0f };
            missilePosition.x = util::center(avatarRect).x;
            missilePosition.y = (avatarRect.position.y + (avatarRect.size.y * 0.4f));
            context.missiles.add(missilePosition, m_isFacingRight);

            context.info_region.dartsAdjust(-1);

            if (m_isFacingRight)
            {
                context.boss.reactToThrow(context);
            }

            return true;
        }

        // all other frames
        if (Action::Throw == m_action)
        {
            if (m_throwAnim.update(frameTimeSec))
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

    bool Avatar::handleGliding(Context & context, const float frameTimeSec)
    {
        // first frame
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Up) && (Action::Jump == m_action) &&
            (m_velocity.y > -1.0f))
        {
            context.audio.play("parachute", 0.5f);
            m_action = Action::Glide;
            m_glideAnim.restart();
            m_sprite.setTexture(m_glideAnim.texture(), true);

            const float maxGlideVertVelocity = 1.0f;
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
                if (m_glideAnim.update(frameTimeSec))
                {
                    m_sprite.setTexture(m_glideAnim.texture(), true);
                }

                return true;
            }
            else
            {
                m_action = Action::Idle;
                return false;
            }
        }

        return false;
    }

    bool Avatar::handleIdle(Context &, const float frameTimeSec)
    {
        if (Action::Idle == m_action)
        {
            if (m_idleAnim.update(frameTimeSec))
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

    void Avatar::sideToSideMotion(Context & context, const float frameTimeSec)
    {
        if (Action::Jump == m_action)
        {
            // Allow moving side-to-side at a reduced rate while in the air.
            // It sounds wrong but feels so right.
            // What the hell, mario did it.
            const float jumpMoveDivisor = 3.0f;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Right))
            {
                m_velocity.x += ((context.settings.walk_acc / jumpMoveDivisor) * frameTimeSec);
                if (m_velocity.x > context.settings.walk_speed_limit)
                {
                    m_velocity.x = context.settings.walk_speed_limit;
                }
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Left))
            {
                m_velocity.x -= ((context.settings.walk_acc / jumpMoveDivisor) * frameTimeSec);
                if (m_velocity.x < -context.settings.walk_speed_limit)
                {
                    m_velocity.x = -context.settings.walk_speed_limit;
                }
            }
        }
        else
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Right))
            {
                m_velocity.x += (context.settings.walk_acc * frameTimeSec);
                if (m_velocity.x > context.settings.walk_speed_limit)
                {
                    m_velocity.x = context.settings.walk_speed_limit;
                }

                if (Action::Run != m_action)
                {
                    m_runAnim.restart();
                }

                m_action = Action::Run;
                context.audio.play("walk");

                if (!m_isFacingRight)
                {
                    m_isFacingRight = true;
                    m_sprite.scale({ -1.0f, 1.0f }); // sfml trick to horiz flip image
                    m_sprite.move({ -m_sprite.getGlobalBounds().size.x, 0.0f });
                }
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Left))
            {
                m_velocity.x -= (context.settings.walk_acc * frameTimeSec);
                if (m_velocity.x < -context.settings.walk_speed_limit)
                {
                    m_velocity.x = -context.settings.walk_speed_limit;
                }

                if (Action::Run != m_action)
                {
                    m_runAnim.restart();
                }

                m_action = Action::Run;
                context.audio.play("walk");

                if (m_isFacingRight)
                {
                    m_isFacingRight = false;
                    m_sprite.scale({ -1.0f, 1.0f }); // sfml trick to horiz flip image
                    m_sprite.move({ m_sprite.getGlobalBounds().size.x, 0.0f });
                }
            }
            else
            {
                m_velocity.x = 0.0f;
                m_action = Action::Idle;
                context.audio.stop("walk");
            }
        }

        if (Action::Run == m_action)
        {
            if (m_runAnim.update(frameTimeSec))
            {
                m_sprite.setTexture(m_runAnim.texture(), true);
            }
        }
    }

    void Avatar::jumping(Context & context, const float frameTimeSec)
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Up) && m_hasLanded)
        {
            m_hasLanded = false;
            m_velocity.y -= (context.settings.jump_acc * frameTimeSec);
            context.audio.play("jump");
            context.audio.stop("walk");
            m_action = Action::Jump;
            m_sprite.setTexture(m_jumpTexture, true);
        }
    }

    void Avatar::collisions(Context & context)
    {
        const float tolerance = 25.0f; // this magic number brought to you by zTn 2021-8-2

        const sf::FloatRect avatarRect = collisionRect();
        const sf::Vector2f avatarCenter = util::center(avatarRect);

        const float footRectHeightAdj{ avatarRect.size.y * 0.8f };
        sf::FloatRect footRect = avatarRect;
        footRect.position.y += footRectHeightAdj;
        footRect.size.y -= footRectHeightAdj;

        std::vector<sf::FloatRect> rects = context.level.walk_collisions;
        context.managers.appendAllCollisions(rects);

        bool hasHitSomething{ false };
        sf::FloatRect intersection;
        for (const sf::FloatRect & collRect : rects)
        {
            const auto intersectionOpt{ avatarRect.findIntersection(collRect) };
            if (intersectionOpt)
            {
                intersection = intersectionOpt.value();
            }
            else
            {
                continue;
            }

            hasHitSomething = true;
            const sf::Vector2f collCenter = util::center(collRect);

            if ((m_velocity.y < 0.0f) && (collCenter.y < avatarCenter.y))
            {
                // rising and hit something above

                m_velocity.y = 0.0f;
                m_sprite.move({ 0.0f, intersection.size.y });
                continue;
            }

            const bool doesIntersectFeet = collRect.findIntersection(footRect).has_value();

            if ((m_velocity.y > 0.0f) && (intersection.size.y < tolerance) && doesIntersectFeet)
            {
                // falling and hit something below

                if (!m_hasLanded)
                {
                    context.audio.play("land");
                    m_action = Action::Idle;
                }

                m_hasLanded = true;
                m_velocity.y = 0.0f;
                m_sprite.move({ 0.0f, -intersection.size.y });
                continue;
            }

            // at this point we hit something from the side

            if (intersection.size.x < tolerance)
            {
                if (m_velocity.x > 0.0f)
                {
                    m_velocity.x = 0.0f;
                    m_sprite.move({ -intersection.size.x, 0.0f });
                    continue;
                }
                else if (m_velocity.x < 0.0f)
                {
                    m_velocity.x = 0.0f;
                    m_sprite.move({ intersection.size.x, 0.0f });
                    continue;
                }
            }

            hasHitSomething = false;
        }

        if (!hasHitSomething)
        {
            m_hasLanded = false;
        }
    }

    void Avatar::killCollisions(Context & context)
    {
        const sf::FloatRect avatarRect = collisionRect();

        for (const sf::FloatRect & coll : context.level.kill_collisions)
        {
            if (avatarRect.findIntersection(coll))
            {
                triggerDeath(context);
                return;
            }
        }
    }

    void Avatar::exitCollisions(Context & context) const
    {
        if (collisionRect().findIntersection(context.level.exit_rect))
        {
            context.audio.stopAllLooped();
            context.state.setChangePending(State::Level);
            return;
        }
    }

    void Avatar::preventBacktracking(const Context & context)
    {
        const sf::FloatRect backtrackRect{ { -100.0f, 0.0f },
                                           { 100.0f, context.layout.wholeSize().y } };

        const auto intersectionOpt{ collisionRect().findIntersection(backtrackRect) };
        if (intersectionOpt)
        {
            m_velocity.x = 0.0f;
            m_sprite.move({ intersectionOpt->size.x, 0.0f });
        }
    }

    void Avatar::handleAttackingEnemies(Context & context)
    {
        if (Action::Attack != m_action)
        {
            return;
        }

        const auto attackRect = attackCollisionRect();
        if (context.slimes.attack(context, attackRect) || context.bats.attack(context, attackRect))
        {
        }
        else if (context.boss.attack(context, attackRect))
        {
            bounceAwayFromBoss(context);
        }
    }

    void Avatar::acidCollisions(Context & context)
    {
        // this check prevents playing the sfx repeatedly while the player falls
        if (m_willDie)
        {
            return;
        }

        const sf::FloatRect avatarRect = collisionRect();

        for (const sf::FloatRect & coll : context.level.acid_collisions)
        {
            if (avatarRect.findIntersection(coll))
            {
                m_willDie = true;
                context.audio.play("acid.ogg");
                return;
            }
        }
    }

    void Avatar::waterCollisions(Context & context)
    {
        // this check prevents playing the sfx repeatedly while the player falls
        if (m_willDie)
        {
            return;
        }

        const sf::FloatRect avatarRect = collisionRect();

        for (const sf::FloatRect & coll : context.level.water_collisions)
        {
            if (avatarRect.findIntersection(coll))
            {
                m_willDie = true;
                context.audio.play("dunk.ogg");
                context.audio.play("dunk-bubble.ogg");
                return;
            }
        }
    }

    void Avatar::killIfOutOfBounds(Context & context)
    {
        if (!context.layout.mapRegion().findIntersection(collisionRect()))
        {
            triggerDeath(context);
        }
    }

    void Avatar::triggerDeath(Context & context)
    {
        if (Action::Dead == m_action)
        {
            return;
        }

        m_blood.start(context, m_sprite.getPosition(), m_isFacingRight);
        m_action = Action::Dead;
        context.audio.stop("walk");
        context.audio.play("scream");
        m_velocity = { 0.0f, 0.0f };
        m_deathAnim.restart();

        context.stats.has_player_died = true;
        context.stats.enemy_killed = 0;
        context.stats.coin_collected = 0;
    }

    void Avatar::handleBossCollisions(Context & context)
    {
        if (!context.boss.doesCollide(collisionRect()))
        {
            return;
        }

        context.audio.play("hurt");
        bounceAwayFromBoss(context);
    }

    void Avatar::bounceAwayFromBoss(const Context & context)
    {
        m_sprite.move({ -15.0f, 0.0f });

        if (Action::Glide == m_action)
        {
            m_velocity.x = -(context.settings.walk_speed_limit * 1.0f);
            m_velocity.y = -(context.settings.walk_speed_limit * 0.25f);
        }
        else if (m_hasLanded)
        {
            m_velocity.x = -(context.settings.walk_speed_limit * 2.25f);
            m_velocity.y = -(context.settings.walk_speed_limit * 1.5f);
        }
        else
        {
            m_velocity.x = -(context.settings.walk_speed_limit * 1.5f);
            m_velocity.y = -(context.settings.walk_speed_limit * 0.75f);
        }
    }

    void Avatar::handleHitByBoss(Context & context) { triggerDeath(context); }

} // namespace halloween
