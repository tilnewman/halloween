#ifndef SMOKE_HPP_INCLUDED
#define SMOKE_HPP_INCLUDED
//
// smoke.hpp
//
#include "object-manager.hpp"

#include <string>
#include <vector>

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace halloween
{
    struct Context;
    struct Settings;

    //

    enum class SmokeType
    {
        Full,
        Still,
        Top
    };

    //

    struct SmokeDetails
    {
        explicit SmokeDetails(const std::string & t_details);

        SmokeType type;
        std::uint8_t alpha;
    };

    //

    struct SmokeAnim
    {
        SmokeAnim(
            const SmokeType t_type,
            const sf::Sprite & t_sprite,
            const std::size_t t_frameIndex,
            const sf::FloatRect & t_rect,
            const bool t_isFacingRight)
            : type{ t_type }
            , sprite{ t_sprite }
            , elapsed_time_sec{ 0.0f }
            , frame_index{ t_frameIndex }
            , rect{ t_rect }
            , is_facing_right{ t_isFacingRight }
        {}

        SmokeType type;
        sf::Sprite sprite;
        float elapsed_time_sec;
        std::size_t frame_index;
        sf::FloatRect rect;
        bool is_facing_right;
    };

    //

    class Smoke final : public IObjectManager
    {
      public:
        Smoke();
        ~Smoke() final = default;

        void setup(const Context &) final;
        void teardown() final {}
        bool willDrawBeforeMap() const final { return true; }

        void
            add(const Context & t_context,
                const sf::FloatRect & t_region,
                const std::string & t_details) final;

        constexpr void clear() noexcept final { m_animations.clear(); }
        void update(const Context & t_context, const float t_frameTimeSec) final;

        void draw(const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states)
            const final;

        void moveWithMap(const sf::Vector2f & t_move) final;
        void collideWithAvatar(const Context &, const sf::FloatRect &) final {}

        bool doesAvatarCollideWithAnyAndDie(const Context &, const sf::FloatRect &) final
        {
            return false;
        }

        void appendCollisions(std::vector<sf::FloatRect> &) const final {}
        constexpr std::size_t count() const noexcept { return m_animations.size(); }

      private:
        std::vector<sf::Texture> m_textures;
        std::vector<SmokeAnim> m_animations;
    };

} // namespace halloween

#endif // SMOKE_HPP_INCLUDED
