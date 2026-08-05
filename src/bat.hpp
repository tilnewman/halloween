#ifndef BAT_HPP_INCLUDED
#define BAT_HPP_INCLUDED
//
// bat.hpp
//
#include "object-manager.hpp"
#include "sfml-defaults.hpp"

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

    //

    struct BatTextures
    {
        std::vector<sf::Texture> flying{};
        std::vector<sf::Texture> dying{};
    };

    //

    struct Bat
    {
        Bat(const std::size_t t_batIndex,
            const bool t_isMovingLeft,
            const sf::FloatRect & t_range,
            const float t_speed)
            : is_alive{ true }
            , is_moving_left{ t_isMovingLeft }
            , bat_index{ t_batIndex }
            , texture_index{ 0 }
            , rect{ t_range }
            , speed{ t_speed }
            , sprite{ util::SfmlDefaults::instance().texture() }
            , has_spotted_player{ false }
            , elpased_time_sec{ 0.0f }
        {}

        bool is_alive;
        bool is_moving_left;
        std::size_t bat_index;
        std::size_t texture_index;
        sf::FloatRect rect;
        float speed;
        sf::Sprite sprite;
        bool has_spotted_player;
        float elpased_time_sec;
    };

    //

    struct BatDeathAnim
    {
        BatDeathAnim(const std::size_t t_batIndex, const sf::Sprite & t_sprite)
            : is_visible{ true }
            , bat_index{ t_batIndex }
            , texture_index{ 0 }
            , death_elapsed_time_sec{ 0.0f }
            , sprite{ t_sprite }
            , scale{ 0.985f }
        {}

        bool is_visible;
        std::size_t bat_index;
        std::size_t texture_index;
        float death_elapsed_time_sec;
        sf::Sprite sprite;
        float scale;
    };

    //

    class Bats final : public IObjectManager
    {
      public:
        Bats();
        ~Bats() final = default;

        void setup(const Context &) final;
        void teardown() final {}
        bool willDrawBeforeMap() const final { return false; }

        void
            add(const Context & t_context,
                const sf::FloatRect & t_rect,
                const std::string & t_details) final;

        void clear() final;
        void update(const Context & t_context, const float t_frameTimeSec) final;

        void draw(const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states)
            const final;

        void moveWithMap(const sf::Vector2f &) final;
        void collideWithAvatar(const Context &, const sf::FloatRect &) final {}

        bool doesAvatarCollideWithAnyAndDie(
            const Context & t_context, const sf::FloatRect & t_avatarRect) final;

        void appendCollisions(std::vector<sf::FloatRect> &) const final {}

        const Harm attack(const Context & t_context, const sf::FloatRect & t_attackRect);
        constexpr std::size_t count() const noexcept { return m_bats.size(); }

      private:
        std::size_t m_batCount;
        std::vector<BatTextures> m_textures;
        std::vector<Bat> m_bats;
        float m_timePerFrameBeforeSec;
        float m_timePerFrameAfterSec;
        std::vector<BatDeathAnim> m_deathAnims;
    };

} // namespace halloween

#endif // SLIME_HPP_INCLUDED
