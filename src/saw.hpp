#ifndef SAW_HPP_INCLUDED
#define SAW_HPP_INCLUDED
//
// saw.hpp
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

    struct Saw
    {
        Saw()
            : sprite()
        {}

        sf::Sprite sprite;
    };

    //

    class Saws
    {
      public:
        Saws();

        void setup(const Settings & settings);
        void add(const Context & context, const sf::FloatRect & region);
        void clear();
        void update(const float frameTimeSec);
        void draw(sf::RenderTarget & target, sf::RenderStates states) const;
        void move(const sf::Vector2f & move);
        bool doesAvatarCollideWithAnyAndDie(const sf::FloatRect & avatarRect) const;

      private:
        sf::Texture m_texture;
        sf::Vector2f m_scale;
        float m_rotationSpeed;
        std::vector<Saw> m_saws;
    };

} // namespace halloween

#endif // SAW_HPP_INCLUDED
