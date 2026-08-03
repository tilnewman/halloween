// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// frog.cpp
//
#include "frog.hpp"

#include "context.hpp"
#include "fonts.hpp"
#include "random.hpp"
#include "screen-regions.hpp"
#include "settings.hpp"
#include "sfml-defaults.hpp"
#include "sfml-util.hpp"

#include <SFML/Graphics/Texture.hpp>

#include <string>

namespace halloween
{

    Frog::Frog(const Context & t_context, const sf::FloatRect & t_rect)
        : m_task{ FrogTask::Idle }
        , m_anim{ FrogAnim::Hop }
        , m_sprite{ util::SfmlDefaults::instance().texture() }
        , m_animElapsedSec{ 0.0f }
        , m_frameIndex{ 0 }
        , m_taskElapsedSec{ 0.0f }
        , m_rect{ t_rect }
        , m_isFacingRight{ t_context.random.boolean() }
        , m_taskRepeatAnimCount{ 0 }
        , m_debugText{ t_context.fonts.makeText(Font::General, 20, "") }
    {
        m_sprite.setTexture(t_context.frog_textures.textures(m_anim).at(0), true);
        m_sprite.setOrigin(m_sprite.getLocalBounds().size * 0.5f);

        const float scale{ 1.5f };
        m_sprite.setScale({ scale, scale });

        m_sprite.setPosition(
            { util::center(t_rect).x, (util::bottom(t_rect) - collisionRect().size.y) });

        m_sprite.move({ 0.0f, -(collisionRect().size.y * 0.225f) });

        // the frog art shows him facing left
        if (m_isFacingRight)
        {
            m_sprite.scale({ -1.0f, 1.0f });
        }
    }

    void Frog::turn()
    {
        m_isFacingRight = not m_isFacingRight;
        m_sprite.scale({ -1.0f, 1.0f });
    }

    const sf::FloatRect Frog::collisionRect() const
    {
        sf::FloatRect rect{ m_sprite.getGlobalBounds() };
        rect.position.y += (rect.size.y * 0.55f);
        rect.size.y -= (rect.size.y * 0.5f);
        util::scaleRectInPlace(rect, { 0.5f, 0.8f });

        if (FrogAnim::Hop == m_anim)
        {
            if (8 == m_frameIndex)
            {
                rect.position.y -= (rect.size.y * 0.15f);
            }
            else if (9 == m_frameIndex)
            {
                rect.position.y -= (rect.size.y * 0.45f);
            }
            else if (10 == m_frameIndex)
            {
                rect.position.y -= (rect.size.y * 0.75f);
            }
            else if (11 == m_frameIndex)
            {
                rect.position.y -= (rect.size.y * 0.95f);
            }
            else if (12 == m_frameIndex)
            {
                rect.position.y -= (rect.size.y * 1.20f);
            }
            else if (13 == m_frameIndex)
            {
                rect.position.y -= (rect.size.y * 1.1f);
            }
            else if (14 == m_frameIndex)
            {
                rect.position.y -= (rect.size.y * 0.95f);
            }
            else if (15 == m_frameIndex)
            {
                rect.position.y -= (rect.size.y * 0.8f);
            }
            else if (16 == m_frameIndex)
            {
                rect.position.y -= (rect.size.y * 0.65f);
            }
            else if (17 == m_frameIndex)
            {
                rect.position.y -= (rect.size.y * 0.4f);
            }
            else if (18 == m_frameIndex)
            {
                rect.position.y -= (rect.size.y * 0.2f);
            }
        }
        return rect;
    }

    const sf::FloatRect Frog::attackRect() const
    {
        sf::FloatRect rect{ collisionRect() };

        if (FrogAnim::AttackBite == m_anim)
        {
            util::scaleRectInPlace(rect, { 0.5f, 0.5f });
        }
        else
        {
            util::scaleRectInPlace(rect, { 0.65f, 0.5f });
        }

        rect.position.x += rect.size.x;

        if (not m_isFacingRight)
        {
            rect.position.x -= (rect.size.x * 2.0f);
        }

        return rect;
    }

    void Frog::setupTask(const FrogTask t_task, const FrogAnim t_anim)
    {
        m_task = t_task;
        m_anim = t_anim;
        m_frameIndex = 0;
        m_animElapsedSec = 0.0f;
        m_taskElapsedSec = 0.0f;
    }

    void Frog::update(const Context & t_context, const float t_frameTimeSec)
    {
        // update animation
        m_animElapsedSec += t_frameTimeSec;
        const float timePerFrameSec{ timePerFrame(m_anim) };
        if (m_animElapsedSec > timePerFrameSec)
        {
            m_animElapsedSec -= timePerFrameSec;

            const std::vector<sf::Texture> & textures{ t_context.frog_textures.textures(m_anim) };
            if (++m_frameIndex >= textures.size())
            {
                m_frameIndex = 0;

                if (m_taskRepeatAnimCount > 0)
                {
                    --m_taskRepeatAnimCount;
                }
            }

            m_sprite.setTexture(textures.at(m_frameIndex));
        }

        if (m_taskRepeatAnimCount > 0)
        {
            return;
        }

        // update task
        if (FrogTask::Idle == m_task)
        {
            if (t_context.random.boolean())
            {
                turn();
            }

            m_taskRepeatAnimCount = t_context.random.fromTo(1_st, 2_st);
            if (t_context.random.boolean())
            {
                setupTask(FrogTask::Idle, FrogAnim::Idle);
            }
            else
            {
                setupTask(FrogTask::Idle, FrogAnim::Eating);
            }
        }
    }

    void Frog::moveWithMap(const sf::Vector2f & t_move)
    {
        m_sprite.move(t_move);
        m_rect.position += t_move;
    }

    void Frog::collideWithAvatar(const Context &, const sf::FloatRect &) {}

    void Frog::draw(
        const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        if (t_context.layout.wholeRegion().findIntersection(m_sprite.getGlobalBounds()))
        {
            t_target.draw(m_sprite, t_states);

            const sf::FloatRect collRect{ collisionRect() };

            std::string str{ toString(m_task) };
            str += ", ";
            str += toString(m_anim);
            m_debugText.setString(str);
            util::setOriginToPosition(m_debugText);
            m_debugText.setPosition({ util::right(collRect), collRect.position.y });
            t_target.draw(m_debugText, t_states);

            // util::drawRectangleShape(t_target, collRect, false, sf::Color::Red);
            // util::drawRectangleShape(t_target, attackRect(), false, sf::Color::Yellow);
        }
    }

} // namespace halloween
