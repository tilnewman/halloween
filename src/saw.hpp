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
        Saw(const sf::Texture & texture)
            : sprite(texture)
        {}

        sf::Sprite sprite;
    };

    //

    class Saws : public IObjectManager
    {
      public:
        Saws();
        virtual ~Saws() override = default;

        bool willDrawBeforeMap() const final { return true; }
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
        sf::Vector2f m_scale;
        float m_rotationSpeed;
        std::vector<Saw> m_saws;
    };

} // namespace halloween

#endif // SAW_HPP_INCLUDED
