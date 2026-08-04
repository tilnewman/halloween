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
        Slime(const bool t_isMovingLeft, const sf::FloatRect & t_range, const float t_speed)
            : is_alive{ true }
            , is_moving_left{ t_isMovingLeft }
            , texture_index{ 0 }
            , rect{ t_range }
            , speed{ t_speed }
            , sprite{ util::SfmlDefaults::instance().texture() }
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
        explicit SlimeDeathAnim(const sf::Sprite & t_sprite)
            : is_visible{ true }
            , sprite{ t_sprite }
            , scale{ 0.999f }
        {}

        bool is_visible;
        sf::Sprite sprite;
        float scale;
    };

    //

    class Slimes final : public IObjectManager
    {
      public:
        Slimes();
        virtual ~Slimes() final = default;

        bool willDrawBeforeMap() const final { return false; }
        void setup(const Context & t_context) final;

        void
            add(const Context & t_context,
                const sf::FloatRect & t_rect,
                const std::string & t_details) final;
        
        void clear() final;
        void update(const Context & t_context, const float t_frameTimeSec) final;

        void draw(const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states)
            const final;

        void moveWithMap(const sf::Vector2f & t_move) final;
        void collideWithAvatar(const Context &, const sf::FloatRect &) final {}
        
        bool doesAvatarCollideWithAnyAndDie(
            const Context & t_context, const sf::FloatRect & t_avatarRect) final;
        
        void appendCollisions(std::vector<sf::FloatRect> &) const final {}

        const Harm attack(const Context & t_context, const sf::FloatRect & t_attackRect);
        constexpr std::size_t count() const noexcept { return m_slimes.size(); }

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
