#ifndef FLOATING_SPIRIT_HPP_INCLUDED
#define FLOATING_SPIRIT_HPP_INCLUDED
//
// floating-spirit.hpp
//
#include "random.hpp"
#include "sfml-sliders.hpp"

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace halloween
{
    struct Context;
    struct Settings;

    class FloatingSpirit
    {
      public:
        FloatingSpirit();

        void setup(const Context & context);
        inline const sf::FloatRect globalBounds() const { return m_sprite.getGlobalBounds(); }
        void update(const Context & context, const float frameTimeSec);
        void draw(sf::RenderTarget & target, sf::RenderStates states) const;

      private:
        sf::Texture m_texture;
        sf::Sprite m_sprite;
        util::PositionDrifter m_drifter;
    };

} // namespace halloween

#endif // FLOATING_SPIRIT_HPP_INCLUDED
