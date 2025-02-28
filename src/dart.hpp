#ifndef DART_HPP_INCLUDED
#define DART_HPP_INCLUDED
//
// dart.hpp
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

    struct Dart
    {
        Dart(const sf::Texture & texture)
            : is_alive(true)
            , sprite(texture)
        {}

        bool is_alive;
        sf::Sprite sprite;
    };

    //

    struct DartAnim
    {
        DartAnim(const sf::Texture & texture)
            : is_alive(true)
            , sprite(texture)
        {}

        bool is_alive;
        sf::Sprite sprite;
    };

    //

    class Darts : public IObjectManager
    {
      public:
        Darts();
        virtual ~Darts() override = default;

        bool willDrawBeforeMap() const final { return false; }
        void setup(const Settings & settings) final;
        void add(Context & context, const sf::FloatRect & region) final;
        void clear() final;
        void update(Context & context, const float frameTimeSec) final;
        void draw(const Context & c, sf::RenderTarget & t, sf::RenderStates s) const final;
        void moveWithMap(const sf::Vector2f & move) final;
        void collideWithAvatar(Context & context, const sf::FloatRect & avatarRect) final;
        bool doesAvatarCollideWithAnyAndDie(const sf::FloatRect &) const final { return false; }
        void appendCollisions(std::vector<sf::FloatRect> &) const final {}

      private:
        sf::Texture m_texture;
        std::vector<Dart> m_darts;
        std::vector<DartAnim> m_dartAnims;
        sf::Vector2f m_scale;
    };

} // namespace halloween

#endif // DART_HPP_INCLUDED
