// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// zombie.cpp
//
#include "zombie.hpp"

#include "context.hpp"
#include "fonts.hpp"
#include "random.hpp"
#include "screen-regions.hpp"
#include "settings.hpp"
#include "sfml-defaults.hpp"
#include "sfml-util.hpp"

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

    void Zombie::setupTask(const ZombieTask t_task, const ZombieAnim t_anim)
    {
        m_task = t_task;
        m_anim = t_anim;
        m_animElpasedSec = 0.0f;
        m_taskElapsedSec = 0.0f;
    }

    void Zombie::update(const Context & t_context, const float t_frameTimeSec)
    {
        // update animation
        m_animElpasedSec += t_frameTimeSec;
        const float timePerFrameSec{ timePerFrame(m_anim) };
        if (m_animElpasedSec > timePerFrameSec)
        {
            m_animElpasedSec -= timePerFrameSec;

            const auto & textures{ t_context.zombie_textures.textures(m_anim) };
            if (++m_frameIndex >= textures.size())
            {
                m_frameIndex = 0;
            }

            m_sprite.setTexture(textures.at(m_frameIndex), true);
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
            const float walkSpeed{ 600.0f * ((m_isFacingRight) ? 1.0f : -1.0f) };
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
            if (std::abs(pos - m_wanderTarget) < 10.0f) // five pixels is close enough
            {
                startWanderingOrStaring(t_context);
            }
        }
    }

    void Zombie::startWanderingOrStaring(const Context & t_context)
    {
        if (true)
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
            t_target.draw(m_debugText, t_states);

            const sf::FloatRect rect({ m_wanderTarget, util::center(collisionRect()).y }, { 1, 1 });
            util::drawRectangleShape(t_target, rect, false, sf::Color::Green);

            util::drawRectangleShape(t_target, collisionRect(), false, sf::Color::Red);
        }
    }

} // namespace halloween
