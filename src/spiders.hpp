#ifndef SPIDERS_HPP_INCLUDED
#define SPIDERS_HPP_INCLUDED
//
// spiders.hpp
//
#include "object-manager.hpp"
#include "spider-textures.hpp"
#include "spider.hpp"

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include <vector>

namespace halloween
{

    struct Context;

    //

    class SpiderObjectManager final : public IObjectManager
    {
      public:
        SpiderObjectManager();
        ~SpiderObjectManager() final = default;

        void setup(const Context & t_context) final { m_textureManager.setup(t_context); }
        void teardown() final { m_textureManager.teardown(); }
        bool willDrawBeforeMap() const final { return false; }
        void clear() final { m_spiders.clear(); }

        void
            add(const Context & t_context,
                const sf::FloatRect & t_region,
                const std::string & t_details = "") final;

        void update(const Context & t_context, const float t_frameTimeSec) final;
        void moveWithMap(const sf::Vector2f & t_move) final;
        void collideWithAvatar(const Context &, const sf::FloatRect &) final {}

        bool doesAvatarCollideWithAnyAndDie(
            const Context & t_context, const sf::FloatRect & t_avatarRect) final;
        
        void appendCollisions(std::vector<sf::FloatRect> & t_rects) const final;
        const Harm attack(const Context & t_context, const sf::FloatRect & t_attackRect);

        void draw(const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states)
            const final;

      private:
        std::vector<Spider> m_spiders;
        SpiderTextureManager m_textureManager;
    };

} // namespace halloween

#endif // SPIDERS_HPP_INCLUDED
