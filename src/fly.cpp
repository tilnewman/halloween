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

namespace halloween
{

    Fly::Fly(const Context & t_context, const sf::FloatRect & t_rect)
        : m_type{ t_context.random.from(
              { FlyType::Beholder, FlyType::Chomp, FlyType::Face, FlyType::Horn, FlyType::Peek }) }
        , m_anim{ FlyAnim::Fly }
        , m_task{ FlyTask::Wander }
        , m_sprite{ t_context.fly_textures.textures(m_type, m_anim).at(0) }
        , m_animElapsedSec{ 0.0f }
        , m_rect{ t_rect }
        , m_frameIndex{ 0 }
        , m_hitPoints{ 1 }
        , m_isFacingRight{ t_context.random.boolean() }
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

    void Fly::update(const Context & t_context, const float m_frameTimeSec) 
    {
        m_animElapsedSec += m_frameTimeSec;
        const float timePerFrameSec{ 0.08f };
        if (m_animElapsedSec > timePerFrameSec)
        {
            m_animElapsedSec -= timePerFrameSec;

            const auto & textures{ t_context.fly_textures.textures(m_type, m_anim) };
            if (++m_frameIndex >= textures.size())
            {
                m_frameIndex = 0;

                std::size_t animIndex{ static_cast<std::size_t>(m_anim) };
                if (++animIndex >= static_cast<std::size_t>(FlyAnim::Count))
                {
                    animIndex = 0;
                }
                m_anim = static_cast<FlyAnim>(animIndex);
            }

            m_sprite.setTexture(textures.at(m_frameIndex), true);
        }
    }

    void Fly::moveWithMap(const sf::Vector2f & t_move)
    {
        m_sprite.move(t_move);
        m_rect.position += t_move;
    }

    [[nodiscard]] bool Fly::doesAvatarCollideWithAnyAndDie(const Context &, const sf::FloatRect &)
    {
        return false;
    }

    [[nodiscard]] const sf::FloatRect Fly::collisionRect() const
    {
        return util::scaleRectInPlaceCopy(m_sprite.getGlobalBounds(), 0.75f);
    }

    void Fly::draw(
        const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        if (t_context.layout.wholeRegion().findIntersection(collisionRect()))
        {
            t_target.draw(m_sprite, t_states);
        }
    }

    const Harm Fly::hit(const Context &) { return {}; }

} // namespace halloween
