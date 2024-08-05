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

#include <iostream>

#include <SFML/Graphics/RenderTarget.hpp>

namespace halloween
{

    SpikedBalls::SpikedBalls()
        : m_texture()
        , m_balls()
        , m_speed(0.0f)
        , m_scale(0.0f, 0.0f)
    {
        // probably no more than a dozen of these in any given level
        m_balls.reserve(100);
    }

    void SpikedBalls::setup(const Settings & settings)
    {
        m_speed = settings.spiked_ball_speed;
        m_scale = settings.spiked_ball_scale;

        m_texture.loadFromFile((settings.media_path / "image" / "spiked-ball.png").string());
        m_texture.setSmooth(true);
    }

    void SpikedBalls::add(Context &, const sf::FloatRect & region)
    {
        SpikedBall & ball = m_balls.emplace_back();

        ball.sprite.setTexture(m_texture);
        ball.sprite.setScale(m_scale);
        util::setOriginToCenter(ball.sprite);
        ball.sprite.setPosition(util::center(region));

        ball.is_horizontal = (region.width > region.height);
        if (ball.is_horizontal)
        {
            ball.slider =
                util::SliderOscillator<float, float>(region.left, util::right(region), m_speed);
        }
        else
        {
            ball.slider =
                util::SliderOscillator<float, float>(region.top, util::bottom(region), m_speed);
        }
    }

    void SpikedBalls::clear() { m_balls.clear(); }

    void SpikedBalls::update(Context &, const float frameTimeSec)
    {
        for (SpikedBall & ball : m_balls)
        {
            const float newPosition = ball.slider.update(frameTimeSec);

            if (ball.is_horizontal)
            {
                ball.sprite.setPosition(newPosition, ball.sprite.getPosition().y);
            }
            else
            {
                ball.sprite.setPosition(ball.sprite.getPosition().x, newPosition);
            }
        }
    }

    void
        SpikedBalls::draw(const Context &, sf::RenderTarget & target, sf::RenderStates states) const
    {
        for (const SpikedBall & ball : m_balls)
        {
            target.draw(ball.sprite, states);
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

    bool SpikedBalls::doesAvatarCollideWithAnyAndDie(const sf::FloatRect & avatarRect) const
    {
        for (const SpikedBall & ball : m_balls)
        {
            if (avatarRect.intersects(ball.sprite.getGlobalBounds()))
            {
                return true;
            }
        }

        return false;
    }

} // namespace halloween
