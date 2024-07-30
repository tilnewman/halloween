#ifndef BLOOD_HPP_INCLUDED
#define BLOOD_HPP_INCLUDED
//
// blood.hpp
//
#include <vector>

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace halloween
{
    struct Context;
    struct Settings;

    //
    class Blood
    {
      public:
        Blood();

        void setup(const Settings & settings);
        void start(Context &, const sf::Vector2f & position, const bool willSplashRight);
        void update(Context &, const float frameTimeSec);
        void draw(sf::RenderTarget & target, sf::RenderStates states) const;

      private:
        sf::Texture m_texture;
        std::vector<sf::IntRect> m_textureCoords1;
        std::vector<sf::IntRect> m_textureCoords2;
        bool m_isUsingFirstAnim;
        const float m_timePerFrame;
        float m_elapsedTimeSec;
        std::size_t m_textureIndex;
        sf::Sprite m_sprite;
        bool m_isFinished;
    };

} // namespace halloween

#endif // BLOOD_HPP_INCLUDED
