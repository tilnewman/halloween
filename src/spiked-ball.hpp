#ifndef SPIKEDBALL_HPP_INCLUDED
#define SPIKEDBALL_HPP_INCLUDED
//
// spiked-ball.hpp
//
#include "object-manager.hpp"
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
        explicit SpikedBall(const sf::Texture & texture)
            : is_horizontal(false) // anything works here
            , sprite(texture)
            , slider()
        {}

        bool is_horizontal;
        sf::Sprite sprite;
        util::SliderOscillator<float, float> slider;
    };

    //

    class SpikedBalls : public IObjectManager
    {
      public:
        SpikedBalls();
        virtual ~SpikedBalls() override = default;

        bool willDrawBeforeMap() const final { return false; }
        void setup(const Settings & settings) final;
        void add(Context & context, const sf::FloatRect & region) final;
        void clear() final;
        void update(Context & context, const float frameTimeSec) final;
        void draw(const Context & c, sf::RenderTarget & t, sf::RenderStates s) const final;
        void moveWithMap(const sf::Vector2f & move) final;
        void collideWithAvatar(Context &, const sf::FloatRect &) final {}
        bool doesAvatarCollideWithAnyAndDie(const sf::FloatRect & avatarRect) const final;
        void appendCollisions(std::vector<sf::FloatRect> &) const final {}

      private:
        sf::Texture m_texture;
        std::vector<SpikedBall> m_balls;
        float m_speed;
        sf::Vector2f m_scale;
    };

} // namespace halloween

#endif // SPIKEDBALL_HPP_INCLUDED
