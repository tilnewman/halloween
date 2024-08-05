#ifndef SPIKEDBALL_HPP_INCLUDED
#define SPIKEDBALL_HPP_INCLUDED
//
// spiked-ball.hpp
//
#include "sliders.hpp"

#include <vector>

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace halloween
{
    struct Context;
    struct Settings;

    //

    struct SpikedBall
    {
        SpikedBall()
            : is_horizontal(false) // anything works here
            , sprite()
            , slider()
        {}

        bool is_horizontal;
        sf::Sprite sprite;
        util::SliderOscillator<float, float> slider;
    };

    //

    class SpikedBalls
    {
      public:
        SpikedBalls();

        void setup(const Settings & settings);
        void add(Context & context, const sf::FloatRect & region);
        void clear();
        void update(const float frameTimeSec);
        void draw(sf::RenderTarget & target, sf::RenderStates states) const;
        void move(const sf::Vector2f & move);
        bool doesAvatarCollideWithAnyAndDie(const sf::FloatRect & avatarRect) const;

      private:
        sf::Texture m_texture;
        std::vector<SpikedBall> m_balls;
        float m_speed;
        sf::Vector2f m_scale;
    };

} // namespace halloween

#endif // SPIKEDBALL_HPP_INCLUDED
