#ifndef FIRESPOUT_HPP_INCLUDED
#define FIRESPOUT_HPP_INCLUDED
//
// fire-spout.hpp
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

    struct FireSpout
    {
        FireSpout(const sf::Texture & spoutTexture, const sf::Texture & fireTexture)
            : is_spurting(false)
            , spout_sprite(spoutTexture)
            , fire_sprite(fireTexture)
            , texture_index(0)
            , elapsed_time_sec(0.0f)
            , time_between_spurts_sec(0.0f)
        {}

        bool is_spurting;
        sf::Sprite spout_sprite;
        sf::Sprite fire_sprite;
        std::size_t texture_index;
        float elapsed_time_sec;
        float time_between_spurts_sec;
    };

    //

    class FireSpouts : public IObjectManager
    {
      public:
        FireSpouts();
        virtual ~FireSpouts() override = default;

        bool willDrawBeforeMap() const final { return false; }
        void setup(const Settings & settings) final;
        void add(Context & context, const sf::FloatRect & region) final;
        void clear() final;
        void update(Context & context, const float frameTimeSec) final;
        void draw(const Context & c, sf::RenderTarget & t, sf::RenderStates s) const final;
        void moveWithMap(const sf::Vector2f & move) final;
        void collideWithAvatar(Context &, const sf::FloatRect &) final {}
        bool doesAvatarCollideWithAnyAndDie(const sf::FloatRect & avatarRect) const final;
        void appendCollisions(std::vector<sf::FloatRect> &) const final;

      private:
        sf::Texture m_spoutTexture;
        float m_timePerFrame;
        std::vector<FireSpout> m_fireSpouts;
        std::vector<sf::Texture> m_fireTextures;
        float m_timebetweenSpurtsMinSec;
        float m_timebetweenSpurtsMaxSec;
    };

} // namespace halloween

#endif // FIRESPOUT_HPP_INCLUDED
