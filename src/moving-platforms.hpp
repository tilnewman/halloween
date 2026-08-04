#ifndef MOVING_PLATFORMS_HPP_INCLUDED
#define MOVING_PLATFORMS_HPP_INCLUDED
//
// moving-platforms.hpp
//
#include "object-manager.hpp"
#include "sliders.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <vector>

namespace halloween
{

    struct Context;

    //

    struct PlatformAnim
    {
        PlatformAnim(
            const Context & t_context,
            const sf::Texture & t_texture,
            const sf::FloatRect & t_rect);

        [[nodiscard]] inline const sf::FloatRect collisionRect() const;

        bool is_horiz;
        sf::Sprite sprite;
        sf::FloatRect rect;
        util::SliderOscillator<float> slider;
    };

    //

    class MovingPlatforms final : public IObjectManager
    {
      public:
        MovingPlatforms();
        ~MovingPlatforms() final = default;

        bool willDrawBeforeMap() const final { return false; }
        void clear() final { m_anims.clear(); }

        void setup(const Context & t_context) final;

        void
            add(const Context & t_context,
                const sf::FloatRect & t_rect,
                const std::string & t_details = "") final;

        void update(const Context & t_context, const float t_frameTimeSec) final;
        void moveWithMap(const sf::Vector2f & t_move) final;

        void collideWithAvatar(const Context & t_context, const sf::FloatRect & t_avatarRect) final;

        bool doesAvatarCollideWithAnyAndDie(
            const Context & t_context, const sf::FloatRect & t_avatarRect) final;
        
        void appendCollisions(std::vector<sf::FloatRect> & t_rects) const final;

        void draw(const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states)
            const final;

      private:
        sf::Texture m_texture;
        std::vector<PlatformAnim> m_anims;
    };

} // namespace halloween

#endif // MOVING_PLATFORMS_HPP_INCLUDED
