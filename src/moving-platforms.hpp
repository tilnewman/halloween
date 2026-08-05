#ifndef MOVING_PLATFORMS_HPP_INCLUDED
#define MOVING_PLATFORMS_HPP_INCLUDED
//
// moving-platforms.hpp
//
#include "sliders.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include <vector>

namespace sf
{
    class RenderTarget;
}

namespace halloween
{

    struct Context;

    //

    struct PlatformAnim
    {
        PlatformAnim(
            const Context & t_context,
            const std::size_t t_uniqueId,
            const sf::Texture & t_texture,
            const sf::FloatRect & t_rect,
            const sf::FloatRect & t_collisionOffsetRect);

        [[nodiscard]] const sf::FloatRect collisionRect() const;

        std::size_t unique_id; // never zero
        bool is_horiz;
        sf::Sprite sprite;
        sf::FloatRect rect;
        sf::FloatRect collision_offset_rect;
        util::SliderOscillator<float> slider;
    };

    //

    struct PlatformMoveId
    {
        PlatformMoveId(const std::size_t t_id, const sf::Vector2f & t_move)
            : id{ t_id }
            , move{ t_move }
        {}

        std::size_t id{ 0 };
        sf::Vector2f move{};
    };

    using MoveIdVec_t = std::vector<PlatformMoveId>;

    //

    struct PlatformRectId
    {
        PlatformRectId(const std::size_t t_id, const sf::FloatRect & t_rect)
            : id{ t_id }
            , rect{ t_rect }
        {}

        std::size_t id{ 0 };
        sf::FloatRect rect{};
    };

    using RectIdVec_t = std::vector<PlatformRectId>;

    //

    class MovingPlatforms
    {
      public:
        MovingPlatforms();
        ~MovingPlatforms() = default;

        void setup(const Context &);
        void teardown() {}
        bool willDrawBeforeMap() const { return false; }
        void clear() { m_anims.clear(); }

        void
            add(const Context & t_context,
                const sf::FloatRect & t_rect,
                const std::string & t_details = "");

        const MoveIdVec_t update(const Context & t_context, const float t_frameTimeSec);
        void moveWithMap(const sf::Vector2f & t_move);

        void collideWithAvatar(const Context & t_context, const sf::FloatRect & t_avatarRect);

        bool doesAvatarCollideWithAnyAndDie(
            const Context & t_context, const sf::FloatRect & t_avatarRect);

        void appendCollisions(RectIdVec_t & t_rectIDs) const;

        void draw(const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states)
            const;

      private:
        std::size_t m_nextUniqueId;
        sf::Texture m_normalTexture;
        sf::Texture m_jungleTexture;
        std::vector<PlatformAnim> m_anims;
    };

} // namespace halloween

#endif // MOVING_PLATFORMS_HPP_INCLUDED
