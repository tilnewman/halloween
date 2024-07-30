#ifndef COIN_HPP_INCLUDED
#define COIN_HPP_INCLUDED
//
// coin.hpp
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

    struct Coin
    {
        Coin(const sf::Texture & texture, const sf::Vector2f & position);

        bool is_alive;
        sf::Sprite sprite;
        std::size_t anim_index;
    };

    //

    class Coins
    {
      public:
        Coins();

        void setup(const Settings & settings);
        void add(const sf::Vector2f & position);
        void clear();
        void update(Context &, const float frameTimeSec);
        void draw(sf::RenderTarget & target, sf::RenderStates states) const;
        void move(const sf::Vector2f & move);
        void collideWithAvatar(Context & context, const sf::FloatRect & avatarRect);

      private:
        sf::Texture m_texture;
        std::vector<sf::IntRect> m_textureCoords;
        const float m_timePerFrame;
        std::vector<Coin> m_coins;
        float m_elapsedTimeSec;
        std::size_t m_textureIndex;
    };

} // namespace halloween

#endif // COIN_HPP_INCLUDED
