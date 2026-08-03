// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// frog.cpp
//
#include "frog.hpp"

#include "context.hpp"
#include "screen-regions.hpp"
#include "settings.hpp"
#include "sfml-defaults.hpp"
#include "sfml-util.hpp"

#include <SFML/Graphics/Texture.hpp>

namespace halloween
{

    Frog::Frog(const Context & t_context, const sf::FloatRect & t_rect)
        : m_task{ FrogTask::Idle }
        , m_anim{ FrogAnim::Idle }
        , m_sprite{ util::SfmlDefaults::instance().texture() }
        , m_animElapsedSec{ 0.0f }
        , m_frameIndex{ 0 }
        , m_taskElapsedSec{ 0.0f }
        , m_rect{ t_rect }
    {
        m_sprite.setTexture(t_context.frog_textures.textures(m_anim).at(0), true);

        const float scale{ 1.5f };
        m_sprite.setScale({ scale, scale });

        m_sprite.setPosition(
            { util::center(t_rect).x, (util::bottom(t_rect) - m_sprite.getGlobalBounds().size.y) });
    }

    const sf::FloatRect Frog::collisionRect() const { return m_sprite.getGlobalBounds(); }

    const sf::FloatRect Frog::attackRect() const { return collisionRect(); }

    void Frog::update(const Context & t_context, const float t_frameTimeSec) 
    {
        // update animation
        m_animElapsedSec += t_frameTimeSec;
        const float timePerFrameSec{ 0.075f };
        if (m_animElapsedSec > timePerFrameSec)
        {
            m_animElapsedSec -= timePerFrameSec;

            const std::vector<sf::Texture> & textures{ t_context.frog_textures.textures(m_anim) };
            if (++m_frameIndex >= textures.size())
            {
                m_frameIndex = 0;

                std::size_t animIndex{ static_cast<std::size_t>(m_anim) };
                if (++animIndex >= static_cast<std::size_t>(FrogAnim::Count))
                {
                    animIndex = 0;
                }

                m_anim = static_cast<FrogAnim>(animIndex);
            }

            m_sprite.setTexture(textures.at(m_frameIndex));
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
        }
    }

} // namespace halloween
