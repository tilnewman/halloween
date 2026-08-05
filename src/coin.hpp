#ifndef COIN_HPP_INCLUDED
#define COIN_HPP_INCLUDED
//
// coin.hpp
//
#include "object-manager.hpp"

#include <vector>

#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace sf
{
    class RenderTarget;
}

namespace halloween
{
    struct Context;
    struct Settings;

    struct Coin
    {
        Coin(const sf::Texture & t_texture, const sf::Vector2f & t_position);

        bool is_alive;
        sf::Sprite sprite;
        std::size_t anim_index;
    };

    //

    struct CoinAnim
    {
        CoinAnim(const sf::Texture & t_texture);

        bool is_alive;
        sf::Sprite sprite;
    };

    //

    class Coins final : public IObjectManager
    {
      public:
        Coins();
        virtual ~Coins() final = default;

        void setup(const Context &) final;
        void teardown() final {}
        bool willDrawBeforeMap() const final { return false; }

        void
            add(const Context & t_context,
                const sf::FloatRect & t_region,
                const std::string & t_details) final;

        void clear() final;
        void update(const Context & t_context, const float t_frameTimeSec) final;

        void draw(const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states)
            const final;

        void moveWithMap(const sf::Vector2f & t_move) final;
        void collideWithAvatar(const Context & t_context, const sf::FloatRect & t_avatarRect) final;

        bool doesAvatarCollideWithAnyAndDie(const Context &, const sf::FloatRect &) final
        {
            return false;
        }

        void appendCollisions(std::vector<sf::FloatRect> &) const final {}

        constexpr std::size_t count() const noexcept { return m_coins.size(); }

      private:
        void addAnimation(const Context & t_context, const sf::Vector2f & t_position);
        void updateTextures(const float t_frameTimeSec);
        void updateAnimations(const float t_frameTimeSec);

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
