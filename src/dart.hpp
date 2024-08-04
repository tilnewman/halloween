#ifndef DART_HPP_INCLUDED
#define DART_HPP_INCLUDED
//
// dart.hpp
//
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
        Dart()
            : is_alive(true)
            , sprite()
        {}

        bool is_alive;
        sf::Sprite sprite;
    };

    //

    struct DartAnim
    {
        DartAnim()
            : is_alive(true)
            , sprite()
        {}

        bool is_alive;
        sf::Sprite sprite;
    };

    //

    class Darts
    {
      public:
        Darts();

        void setup(const Settings & settings);
        void add(Context & context, const sf::FloatRect & region);
        void clear();
        void update(const float frameTimeSec);
        void draw(sf::RenderTarget & target, sf::RenderStates states) const;
        void move(const sf::Vector2f & move);
        void collideWithAvatar(Context & context, const sf::FloatRect & avatarRect);

      private:
        sf::Texture m_texture;
        std::vector<Dart> m_darts;
        std::vector<DartAnim> m_dartAnims;
        sf::Vector2f m_scale;
    };

} // namespace halloween

#endif // DART_HPP_INCLUDED
