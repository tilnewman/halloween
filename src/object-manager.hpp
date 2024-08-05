#ifndef OBJECTMANAGER_HPP_INCLUDED
#define OBJECTMANAGER_HPP_INCLUDED
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
    struct Settings;

    //

    class IObjectManager
    {
      public:
        virtual ~IObjectManager() = default;

        virtual void clear() = 0;
        virtual void setup(const Settings & settings) = 0;
        virtual void add(Context & context, const sf::FloatRect & region) = 0;
        virtual void update(Context & context, const float frameTimeSec) = 0;
        virtual void draw(const Context & c, sf::RenderTarget & t, sf::RenderStates s) const = 0;
        virtual void moveWithMap(const sf::Vector2f & move) = 0;
        virtual void collideWithAvatar(Context & context, const sf::FloatRect & avatarRect) = 0;
        virtual bool doesAvatarCollideWithAnyAndDie(const sf::FloatRect & avatarRect) const = 0;
    };

    //

    class ObjectManagerList
    {
      public:
        ObjectManagerList();

        void add(IObjectManager & manager);

        void clearAll();
        void setupAll(const Settings & settings);
        void updateAll(Context & context, const float frameTimeSec);
        void drawAll(const Context & c, sf::RenderTarget & t, sf::RenderStates s) const;
        void moveAllWithMap(const sf::Vector2f & move);
        void collideAllWithAvatar(Context & context, const sf::FloatRect & avatarRect);
        bool doesAvatarCollideWithAnyAndDie(const sf::FloatRect & avatarRect) const;

      private:
        std::vector<std::reference_wrapper<IObjectManager>> m_managers;
    };

} // namespace halloween

#endif // OBJECTMANAGER_HPP_INCLUDED
