#ifndef OBJECT_MANAGER_HPP_INCLUDED
#define OBJECT_MANAGER_HPP_INCLUDED
//
// object-manager.hpp
//
#include <functional>
#include <vector>

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace halloween
{
    struct Context;

    //

    struct Harm
    {
        [[nodiscard]] constexpr bool wasAnyHarm() const noexcept { return (did_hit or did_kill); }
        bool did_hit{ false };
        bool did_kill{ false };
    };

    //

    struct IObjectManager
    {
        virtual ~IObjectManager() = default;

        virtual bool willDrawBeforeMap() const = 0;
        virtual void clear() = 0;
        virtual void setup(const Context & t_context) = 0;

        virtual void
            add(const Context & t_context,
                const sf::FloatRect & t_region,
                const std::string & t_details = "") = 0;

        virtual void update(const Context & t_context, const float t_frameTimeSec) = 0;
        virtual void moveWithMap(const sf::Vector2f & t_move) = 0;

        virtual void
            collideWithAvatar(const Context & t_context, const sf::FloatRect & t_avatarRect) = 0;

        virtual bool doesAvatarCollideWithAnyAndDie(const sf::FloatRect & t_avatarRect) const = 0;
        virtual void appendCollisions(std::vector<sf::FloatRect> & t_rects) const = 0;

        virtual void draw(
            const Context & t_context,
            sf::RenderTarget & t_target,
            sf::RenderStates t_states) const = 0;
    };

    //

    class ObjectManagerList
    {
      public:
        ObjectManagerList();

        void add(IObjectManager & t_manager);

        void clearAll();
        void setupAll(const Context & t_context);
        void updateAll(const Context & t_context, const float t_frameTimeSec);

        void drawAllBeforeMap(
            const Context & t_context,
            sf::RenderTarget & t_target,
            sf::RenderStates t_states) const;

        void drawAllAfterMap(
            const Context & t_context,
            sf::RenderTarget & t_target,
            sf::RenderStates t_states) const;

        void moveAllWithMap(const sf::Vector2f & t_move);
        void collideAllWithAvatar(const Context & t_context, const sf::FloatRect & t_avatarRect);
        bool doesAvatarCollideWithAnyAndDie(const sf::FloatRect & t_avatarRect) const;
        void appendAllCollisions(std::vector<sf::FloatRect> & t_rects) const;

      private:
        std::vector<std::reference_wrapper<IObjectManager>> m_managers;
    };

} // namespace halloween

#endif // OBJECT_MANAGER_HPP_INCLUDED
