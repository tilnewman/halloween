#ifndef SLIME_HPP_INCLUDED
#define SLIME_HPP_INCLUDED
//
// slime.hpp
//
#include "object-manager.hpp"
#include "sfml-defaults.hpp"

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

    struct Slime
    {
        Slime(const bool isMovingLeft, const sf::FloatRect & range, const float spd)
            : is_alive(true)
            , is_moving_left(isMovingLeft)
            , texture_index(0)
            , rect(range)
            , speed(spd)
            , sprite(util::SfmlDefaults::instance().texture())
        {}

        bool is_alive;
        bool is_moving_left;
        std::size_t texture_index;
        sf::FloatRect rect;
        float speed;
        sf::Sprite sprite;
    };

    //

    struct SlimeDeathAnim
    {
        SlimeDeathAnim(const sf::Sprite & spr)
            : is_visible(true)
            , sprite(spr)
        {}

        bool is_visible;
        sf::Sprite sprite;
    };

    //

    class Slimes : public IObjectManager
    {
      public:
        Slimes();
        virtual ~Slimes() override = default;

        bool willDrawBeforeMap() const final { return false; }
        void setup(const Settings & settings) final;
        void add(Context & context, const sf::FloatRect & rect) final;
        void clear() final;
        void update(Context & context, const float frameTimeSec) final;
        void draw(const Context & c, sf::RenderTarget & t, sf::RenderStates s) const final;
        void moveWithMap(const sf::Vector2f & move) final;
        void collideWithAvatar(Context &, const sf::FloatRect &) final {}
        bool doesAvatarCollideWithAnyAndDie(const sf::FloatRect & avatarRect) const final;
        void appendCollisions(std::vector<sf::FloatRect> &) const final {}

        bool attack(Context & context, const sf::FloatRect & attackRect);
        std::size_t count() const { return m_slimes.size(); }

      private:
        std::vector<sf::Texture> m_textures;
        std::vector<Slime> m_slimes;
        float m_timePerTextureSec;
        float m_elapsedTimeSec;
        std::size_t m_textureCount;
        std::vector<SlimeDeathAnim> m_deathAnims;
    };

} // namespace halloween

#endif // SLIME_HPP_INCLUDED
