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
        explicit SpikedBall(const sf::Texture & t_texture)
            : is_horizontal{ false } // anything works here
            , sprite{ t_texture }
            , slider{}
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
        void setup(const Settings & t_settings) final;
        void add(Context & t_context, const sf::FloatRect & t_region) final;
        void clear() final;
        void update(Context & t_context, const float t_frameTimeSec) final;

        void draw(const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states)
            const final;

        void moveWithMap(const sf::Vector2f & t_move) final;
        void collideWithAvatar(Context &, const sf::FloatRect &) final {}
        bool doesAvatarCollideWithAnyAndDie(const sf::FloatRect & t_avatarRect) const final;
        void appendCollisions(std::vector<sf::FloatRect> &) const final {}

      private:
        sf::Texture m_texture;
        std::vector<SpikedBall> m_balls;
        float m_speed;
        sf::Vector2f m_scale;
    };

} // namespace halloween

#endif // SPIKEDBALL_HPP_INCLUDED
