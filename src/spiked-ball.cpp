// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// spiked-ball.cpp
//
#include "spiked-ball.hpp"

#include "context.hpp"
#include "info-region.hpp"
#include "level.hpp"
#include "screen-regions.hpp"
#include "settings.hpp"
#include "sfml-util.hpp"
#include "sound-player.hpp"
#include "texture-loader.hpp"

#include <iostream>

#include <SFML/Graphics/RenderTarget.hpp>

namespace halloween
{

    SpikedBalls::SpikedBalls()
        : m_texture{}
        , m_balls{}
        , m_speed{ 0.0f }
        , m_scale{}
    {
        // probably no more than a dozen of these in any given level
        m_balls.reserve(100);
    }

    void SpikedBalls::setup(const Context & t_context)
    {
        m_speed = t_context.random.fromTo(0.75f, 1.25f);
        m_scale = t_context.settings.spiked_ball_scale;

        util::TextureLoader::load(
            m_texture, (t_context.settings.media_path / "image" / "spiked-ball.png"), true);
    }

    void SpikedBalls::add(const Context &, const sf::FloatRect & t_region)
    {
        SpikedBall & ball{ m_balls.emplace_back(m_texture) };
        ball.sprite.setScale(m_scale);
        util::setOriginToCenter(ball.sprite);
        ball.sprite.setPosition(util::center(t_region));

        ball.is_horizontal = (t_region.size.x > t_region.size.y);
        if (ball.is_horizontal)
        {
            ball.slider = util::SliderOscillator<float, float>(
                t_region.position.x, util::right(t_region), m_speed);
        }
        else
        {
            ball.slider = util::SliderOscillator<float, float>(
                t_region.position.y, util::bottom(t_region), m_speed);
        }
    }

    void SpikedBalls::clear() { m_balls.clear(); }

    void SpikedBalls::update(const Context &, const float t_frameTimeSec)
    {
        for (SpikedBall & ball : m_balls)
        {
            const float newPosition{ ball.slider.update(t_frameTimeSec) };

            if (ball.is_horizontal)
            {
                ball.sprite.setPosition({ newPosition, ball.sprite.getPosition().y });
            }
            else
            {
                ball.sprite.setPosition({ ball.sprite.getPosition().x, newPosition });
            }
        }
    }

    void SpikedBalls::draw(
        const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        const sf::FloatRect mapRect{ t_context.layout.mapRegion() };

        for (const SpikedBall & ball : m_balls)
        {
            if (mapRect.findIntersection(ball.sprite.getGlobalBounds()))
            {
                t_target.draw(ball.sprite, t_states);
            }
        }
    }

    void SpikedBalls::moveWithMap(const sf::Vector2f & move)
    {
        for (SpikedBall & ball : m_balls)
        {
            ball.sprite.move(move);

            if (ball.is_horizontal)
            {
                ball.slider = util::SliderOscillator<float, float>(
                    (ball.slider.from() + move.x),
                    (ball.slider.to() + move.x),
                    m_speed,
                    (ball.slider.value() + move.x));
            }
        }
    }

    bool SpikedBalls::doesAvatarCollideWithAnyAndDie(const sf::FloatRect & t_avatarRect) const
    {
        for (const SpikedBall & ball : m_balls)
        {
            if (t_avatarRect.findIntersection(ball.sprite.getGlobalBounds()))
            {
                return true;
            }
        }

        return false;
    }

} // namespace halloween
