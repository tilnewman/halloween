#ifndef GHOST_HPP_INCLUDED
#define GHOST_HPP_INCLUDED
//
// ghost.hpp
//
#include <vector>

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace halloween
{
    struct Context;
    struct Settings;

    struct GhostSpawnPoint
    {
        GhostSpawnPoint(const sf::Vector2f & pos, const float timeUntilSpawn)
            : time_remaining_sec(timeUntilSpawn)
            , position(pos)
        {}

        float time_remaining_sec;
        sf::Vector2f position;
    };

    struct Ghost
    {
        Ghost(const float spd)
            : is_alive(true)
            , is_fading_in(true)
            , speed(spd)
            , sprite()
        {}

        bool is_alive;
        bool is_fading_in;
        float speed;
        sf::Sprite sprite;
    };

    //

    class Ghosts
    {
      public:
        Ghosts();

        void setup(const Settings & settings);
        void clearSpawnPoints();
        void addSpawnPoint(const Context & context, const sf::Vector2f & position);
        void update(const Context &, const float frameTimeSec);
        void draw(sf::RenderTarget & target, sf::RenderStates states) const;
        void move(const sf::Vector2f & move);

      private:
        sf::Texture m_texture1;
        sf::Texture m_texture2;
        sf::Texture m_texture3;
        std::vector<GhostSpawnPoint> m_spawnPoints;
        std::vector<Ghost> m_ghosts;
        float m_spawnMinTimeSec;
        float m_spawnMaxTimeSec;
        float m_floatSpeedMin;
        float m_floatSpeedMax;
    };

} // namespace halloween

#endif // GHOST_HPP_INCLUDED
