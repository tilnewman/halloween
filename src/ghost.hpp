#ifndef GHOST_HPP_INCLUDED
#define GHOST_HPP_INCLUDED
//
// ghost.hpp
//
#include "object-manager.hpp"
#include "sfml-defaults.hpp"

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
        GhostSpawnPoint(const sf::Vector2f & t_position, const float t_timeUntilSpawn)
            : time_remaining_sec{ t_timeUntilSpawn }
            , position{ t_position }
        {}

        float time_remaining_sec;
        sf::Vector2f position;
    };

    struct Ghost
    {
        explicit Ghost(const float t_speed)
            : is_alive{ true }
            , is_fading_in{ true }
            , speed{ t_speed }
            , sprite{ util::SfmlDefaults::instance().texture() }
        {}

        bool is_alive;
        bool is_fading_in;
        float speed;
        sf::Sprite sprite;
    };

    //

    class Ghosts : public IObjectManager
    {
      public:
        Ghosts();
        virtual ~Ghosts() override = default;

        bool willDrawBeforeMap() const final { return false; }
        void setup(const Settings & t_settings) final;
        void clear() final;
        void add(Context & t_context, const sf::FloatRect & t_region) final;
        void update(Context & t_context, const float t_frameTimeSec) final;

        void draw(const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states)
            const final;

        void moveWithMap(const sf::Vector2f & t_move) final;
        void collideWithAvatar(Context &, const sf::FloatRect &) final {}
        bool doesAvatarCollideWithAnyAndDie(const sf::FloatRect &) const final { return false; }
        void appendCollisions(std::vector<sf::FloatRect> &) const final {}

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
