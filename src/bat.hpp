#ifndef BAT_HPP_INCLUDED
#define BAT_HPP_INCLUDED
//
// bat.hpp
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

    struct BatTextures
    {
        std::vector<sf::Texture> flying{};
        std::vector<sf::Texture> dying{};
    };

    //

    struct Bat
    {
        Bat(const std::size_t batIndex,
            const bool isMovingLeft,
            const sf::FloatRect & range,
            const float spd)
            : is_alive(true)
            , is_moving_left(isMovingLeft)
            , bat_index(batIndex)
            , texture_index(0)
            , rect(range)
            , speed(spd)
            , sprite(util::SfmlDefaults::instance().texture())
        {}

        bool is_alive;
        bool is_moving_left;
        std::size_t bat_index;
        std::size_t texture_index;
        sf::FloatRect rect;
        float speed;
        sf::Sprite sprite;
    };

    //

    struct BatDeathAnim
    {
        BatDeathAnim(const std::size_t batIndex, const sf::Sprite & spr)
            : is_visible(true)
            , bat_index(batIndex)
            , texture_index(0)
            , elapsed_time_sec(0.0f)
            , sprite(spr)
        {}

        bool is_visible;
        std::size_t bat_index;
        std::size_t texture_index;
        float elapsed_time_sec;
        sf::Sprite sprite;
    };

    //

    class Bats : public IObjectManager
    {
      public:
        Bats();
        virtual ~Bats() override = default;

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
        constexpr std::size_t count() const noexcept { return m_bats.size(); }

      private:
        std::size_t m_batCount;
        std::vector<BatTextures> m_textures;
        std::vector<Bat> m_bats;
        float m_timePerTextureSec;
        float m_elapsedTimeSec;
        std::vector<BatDeathAnim> m_deathAnims;
    };

} // namespace halloween

#endif // SLIME_HPP_INCLUDED
