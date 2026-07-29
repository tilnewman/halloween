#ifndef SAW_HPP_INCLUDED
#define SAW_HPP_INCLUDED
//
// saw.hpp
//
#include "object-manager.hpp"

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

    struct Saw
    {
        explicit Saw(const sf::Texture & t_texture)
            : sprite{ t_texture }
        {}

        sf::Sprite sprite;
    };

    //

    class Saws final : public IObjectManager
    {
      public:
        Saws();
        virtual ~Saws() final = default;

        bool willDrawBeforeMap() const final { return true; }
        void setup(const Context & t_context) final;
        void add(const Context & t_context, const sf::FloatRect & t_region) final;
        void clear() final;
        void update(const Context & t_context, const float t_frameTimeSec) final;

        void draw(const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states)
            const final;

        void moveWithMap(const sf::Vector2f & t_move) final;
        void collideWithAvatar(const Context &, const sf::FloatRect &) final {}
        bool doesAvatarCollideWithAnyAndDie(const sf::FloatRect & t_avatarRect) const final;
        void appendCollisions(std::vector<sf::FloatRect> &) const final {}

      private:
        sf::Texture m_texture;
        sf::Vector2f m_scale;
        float m_rotationSpeed;
        std::vector<Saw> m_saws;
    };

} // namespace halloween

#endif // SAW_HPP_INCLUDED
