#ifndef FIRE_SPOUT_LARGE_HPP_INCLUDED
#define FIRE_SPOUT_LARGE_HPP_INCLUDED
//
// fire-spout-large.hpp
//
#include "object-manager.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <vector>

//

namespace sf
{
    class RenderTarget;
    struct RenderStates;
} // namespace sf

//

namespace halloween
{
    struct Context;

    //

    struct FireSpoutLargeAnim
    {
        FireSpoutLargeAnim(
            const Context & t_context,
            const sf::Texture & t_spoutTexture,
            const sf::Texture & t_fireTexture,
            const std::size_t t_frameIndex,
            const sf::FloatRect & t_rect);

        inline const sf::FloatRect collisionRect() const;

        sf::Sprite fire_sprite;
        sf::Sprite spout_sprite;
        float elapsed_time_sec;
        std::size_t frame_index;
    };

    //

    class FireSpoutLargeManager final : public IObjectManager
    {
      public:
        FireSpoutLargeManager();

        ~FireSpoutLargeManager() final = default;

        bool willDrawBeforeMap() const final { return false; }
        void clear() final { m_anims.clear(); }

        void setup(const Context & t_context) final;

        void
            add(const Context & t_context,
                const sf::FloatRect & t_region,
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
        sf::Texture m_spoutTexture;
        std::vector<sf::Texture> m_fireTextures;
        std::vector<FireSpoutLargeAnim> m_anims;
    };

} // namespace halloween

#endif // FIRE_SPOUT_LARGE_HPP_INCLUDED
