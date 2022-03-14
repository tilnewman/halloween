// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "avatar.hpp"

#include "check-macros.hpp"
#include "coin.hpp"
#include "context.hpp"
#include "info-region.hpp"
#include "level.hpp"
#include "missile.hpp"
#include "resources.hpp"
#include "screen-regions.hpp"
#include "settings.hpp"
#include "sound-player.hpp"
#include "states.hpp"

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
        , m_idleTexture()
        , m_jumpTexture()
        , m_sprite()
        , m_velocity()
        , m_acceleration(0.0f, 25.0f) // gravity only
        , m_action(Action::Idle)
        , m_hasLanded(false)
        , m_isFacingRight(true)
        , m_jumpSpeed(15.0f)
        , m_walkSpeed(15.0f)
        , m_walkSpeedLimit(5.0f)
        , m_deadDelaySec(0.0f)
    {}

    void Avatar::setup(const Settings & settings)
    {
        m_blood.setup(settings);

        m_runAnim.setup(settings.media_path, "Run", 10, 0.045f, true);
        m_attackAnim.setup(settings.media_path, "Attack", 10, 0.03f, false);
        m_deathAnim.setup(settings.media_path, "Dead", 10, 0.05f, false);
        m_throwAnim.setup(settings.media_path, "Throw", 10, 0.02f, false);

        const std::string IMAGE_PATH = (settings.media_path / "image/avatar/").string();

        m_idleTexture.loadFromFile(IMAGE_PATH + "Idle-0.png");
        m_jumpTexture.loadFromFile(IMAGE_PATH + "Jump-6.png");

        m_idleTexture.setSmooth(true);
        m_jumpTexture.setSmooth(true);

        m_sprite.setTexture(m_idleTexture);
        m_sprite.setScale(settings.avatar_scale, settings.avatar_scale);
    }

    void Avatar::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        target.draw(m_sprite, states);
        m_blood.draw(target, states);
    }

    sf::FloatRect Avatar::collisionRect() const
    {
        // Avatar images for the various actions have different sizes and amounts of empty space
        // on the sides.  I know...free resources online suck.  So this function tweaks the
        // collison rect to compensate for that empty space on all four sides.  For example, the
        // attack sprite includes the sword swinging, but we don't want that sword colliding with
        // the walls and floors.

        auto rect = m_sprite.getGlobalBounds();

        if (Action::Idle == m_action)
        {
            const float HAIR_VERT_ADJ = 0.15f;
            rect.top += (rect.height * HAIR_VERT_ADJ);
            rect.height -= (rect.height * HAIR_VERT_ADJ);

            rect.width *= 0.7f;
            if (m_isFacingRight)
            {
                rect.left += (m_sprite.getGlobalBounds().width * 0.2f);
            }
            else
            {
                rect.left += (m_sprite.getGlobalBounds().width * 0.1f);
            }
        }
        else if (Action::Attack == m_action)
        {
            const float HAIR_VERT_ADJ = 0.165f;
            rect.top += (rect.height * HAIR_VERT_ADJ);
            rect.height -= (rect.height * (1.5f * HAIR_VERT_ADJ));

            rect.width *= 0.4f;
            if (m_isFacingRight)
            {
                rect.left += (m_sprite.getGlobalBounds().width * 0.1f);
            }
            else
            {
                rect.left += (m_sprite.getGlobalBounds().width * 0.5f);
            }
        }
        else if (Action::Run == m_action)
        {
            const float HAIR_VERT_ADJ = 0.18f;
            rect.top += (rect.height * HAIR_VERT_ADJ);
            rect.height -= (rect.height * HAIR_VERT_ADJ);

            rect.width *= 0.5f;
            if (m_isFacingRight)
            {
                rect.left += (m_sprite.getGlobalBounds().width * 0.2f);
            }
            else
            {
                rect.left += (m_sprite.getGlobalBounds().width * 0.3f);
            }
        }
        else if (Action::Jump == m_action)
        {
            const float HAIR_VERT_ADJ = 0.18f;
            rect.top += (rect.height * HAIR_VERT_ADJ);
            rect.height -= (rect.height * (1.5f * HAIR_VERT_ADJ));

            rect.width *= 0.5f;
            if (m_isFacingRight)
            {
                rect.left += (m_sprite.getGlobalBounds().width * 0.2f);
            }
            else
            {
                rect.left += (m_sprite.getGlobalBounds().width * 0.4f);
            }
        }
        else if (Action::Throw == m_action)
        {
            const float HAIR_VERT_ADJ = 0.18f;
            rect.top += (rect.height * HAIR_VERT_ADJ);
            rect.height -= (rect.height * HAIR_VERT_ADJ);

            rect.width *= 0.5f;
            if (m_isFacingRight)
            {
                rect.left += (m_sprite.getGlobalBounds().width * 0.17f);
            }
            else
            {
                rect.left += (m_sprite.getGlobalBounds().width * 0.33f);
            }
        }

        return rect;
    }

    sf::FloatRect Avatar::attackCollisionRect() const
    {
        sf::FloatRect rect{ 0.0f, 0.0f, 0.0f, 0.0f };

        if (Action::Attack == m_action)
        {
            rect = m_sprite.getGlobalBounds();

            const float HAIR_VERT_ADJ = 0.165f;
            rect.top += (rect.height * HAIR_VERT_ADJ);
            rect.height -= (rect.height * (1.5f * HAIR_VERT_ADJ));

            rect.width *= 0.4f;
            if (m_isFacingRight)
            {
                rect.left += (m_sprite.getGlobalBounds().width * 0.5f);
            }
            else
            {
                rect.left += (m_sprite.getGlobalBounds().width * 0.1f);
            }
        }

        return rect;
    }

    void Avatar::setPosition(const sf::FloatRect & RECT)
    {
        sf::Vector2f position = util::center(RECT);
        position.y = (util::bottom(RECT) - m_sprite.getGlobalBounds().height);
        m_sprite.setPosition(position);
    }

    void Avatar::setAction(const Action ACTION)
    {
        m_action = ACTION;

        // clang-format off
        switch (ACTION)
        {
            case Action::Idle:   { m_sprite.setTexture(m_idleTexture, true);  break; }
            case Action::Jump:   { m_sprite.setTexture(m_jumpTexture, true);  break; }
            case Action::Dead:
            case Action::Throw:
            case Action::Attack:
            case Action::Run:    { break; }
            default:             { assert(false); break; }
        }
        // clang-format on
    }

    void Avatar::update(Context & context, const float FRAME_TIME_SEC)
    {
        if (handleDeath(context, FRAME_TIME_SEC))
        {
            return;
        }

        const bool IS_ATTACKING = handleAttacking(context, FRAME_TIME_SEC);
        const bool IS_THROWING = handleThrowing(context, FRAME_TIME_SEC);

        if (!IS_ATTACKING && !IS_THROWING)
        {
            sideToSideMotion(context, FRAME_TIME_SEC);
            jumping(context);
        }

        m_velocity += (m_acceleration * FRAME_TIME_SEC);
        m_sprite.move(m_velocity);

        moveMap(context);

        preventBacktracking(context);
        walkCollisions(context);
        killCollisions(context);
        exitCollisions(context);
        coinCollisions(context);
        respawnIfOutOfBounds(context);
    }

    void Avatar::moveMap(Context & context)
    {
        const float POSITION_X_AFTER = util::center(m_sprite.getGlobalBounds()).x;
        const float SCREEN_MIDDLE = (context.layout.mapRegion().width * 0.5f);

        if ((m_velocity.x < 0.0f) || (POSITION_X_AFTER < SCREEN_MIDDLE))
        {
            return;
        }

        const float MOVE_X = (SCREEN_MIDDLE - POSITION_X_AFTER);

        if (!context.level.move(context.layout, MOVE_X))
        {
            return;
        }

        m_sprite.move(MOVE_X, 0.0f);
        context.coins.move({ MOVE_X, 0.0f });
    }

    bool Avatar::handleDeath(Context & context, const float FRAME_TIME_SEC)
    {
        if (Action::Dead == m_action)
        {
            // Delay a few seconds after death before changing states.
            // This allows the player to see how they died.

            m_deadDelaySec += FRAME_TIME_SEC;
            if (m_deadDelaySec > 4.0f)
            {
                context.state.setChangePending(State::GameOver);
            }

            if (m_deathAnim.update(FRAME_TIME_SEC))
            {
                m_sprite.setTexture(m_deathAnim.texture(), true);
            }

            m_blood.update(context, FRAME_TIME_SEC);
            return true;
        }

        return false;
    }

    bool Avatar::handleAttacking(Context & context, const float FRAME_TIME_SEC)
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::F) && (Action::Attack != m_action) &&
            (Action::Throw != m_action))
        {
            context.audio.play("swipe");

            // attacking in any way slows walk speed looks right
            if (Action::Run == m_action)
            {
                m_velocity.x *= 0.8f;
            }

            setAction(Action::Attack);

            m_attackAnim.restart();
            m_sprite.setTexture(m_attackAnim.texture(), true);

            return true;
        }

        if (Action::Attack == m_action)
        {
            if (m_attackAnim.update(FRAME_TIME_SEC))
            {
                m_sprite.setTexture(m_attackAnim.texture(), true);

                if (m_attackAnim.isFinished())
                {
                    setAction(Action::Idle);
                }
            }

            return true;
        }

        return false;
    }

    bool Avatar::handleThrowing(Context & context, const float FRAME_TIME_SEC)
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && (Action::Attack != m_action) &&
            (Action::Throw != m_action))
        {
            context.audio.play("throw");

            // attacking in any way slows walk speed looks right
            if (Action::Run == m_action)
            {
                m_velocity.x *= 0.8f;
            }

            setAction(Action::Throw);

            m_throwAnim.restart();
            m_sprite.setTexture(m_throwAnim.texture(), true);

            const sf::FloatRect AVATAR_RECT = collisionRect();
            sf::Vector2f missilePosition{ 0.0f, 0.0f };
            missilePosition.x = util::center(AVATAR_RECT).x;
            missilePosition.y = (AVATAR_RECT.top + (AVATAR_RECT.height * 0.4f));
            context.missiles.add(missilePosition, m_isFacingRight);

            return true;
        }

        if (Action::Throw == m_action)
        {
            if (m_throwAnim.update(FRAME_TIME_SEC))
            {
                m_sprite.setTexture(m_throwAnim.texture(), true);

                if (m_throwAnim.isFinished())
                {
                    setAction(Action::Idle);
                }
            }

            return true;
        }

        return false;
    }

    void Avatar::sideToSideMotion(Context & context, const float FRAME_TIME_SEC)
    {
        if (Action::Jump == m_action)
        {
            // Allow moving side-to-side while in the air.
            // It sounds wrong but feels so right.
            // What the hell, mario did it.

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            {
                m_velocity.x += ((m_walkSpeed / 3.0f) * FRAME_TIME_SEC);
                if (m_velocity.x > m_walkSpeedLimit)
                {
                    m_velocity.x = m_walkSpeedLimit;
                }
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            {
                m_velocity.x -= ((m_walkSpeed / 3.0f) * FRAME_TIME_SEC);
                if (m_velocity.x < -m_walkSpeedLimit)
                {
                    m_velocity.x = -m_walkSpeedLimit;
                }
            }
        }
        else
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            {
                m_velocity.x += (m_walkSpeed * FRAME_TIME_SEC);
                if (m_velocity.x > m_walkSpeedLimit)
                {
                    m_velocity.x = m_walkSpeedLimit;
                }

                if (Action::Run != m_action)
                {
                    m_runAnim.restart();
                }

                setAction(Action::Run);
                context.audio.play("walk");

                if (!m_isFacingRight)
                {
                    m_isFacingRight = true;
                    m_sprite.scale(-1.0f, 1.0f);
                    m_sprite.move(-m_sprite.getGlobalBounds().width, 0.0f);
                }
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            {
                m_velocity.x -= (m_walkSpeed * FRAME_TIME_SEC);
                if (m_velocity.x < -m_walkSpeedLimit)
                {
                    m_velocity.x = -m_walkSpeedLimit;
                }

                if (Action::Run != m_action)
                {
                    m_runAnim.restart();
                }

                setAction(Action::Run);
                context.audio.play("walk");

                if (m_isFacingRight)
                {
                    m_isFacingRight = false;
                    m_sprite.scale(-1.0f, 1.0f);
                    m_sprite.move(m_sprite.getGlobalBounds().width, 0.0f);
                }
            }
            else
            {
                m_velocity.x = 0.0f;
                setAction(Action::Idle);
                context.audio.stop("walk");
            }
        }

        if (Action::Run == m_action)
        {
            if (m_runAnim.update(FRAME_TIME_SEC))
            {
                m_sprite.setTexture(m_runAnim.texture(), true);
            }
        }
    }

    void Avatar::jumping(Context & context)
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && m_hasLanded)
        {
            m_hasLanded = false;
            m_velocity.y -= m_jumpSpeed;
            context.audio.play("jump");
            context.audio.stop("walk");
            setAction(Action::Jump);
        }
    }

    void Avatar::walkCollisions(Context & context)
    {
        const float TOLERANCE = 25.0f; // this magic number brought to you by zTn 2021-8-2

        const sf::FloatRect AVATAR_RECT = collisionRect();
        const sf::Vector2f AVATAR_CENTER = util::center(AVATAR_RECT);

        sf::FloatRect footRect = AVATAR_RECT;
        footRect.top += (AVATAR_RECT.height * 0.8f);
        footRect.height -= (AVATAR_RECT.height * 0.8f);

        bool hasHitSomething{ false };
        sf::FloatRect intersection;
        for (const sf::FloatRect & COLL_RECT : context.level.walk_collisions)
        {
            if (!AVATAR_RECT.intersects(COLL_RECT, intersection))
            {
                continue;
            }

            hasHitSomething = true;
            const sf::Vector2f COLL_CENTER = util::center(COLL_RECT);

            if ((m_velocity.y < 0.0f) && (COLL_CENTER.y < AVATAR_CENTER.y))
            {
                // rising and hit something above

                m_velocity.y = 0.0f;
                m_sprite.move(0.0f, intersection.height);
                continue;
            }

            const bool DOES_INTERSECT_FEET = COLL_RECT.intersects(footRect);

            if ((m_velocity.y > 0.0f) && (intersection.height < TOLERANCE) && DOES_INTERSECT_FEET)
            {
                // falling and hit something below

                if (!m_hasLanded)
                {
                    context.audio.play("land");
                    setAction(Action::Idle);
                }

                m_hasLanded = true;
                m_velocity.y = 0.0f;
                m_sprite.move(0.0f, -intersection.height);
                continue;
            }

            if (intersection.width < TOLERANCE)
            {
                if (m_velocity.x > 0.0f)
                {
                    m_sprite.move(-intersection.width, 0.0f);
                }
                else if (m_velocity.x < 0.0f)
                {
                    m_sprite.move(intersection.width, 0.0f);
                }
            }
        }

        if (!hasHitSomething)
        {
            m_hasLanded = false;
        }
    }

    void Avatar::killCollisions(Context & context)
    {
        const sf::FloatRect AVATAR_RECT = collisionRect();

        sf::FloatRect intersection;
        for (const sf::FloatRect & COLL : context.level.kill_collisions)
        {
            if (!AVATAR_RECT.intersects(COLL, intersection))
            {
                continue;
            }

            m_blood.start(context, m_sprite.getPosition(), m_isFacingRight);
            setAction(Action::Dead);
            context.audio.play("scream");
            context.audio.stop("walk");
            m_velocity = { 0.0f, 0.0f };
            m_deathAnim.restart();
            context.info_region.livesAdjust(-1);
            return;
        }
    }

    void Avatar::exitCollisions(Context & context) const
    {
        if (collisionRect().intersects(context.level.exit_rect))
        {
            context.audio.stopAllLooped();
            context.state.setChangePending(State::Level);
        }
    }

    void Avatar::preventBacktracking(const Context & context)
    {
        const sf::FloatRect backtrackRect{ -100.0f, 0.0f, 100.0f, context.layout.wholeSize().y };

        sf::FloatRect intersection;
        if (collisionRect().intersects(backtrackRect, intersection))
        {
            m_velocity.x = 0.0f;
            m_sprite.move(intersection.width, 0.0f);
        }
    }

    void Avatar::coinCollisions(Context & context) const
    {
        sf::FloatRect rect = collisionRect();

        // The normal collision rect is very small, and makes gathering coins unfair.
        // This will increase it so that any slight touch can gather coins.
        util::scaleRectInPlace(rect, { 1.2f, 1.75f });

        context.coins.collideWithAvatar(context, rect);
    }

    void Avatar::respawnIfOutOfBounds(Context & context)
    {
        if (context.layout.mapRegion().intersects(collisionRect()))
        {
            return;
        }

        m_velocity = { 0.0f, 0.0f };
        m_sprite.setPosition(m_sprite.getPosition().x, 0.0f);
    }

} // namespace halloween
