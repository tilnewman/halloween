// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// zombie.cpp
//
#include "zombie.hpp"

#include "context.hpp"
#include "random.hpp"
#include "screen-regions.hpp"
#include "settings.hpp"
#include "sfml-defaults.hpp"
#include "sfml-util.hpp"

namespace halloween
{

    Zombie::Zombie(const Context & t_context, const sf::FloatRect & t_rect)
        : m_sprite{ util::SfmlDefaults::instance().texture() }
        , m_anim{ ZombieAnim::Idle }
        , m_rect{ t_rect }
        , m_isFacingRight{ t_context.random.boolean() }
        , m_animElpasedTimeSec{ 0.0f }
        , m_frameIndex{ 0 }
    {
        m_sprite.setTexture(t_context.zombie_textures.textures(m_anim).at(0), true);

        const float scale{ 0.75f };
        m_sprite.setScale({ scale, scale });

        const sf::FloatRect collRect{ collisionRect() };

        m_sprite.setPosition(
            { (util::center(t_rect).x - (collRect.size.x * 0.5f)),
              (util::bottom(t_rect) - collRect.size.y) });

        m_sprite.move({ 0.0f, -(collRect.size.y * 0.35f) });

        if (!m_isFacingRight)
        {
            m_sprite.scale({ -1.0f, 1.0f });
            m_sprite.move({ collRect.size.x, 0.0f });
        }
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

    void Zombie::update(const Context & t_context, const float t_frameTimeSec)
    {
        m_animElpasedTimeSec += t_frameTimeSec;
        const float timePerFrameSec{ timePerFrame(m_anim) };
        if (m_animElpasedTimeSec > timePerFrameSec)
        {
            m_animElpasedTimeSec -= timePerFrameSec;

            const auto & textures{ t_context.zombie_textures.textures(m_anim) };
            if (++m_frameIndex >= textures.size())
            {
                m_frameIndex = 0;

                std::size_t animIndex{ static_cast<std::size_t>(m_anim) };
                if (++animIndex >= static_cast<std::size_t>(ZombieAnim::Count))
                {
                    animIndex = 0;
                }

                m_anim = static_cast<ZombieAnim>(animIndex);
            }

            m_sprite.setTexture(textures.at(m_frameIndex), true);
        }
    }

    void Zombie::moveWithMap(const sf::Vector2f & t_move)
    {
        m_sprite.move(t_move);
        m_rect.position += t_move;
    }

    void Zombie::draw(
        const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        if (t_context.layout.wholeRegion().findIntersection(m_sprite.getGlobalBounds()))
        {
            t_target.draw(m_sprite, t_states);
            // util::drawRectangleShape(t_target, collisionRect(), false, sf::Color::Red);
            // util::drawRectangleShape(t_target, m_rect, false, sf::Color::Yellow);
        }
    }

} // namespace halloween
