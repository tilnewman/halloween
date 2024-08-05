#ifndef COIN_HPP_INCLUDED
#define COIN_HPP_INCLUDED
//
// coin.hpp
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

    struct Coin
    {
        Coin(const sf::Texture & texture, const sf::Vector2f & position);

        bool is_alive;
        sf::Sprite sprite;
        std::size_t anim_index;
    };

    //

    struct CoinAnim
    {
        CoinAnim();

        bool is_alive;
        sf::Sprite sprite;
    };

    //

    class Coins : public IObjectManager
    {
      public:
        Coins();
        virtual ~Coins() override = default;

        void setup(const Settings & settings) final;
        void add(Context & context, const sf::FloatRect & region) final;
        void clear() final;
        void update(Context & context, const float frameTimeSec) final;
        void draw(const Context & c, sf::RenderTarget & t, sf::RenderStates s) const final;
        void moveWithMap(const sf::Vector2f & move) final;
        void collideWithAvatar(Context & context, const sf::FloatRect & avatarRect) final;
        bool doesAvatarCollideWithAnyAndDie(const sf::FloatRect &) const final { return false; }

      private:
        void addAnimation(const Context & context, const sf::Vector2f & position);
        void updateTextures(const float frameTimeSec);
        void updateAnimations(const float frameTimeSec);

      private:
        sf::Texture m_texture;
        std::vector<sf::IntRect> m_textureCoords;
        const float m_timePerFrame;
        std::vector<Coin> m_coins;
        float m_elapsedTimeSec;
        std::size_t m_textureIndex;
        std::vector<CoinAnim> m_animations;
    };

} // namespace halloween

#endif // COIN_HPP_INCLUDED
