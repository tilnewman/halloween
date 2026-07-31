//
// moving-platforms.cpp
//
#include "moving-platforms.hpp"

#include "context.hpp"
#include "random.hpp"
#include "settings.hpp"
#include "sfml-util.hpp"
#include "texture-loader.hpp"

namespace halloween
{

    PlatformAnim::PlatformAnim(
        const Context & t_context,
        const PlatformMoveType t_type,
        const sf::Texture & t_texture,
        const sf::FloatRect & t_rect)
        : type{ t_type }
        , sprite{ t_texture }
        , elapsed_time_sec{ 0.0f }
        , is_facing_right{ t_context.random.boolean() }
        , rect{ t_rect }
        , horiz_slider{ 1.0f }
        , vert_slider{ 1.0f }
    {
        if (!is_facing_right)
        {
            sprite.scale({ -1.0f, 1.0f });
            sprite.move({ sprite.getGlobalBounds().size.x, 0.0f });
        }

        if (PlatformMoveType::Horizontal == type)
        {
            sprite.setPosition(
                { t_rect.position.x,
                  (util::center(t_rect).y - (sprite.getGlobalBounds().size.y * 0.5f)) });
        }
        else if (PlatformMoveType::Vertical == type)
        {
            sprite.setPosition(
                { (util::center(t_rect).x - (sprite.getGlobalBounds().size.y * 0.5f)),
                  util::bottom(t_rect) });
        }
        else
        {
            sprite.setPosition(
                { t_rect.position.x,
                  (util::center(t_rect).y - (sprite.getGlobalBounds().size.y * 0.5f)) });
        }
    }

    const sf::FloatRect PlatformAnim::collisionRect() const
    {
        sf::FloatRect collRect(sprite.getGlobalBounds());
        collRect.position += { 8.0f, 13.0f };
        collRect.size = { 113.0f, 38.0f };
        return collRect;
    }

    //

    MovingPlatforms::MovingPlatforms()
        : m_texture{}
        , m_anims{}
    {
        m_anims.reserve(32); // just a harmless guess
    }

    void MovingPlatforms::setup(const Context & t_context)
    {
        util::TextureLoader::load(
            m_texture, (t_context.settings.media_path / "image" / "platform.png"));
    }

    void MovingPlatforms::add(
        const Context & t_context, const sf::FloatRect & t_rect, const std::string &)
    {
        const PlatformMoveType type = [&]() {
            if (t_rect.size.x > t_rect.size.y)
            {
                return PlatformMoveType::Horizontal;
            }
            else if (t_rect.size.y > t_rect.size.x)
            {
                return PlatformMoveType::Vertical;
            }
            else
            {
                return PlatformMoveType::Circular;
            }
        }();

        m_anims.emplace_back(t_context, type, m_texture, t_rect);
    }

    void MovingPlatforms::update(const Context & t_context, const float t_frameTimeSec)
    {
        for (PlatformAnim & anim : m_anims)
        {
            if (PlatformMoveType::Horizontal == anim.type)
            {
            }
            else if (PlatformMoveType::Vertical == anim.type)
            {
            }
            else if (PlatformMoveType::Circular == anim.type)
            {
            }
        }
    }

    void MovingPlatforms::moveWithMap(const sf::Vector2f & t_move)
    {
        for (PlatformAnim & anim : m_anims)
        {
            anim.sprite.move(t_move);
            anim.rect.position += t_move;
        }
    }

    void MovingPlatforms::collideWithAvatar(
        const Context & t_context, const sf::FloatRect & t_avatarRect)
    {}

    bool MovingPlatforms::doesAvatarCollideWithAnyAndDie(const sf::FloatRect & t_avatarRect) const
    {
        return false;
    }

    void MovingPlatforms::appendCollisions(std::vector<sf::FloatRect> & t_rects) const
    {
        for (const PlatformAnim & anim : m_anims)
        {
            t_rects.push_back(anim.collisionRect());
        }
    }

    void MovingPlatforms::draw(
        const Context &, sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        for (const PlatformAnim & anim : m_anims)
        {
            t_target.draw(anim.sprite, t_states);
        }
    }

} // namespace halloween