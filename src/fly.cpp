// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// fly.cpp
//
#include "fly.hpp"

#include "avatar.hpp"
#include "context.hpp"
#include "random.hpp"
#include "screen-regions.hpp"
#include "settings.hpp"
#include "sfml-defaults.hpp"
#include "sfml-util.hpp"
#include "sound-player.hpp"

namespace halloween
{

    Fly::Fly(
        const Context & t_context,
        const sf::FloatRect & t_rect, const FlyTextureManager & t_textureManager)
        : m_type{ t_context.random.from(
              { FlyType::Beholder, FlyType::Chomp, FlyType::Face, FlyType::Horn, FlyType::Peek }) }
        , m_anim{ FlyAnim::Fly }
        , m_task{ FlyTask::Wander }
        , m_sprite{ t_textureManager.textures(m_type, m_anim).at(0) }
        , m_animElapsedSec{ 0.0f }
        , m_rect{ t_rect }
        , m_frameIndex{ 0 }
        , m_hitPoints{ 1 }
        , m_isFacingRight{ t_context.random.boolean() }
        , m_hasDeathAnimFinished{ false }
        , m_textureManager{ t_textureManager }
    {
        util::setOriginToCenter(m_sprite);

        const float scale{ 0.5f };
        m_sprite.setScale({ scale, scale });

        m_sprite.setPosition(
            { util::center(m_rect).x, (util::bottom(m_rect) - m_sprite.getGlobalBounds().size.y) });

        if (m_isFacingRight)
        {
            m_sprite.scale({ -1.0f, 1.0f });
        }
    }

    void Fly::turn()
    {
        m_isFacingRight = not m_isFacingRight;
        m_sprite.scale({ -1.0f, 1.0f });
    }

    void Fly::setupTask(const FlyTask t_task, const FlyAnim t_anim)
    {
        m_task = t_task;
        m_anim = t_anim;
        m_animElapsedSec = 0.0f;
        m_frameIndex = 0;
    }

    void Fly::turnToFacePlayer(const Context & t_context)
    {
        const sf::FloatRect avatarRect{ t_context.avatar.collisionRect() };
        const bool isPlayerRight{ util::center(collisionRect()).x < util::center(avatarRect).x };
        if (isPlayerRight != m_isFacingRight)
        {
            turn();
        }
    }

    void Fly::update(const Context & t_context, const float m_frameTimeSec)
    {
        if (m_hasDeathAnimFinished)
        {
            return;
        }

        // animate
        m_animElapsedSec += m_frameTimeSec;
        const float timePerFrameSec{ 0.08f * ((FlyTask::Chase == m_task) ? 0.5f : 1.0f) };
        if (m_animElapsedSec > timePerFrameSec)
        {
            m_animElapsedSec -= timePerFrameSec;

            const auto & textures{ m_textureManager.textures(m_type, m_anim) };
            if (++m_frameIndex >= textures.size())
            {
                m_frameIndex = 0;

                if (FlyAnim::Hit == m_anim)
                {
                    m_hasDeathAnimFinished = true;
                    return;
                }
            }

            m_sprite.setTexture(textures.at(m_frameIndex), true);
        }

        // notice when player gets too close
        if (not t_context.avatar.isDead() and (FlyTask::Wander == m_task) and
            t_context.avatar.collisionRect().findIntersection(m_rect))
        {
            // TODO play 'fly noticed the player' sfx
            turnToFacePlayer(t_context);
            setupTask(FlyTask::Chase, FlyAnim::Fly);
        }

        // update tasks
        if (FlyTask::Wander == m_task)
        {
            const float wanderSpeed{ (m_isFacingRight) ? 30.0f : -30.0f };
            const sf::Vector2f move{ (wanderSpeed * m_frameTimeSec), 0.0f };
            m_sprite.move(move);

            const sf::FloatRect collRect{ collisionRect() };
            if ((collRect.position.x < m_rect.position.x) or
                (util::right(collRect) > util::right(m_rect)))
            {
                m_sprite.move(move * -1.0f);
                turn();
            }
        }
        else if (FlyTask::Chase == m_task)
        {
            turnToFacePlayer(t_context);

            const float chaseSpeed{ (m_isFacingRight) ? 60.0f : -60.0f };
            const sf::Vector2f move{ (chaseSpeed * m_frameTimeSec), 0.0f };
            m_sprite.move(move);

            const sf::FloatRect collRect{ collisionRect() };
            if ((collRect.position.x < m_rect.position.x) or
                (util::right(collRect) > util::right(m_rect)))
            {
                m_sprite.move(move * -1.0f);
                setupTask(FlyTask::Wander, FlyAnim::Fly);
            }
        }
    }

    void Fly::moveWithMap(const sf::Vector2f & t_move)
    {
        m_sprite.move(t_move);
        m_rect.position += t_move;
    }

    [[nodiscard]] bool
        Fly::doesAvatarCollideWithAnyAndDie(const Context &, const sf::FloatRect & t_avatarRect)
    {
        if (isAlive() and
            t_avatarRect.findIntersection(util::scaleRectInPlaceCopy(collisionRect(), 1.05f)))
        {
            setupTask(FlyTask::Wander, FlyAnim::Fly);
            return true;
        }
        else
        {
            return false;
        }
    }

    [[nodiscard]] const sf::FloatRect Fly::collisionRect() const
    {
        return util::scaleRectInPlaceCopy(m_sprite.getGlobalBounds(), { 0.45f, 0.75f });
    }

    void Fly::draw(
        const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        if (not m_hasDeathAnimFinished && t_context.layout.wholeRegion().findIntersection(collisionRect()))
        {
            t_target.draw(m_sprite, t_states);
            // util::drawRectangleShape(t_target, collisionRect(), false, sf::Color::Red);
        }
    }

    void Fly::hit(const Context & t_context)
    {
        t_context.audio.play("metal-hit");

        if (m_hitPoints > 0)
        {
            --m_hitPoints;

            if (not isAlive())
            {
                //t_context.audio.play("spider-death");
                setupTask(FlyTask::Death, FlyAnim::Hit);
            }
            else
            {
                // TODO t_context.audio.play("spider-hit");
                // setupTask(SpiderTask::Hit, ZombieAnim::Hit);
            }
        }
    }

} // namespace halloween
