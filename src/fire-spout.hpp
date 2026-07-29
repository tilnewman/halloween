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
        FireSpout(const sf::Texture & t_spoutTexture, const sf::Texture & t_fireTexture)
            : is_spurting{ false }
            , spout_sprite{ t_spoutTexture }
            , fire_sprite{ t_fireTexture }
            , texture_index{ 0 }
            , elapsed_time_sec{ 0.0f }
            , time_between_spurts_sec{ 0.0f }
        {}

        bool is_spurting;
        sf::Sprite spout_sprite;
        sf::Sprite fire_sprite;
        std::size_t texture_index;
        float elapsed_time_sec;
        float time_between_spurts_sec;
    };

    //

    class FireSpouts final : public IObjectManager
    {
      public:
        FireSpouts();
        virtual ~FireSpouts() final = default;

        bool willDrawBeforeMap() const final { return false; }
        void setup(const Context & t_context) final;
        void add(const Context & t_context, const sf::FloatRect & t_region) final;
        void clear() final;
        void update(const Context & t_context, const float t_frameTimeSec) final;

        void draw(const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states)
            const final;

        void moveWithMap(const sf::Vector2f & t_move) final;
        void collideWithAvatar(const Context &, const sf::FloatRect &) final {}
        bool doesAvatarCollideWithAnyAndDie(const sf::FloatRect & t_avatarRect) const final;
        void appendCollisions(std::vector<sf::FloatRect> &) const final;

      private:
        sf::Texture m_spoutTexture;
        float m_timePerFrame;
        std::vector<FireSpout> m_fireSpouts;
        std::vector<sf::Texture> m_fireTextures;
        float m_timeBetweenSpurtsMinSec;
        float m_timeBetweenSpurtsMaxSec;
    };

} // namespace halloween

#endif // FIRESPOUT_HPP_INCLUDED
