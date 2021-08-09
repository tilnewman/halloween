#ifndef COIN_HPP_INCLUDED
#define COIN_HPP_INCLUDED
//
// coin.hpp
//
#include <vector>

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace slnghn
{
    struct Context;
    struct Settings;

    //
    struct Coin
    {
        Coin(const sf::Texture & TEXTURE, const sf::Vector2f & POSITION);

        bool is_alive;
        sf::Sprite sprite;
        std::size_t anim_index;
    };

    //
    class Coins
    {
      public:
        Coins();

        void setup(const Settings & SETTINGS);
        void add(const sf::Vector2f & POSITION);
        void clear();
        void update(Context &, const float FRAME_TIME_SEC);
        void draw(sf::RenderTarget & target, sf::RenderStates states) const;
        void move(const sf::Vector2f & MOVE);
        void collideWithAvatar(Context & context, const sf::FloatRect & AVATAR_RECT);

      private:
        sf::Texture m_texture;
        std::vector<sf::IntRect> m_textureCoords;
        const float m_timePerFrame;
        std::vector<Coin> m_coins;
        float m_elapsedTimeSec;
        std::size_t m_textureIndex;
    };

} // namespace slnghn

#endif // COIN_HPP_INCLUDED
