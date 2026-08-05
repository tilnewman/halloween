//
// moving-platforms.cpp
//
#include "moving-platforms.hpp"

#include "check-macros.hpp"
#include "context.hpp"
#include "random.hpp"
#include "screen-regions.hpp"
#include "settings.hpp"
#include "sfml-util.hpp"
#include "texture-loader.hpp"

namespace halloween
{

    PlatformAnim::PlatformAnim(
        const Context & t_context,
        const sf::Texture & t_texture,
        const sf::FloatRect & t_rect,
        const sf::FloatRect & t_collisionOffsetRect)
        : is_horiz{ t_rect.size.x > t_rect.size.y }
        , sprite{ t_texture }
        , rect{ t_rect }
        , collision_offset_rect{ t_collisionOffsetRect }
        , slider{ 0.0f, 1.0f, t_context.random.fromTo(0.2f, 0.35f) }
    {
        if (is_horiz)
        {
            sprite.setPosition(
                { t_rect.position.x,
                  (util::center(t_rect).y - (sprite.getGlobalBounds().size.y * 0.5f)) });
        }
        else
        {
            sprite.setPosition(
                { (util::center(t_rect).x - (sprite.getGlobalBounds().size.y * 0.5f)),
                  t_rect.position.y });
        }

        util::setOriginToCenter(sprite);

        if (t_context.random.boolean())
        {
            sprite.scale({ -1.0f, 1.0f });
        }
    }

    const sf::FloatRect PlatformAnim::collisionRect() const
    {
        sf::FloatRect collRect{ sprite.getGlobalBounds() };
        collRect.position += collision_offset_rect.position;
        collRect.size = collision_offset_rect.size;
        return collRect;
    }

    //

    MovingPlatforms::MovingPlatforms()
        : m_normalTexture{}
        , m_jungleTexture{}
        , m_anims{}
    {
        m_anims.reserve(32); // just a harmless guess
    }

    void MovingPlatforms::setup(const Context & t_context)
    {
        util::TextureLoader::load(
            m_normalTexture, (t_context.settings.media_path / "image" / "platform-normal.png"));

        util::TextureLoader::load(
            m_jungleTexture, (t_context.settings.media_path / "image" / "platform-jungle.png"));
    }

    void MovingPlatforms::add(
        const Context & t_context, const sf::FloatRect & t_rect, const std::string & t_details)
    {
        if ("normal" == t_details)
        {
            const sf::FloatRect collisionOffsetRect({ 0.0f, 0.0f }, { 144.0f, 50.0f });
            m_anims.emplace_back(t_context, m_normalTexture, t_rect, collisionOffsetRect);
        }
        else if ("jungle" == t_details)
        {
            const sf::FloatRect collisionOffsetRect({ 8.0f, 13.0f }, { 113.0f, 38.0f });
            m_anims.emplace_back(t_context, m_jungleTexture, t_rect, collisionOffsetRect);
        }
        else
        {
            M_LOG("Error:  Unknown moving platform name found in map file \"" << t_details << "\"");
        }
    }

    void MovingPlatforms::update(const Context &, const float t_elapsedTimeSec)
    {
        for (PlatformAnim & anim : m_anims)
        {
            if (anim.is_horiz)
            {
                const float span{ anim.rect.size.x - anim.sprite.getGlobalBounds().size.x };
                const float ratio{ anim.slider.update(t_elapsedTimeSec) };

                const float posHoriz{ (
                    anim.rect.position.x + (anim.sprite.getGlobalBounds().size.x * 0.5f) +
                    (ratio * span)) };

                anim.sprite.setPosition({ posHoriz, anim.sprite.getPosition().y });
            }
            else
            {
                const float span{ anim.rect.size.y - anim.sprite.getGlobalBounds().size.y };
                const float ratio{ anim.slider.update(t_elapsedTimeSec) };

                const float posVert{ anim.rect.position.y +
                                     (anim.sprite.getGlobalBounds().size.y * 0.5f) +
                                     (ratio * span) };

                anim.sprite.setPosition({ anim.sprite.getPosition().x, posVert });
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

    void MovingPlatforms::collideWithAvatar(const Context &, const sf::FloatRect &) {}

    bool MovingPlatforms::doesAvatarCollideWithAnyAndDie(const Context &, const sf::FloatRect &)
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
        const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        if (m_anims.empty())
        {
            return;
        }

        const sf::FloatRect wholeRect{ t_context.layout.wholeRegion() };

        for (const PlatformAnim & anim : m_anims)
        {
            if (wholeRect.findIntersection(anim.sprite.getGlobalBounds()))
            {
                t_target.draw(anim.sprite, t_states);
            }
        }
    }

} // namespace halloween