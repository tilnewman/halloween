#ifndef BLOOD_HPP_INCLUDED
#define BLOOD_HPP_INCLUDED
//
// blood.hpp
//
#include <vector>

#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace sf
{
    class RenderTarget;
}

namespace halloween
{
    struct Context;
    struct Settings;

    //

    class Blood
    {
      public:
        Blood();

        void setup(const Settings & t_settings);

        void start(
            const Context & t_context,
            const sf::Vector2f & t_position,
            const bool t_willSplashRight);

        void update(const float t_frameTimeSec);
        void draw(sf::RenderTarget & t_target, sf::RenderStates t_states) const;

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
