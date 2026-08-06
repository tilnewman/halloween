#ifndef NINJAS_HPP_INCLUDED
#define NINJAS_HPP_INCLUDED
//
// ninjas.hpp
//
#include "object-manager.hpp"

#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include <vector>

namespace sf
{
    class RenderTarget;
}

namespace halloween
{

    struct Context;

    struct Ninja
    {
        Ninja(
            const Context & t_context, const sf::Texture & t_texture, const sf::FloatRect & t_rect);

        void turnToFace(const sf::Vector2f & t_position);

        sf::Sprite sprite;
        float anim_elapsed_sec;
        std::size_t frame_index;
        bool is_facing_right;
    };

    class NinjaObjectManager final : public IObjectManager
    {
      public:
        NinjaObjectManager();
        ~NinjaObjectManager() final = default;

        void setup(const Context & t_context) final;
        void teardown() final { m_textures.clear(); }
        bool willDrawBeforeMap() const final { return false; }
        void clear() final { m_ninjas.clear(); }

        void
            add(const Context & t_context,
                const sf::FloatRect & t_region,
                const std::string & t_details = "") final;

        const Harm attack(const Context & t_context, const sf::FloatRect & t_attackRect);
        void update(const Context & t_context, const float t_frameTimeSec) final;
        void moveWithMap(const sf::Vector2f & t_move) final;
        void collideWithAvatar(const Context &, const sf::FloatRect &) final {}

        bool doesAvatarCollideWithAnyAndDie(
            const Context & t_context, const sf::FloatRect & t_avatarRect) final;

        void appendCollisions(std::vector<sf::FloatRect> & t_rects) const final;

        void draw(const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states)
            const final;

      private:
        std::vector<Ninja> m_ninjas;
        std::vector<sf::Texture> m_textures;
    };

} // namespace halloween

#endif // NINJAS_HPP_INCLUDED
