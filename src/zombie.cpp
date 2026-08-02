// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// zombie.cpp
//
#include "zombie.hpp"

#include "avatar.hpp"
#include "context.hpp"
#include "fonts.hpp"
#include "random.hpp"
#include "screen-regions.hpp"
#include "settings.hpp"
#include "sfml-defaults.hpp"
#include "sfml-util.hpp"
#include "sound-player.hpp"

namespace halloween
{

    Zombie::Zombie(const Context & t_context, const sf::FloatRect & t_rect)
        : m_anim{ ZombieAnim::Idle }
        , m_task{ ZombieTask::Stare }
        , m_sprite{ util::SfmlDefaults::instance().texture() }
        , m_rect{ t_rect }
        , m_isFacingRight{ t_context.random.boolean() }
        , m_animElpasedSec{ 0.0f }
        , m_taskElapsedSec{ 0.0f }
        , m_frameIndex{ 0 }
        , m_wanderTarget{ 0.0f }
        , m_walkSpeed{ 30.0f }
        , m_debugText{ util::SfmlDefaults::instance().font() }
    {
        m_sprite.setTexture(t_context.zombie_textures.textures(m_anim).at(0), true);

        const float scale{ 0.6f };
        m_sprite.setScale({ scale, scale });

        const sf::FloatRect collRect{ collisionRect() };

        m_sprite.setOrigin(
            { ((collRect.position.x * 0.5f) + (collRect.size.x * 1.65f)),
              collRect.position.y + (collRect.size.y * 0.5f) });

        m_sprite.setPosition(
            { (util::center(t_rect).x - (collRect.size.x * 0.5f)),
              (util::bottom(t_rect) - collRect.size.y) });

        m_sprite.move({ 0.0f, (collRect.size.y * 0.125f) });

        if (!m_isFacingRight)
        {
            m_sprite.scale({ -1.0f, 1.0f });
        }

        m_debugText = t_context.fonts.makeText(Font::General, 16, "");
    }

    void Zombie::turn()
    {
        m_isFacingRight = !m_isFacingRight;
        m_sprite.scale({ -1.0f, 1.0f });
    }

    const sf::FloatRect Zombie::collisionRect() const
    {
        sf::FloatRect rect{ m_sprite.getGlobalBounds() };
        util::scaleRectInPlace(rect, 0.8f);
        rect.size.x -= (rect.size.x * 0.5f);

        if (!m_isFacingRight)
        {
            rect.position.x += rect.size.x;
        }

        util::scaleRectInPlace(rect, { 0.5f, 0.8f });

        return rect;
    }

    const sf::FloatRect Zombie::attackRect() const
    {
        if (ZombieAnim::Attack != m_anim)
        {
            return {};
        }

        sf::FloatRect rect{ m_sprite.getGlobalBounds() };
        util::scaleRectInPlace(rect, 0.4f);
        rect.position.x += (rect.size.x * 0.5f);
        rect.position.y -= (rect.size.y * 0.6f);
        util::scaleRectInPlace(rect, { 1.25f, 1.0f });

        if (m_frameIndex < 14)
        {
            rect.position.x -= (rect.size.x * 0.5f);
            rect.position.y -= (rect.size.y * 0.25f);
            rect.size.y *= 0.5f;
        }
        else
        {
            if (!m_isFacingRight)
            {
                rect.position.x -= rect.size.x;
            }
        }

        return rect;
    }

    void Zombie::setupTask(const ZombieTask t_task, const ZombieAnim t_anim)
    {
        m_task = t_task;
        m_anim = t_anim;
        m_frameIndex = 0;
        m_animElpasedSec = 0.0f;
        m_taskElapsedSec = 0.0f;
    }

    float Zombie::calcTimePerFrame() const
    {
        float timePerFrameSec{ timePerFrame(m_anim) };

        if ((ZombieTask::Chase == m_task) && (ZombieAnim::Walk == m_anim))
        {
            timePerFrameSec *= 0.5f;
        }

        return timePerFrameSec;
    }

    void Zombie::update(const Context & t_context, const float t_frameTimeSec)
    {
        // update animation
        m_animElpasedSec += t_frameTimeSec;

        const float timePerFrameSec{ calcTimePerFrame() };
        if (m_animElpasedSec > timePerFrameSec)
        {
            m_animElpasedSec -= timePerFrameSec;

            const auto & textures{ t_context.zombie_textures.textures(m_anim) };
            if (++m_frameIndex >= textures.size())
            {
                m_frameIndex = 0;

                if (ZombieAnim::Attack == m_anim)
                {
                    startChasing(t_context);
                }
            }

            m_sprite.setTexture(textures.at(m_frameIndex), true);
        }

        // notice when the player gets witihin range
        if ((ZombieTask::Stare == m_task) || (ZombieTask::Wander == m_task))
        {
            if (m_rect.findIntersection(t_context.avatar.collisionRect()))
            {
                startChasing(t_context);
            }
        }

        // update tasks
        m_taskElapsedSec += t_frameTimeSec;

        if (ZombieTask::Stare == m_task)
        {
            if (m_taskElapsedSec > m_taskDurationSec)
            {
                startWanderingOrStaring(t_context);
            }
        }
        else if (ZombieTask::Wander == m_task)
        {
            const float walkSpeed{ m_walkSpeed * ((m_isFacingRight) ? 1.0f : -1.0f) };
            m_sprite.move({ (walkSpeed * t_frameTimeSec), 0.0f });

            const sf::FloatRect collRect{ collisionRect() };
            if (collRect.position.x < m_rect.position.x)
            {
                startWanderingOrStaring(t_context);
            }
            else if (util::right(collRect) > util::right(m_rect))
            {
                startWanderingOrStaring(t_context);
            }

            const float pos{ util::center(collisionRect()).x };
            if (std::abs(pos - m_wanderTarget) < 10.0f) // close enough
            {
                startWanderingOrStaring(t_context);
            }
        }
        else if (ZombieTask::Chase == m_task)
        {
            // stop chasing if time has come
            if (m_taskElapsedSec > m_taskDurationSec)
            {
                setupTask(ZombieTask::Attack, ZombieAnim::Attack);
            }
            else
            {
                // turn towards the player
                const bool isPlayerRight{ util::center(collisionRect()).x <
                                          util::center(t_context.avatar.collisionRect()).x };

                if (isPlayerRight != m_isFacingRight)
                {
                    turn();
                }

                // move toward player
                const float chaseSpeed{ m_walkSpeed * ((m_isFacingRight) ? 2.0f : -2.0f) };
                const float moveHoriz{ chaseSpeed * t_frameTimeSec };
                m_sprite.move({ moveHoriz, 0.0f });

                // move back if going out of bounds
                const sf::FloatRect collRect{ collisionRect() };
                if (not m_isFacingRight && (collRect.position.x < m_rect.position.x))
                {
                    m_sprite.move({ moveHoriz, 0.0f });
                    startWanderingOrStaring(t_context);
                }
                else if (m_isFacingRight && (util::right(collRect) > util::right(m_rect)))
                {
                    m_sprite.move({ -moveHoriz, 0.0f });
                    startWanderingOrStaring(t_context);
                }
            }
        }
        else if (ZombieTask::Attack == m_task)
        {
        }
    }

    void Zombie::startWanderingOrStaring(const Context & t_context)
    {
        if (t_context.random.boolean())
        {
            setupTask(ZombieTask::Wander, ZombieAnim::Walk);

            m_wanderTarget = t_context.random.fromTo(m_rect.position.x, util::right(m_rect));
            const bool isWanderTargetRight{ util::center(collisionRect()).x < m_wanderTarget };
            if (isWanderTargetRight != m_isFacingRight)
            {
                turn();
            }
        }
        else
        {
            setupTask(ZombieTask::Stare, ZombieAnim::Idle);
            m_taskDurationSec = t_context.random.fromTo(2.0f, 5.0f);

            if (t_context.random.boolean())
            {
                turn();
            }
        }
    }

    void Zombie::startChasing(const Context & t_context)
    {
        t_context.audio.play("zombie-alert");
        setupTask(ZombieTask::Chase, ZombieAnim::Walk);
        m_taskDurationSec = t_context.random.fromTo(0.5f, 2.0f);
    }

    void Zombie::moveWithMap(const sf::Vector2f & t_move)
    {
        m_sprite.move(t_move);
        m_rect.position += t_move;
        m_wanderTarget += t_move.x;
    }

    void Zombie::draw(
        const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        if (t_context.layout.wholeRegion().findIntersection(m_sprite.getGlobalBounds()))
        {
            t_target.draw(m_sprite, t_states);

            std::string str{ toString(m_task) };
            str += ", ";
            str += toString(m_anim);
            str += ", ";
            str += ((m_isFacingRight) ? "right" : "left");

            m_debugText.setString(str);
            util::setOriginToPosition(m_debugText);
            m_debugText.setPosition({ util::right(collisionRect()), collisionRect().position.y });
            // t_target.draw(m_debugText, t_states);

            // util::drawRectangleShape(t_target, collisionRect(), false, sf::Color::Red);
            // util::drawRectangleShape(t_target, attackRect(), false, sf::Color::Yellow);
        }
    }

} // namespace halloween
