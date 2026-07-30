#ifndef DART_HPP_INCLUDED
#define DART_HPP_INCLUDED
//
// dart.hpp
//
#include "object-manager.hpp"

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

    struct Dart
    {
        explicit Dart(const sf::Texture & t_texture)
            : is_alive{ true }
            , sprite{ t_texture }
        {}

        bool is_alive;
        sf::Sprite sprite;
    };

    //

    struct DartAnim
    {
        explicit DartAnim(const sf::Texture & t_texture)
            : is_alive{ true }
            , sprite{ t_texture }
        {}

        bool is_alive;
        sf::Sprite sprite;
    };

    //

    class Darts final : public IObjectManager
    {
      public:
        Darts();
        virtual ~Darts() final = default;

        bool willDrawBeforeMap() const final { return false; }
        void setup(const Context & t_context) final;

        void
            add(const Context & t_context,
                const sf::FloatRect & t_region,
                const std::string & t_details) final;

        void clear() final;
        void update(const Context & t_context, const float t_frameTimeSec) final;

        void draw(const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states)
            const final;

        void moveWithMap(const sf::Vector2f & t_move) final;
        void collideWithAvatar(const Context & t_context, const sf::FloatRect & t_avatarRect) final;
        bool doesAvatarCollideWithAnyAndDie(const sf::FloatRect &) const final { return false; }
        void appendCollisions(std::vector<sf::FloatRect> &) const final {}

      private:
        sf::Texture m_texture;
        std::vector<Dart> m_darts;
        std::vector<DartAnim> m_dartAnims;
        sf::Vector2f m_scale;
    };

} // namespace halloween

#endif // DART_HPP_INCLUDED
