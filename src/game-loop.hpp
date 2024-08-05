#ifndef GAME_LOOP_HPP_INCLUDED
#define GAME_LOOP_HPP_INCLUDED
//
// game-loop.hpp
//
#include "avatar.hpp"
#include "check-macros.hpp"
#include "coin.hpp"
#include "context.hpp"
#include "dart.hpp"
#include "fire-spout.hpp"
#include "ghost.hpp"
#include "info-region.hpp"
#include "level-file-loader.hpp"
#include "level.hpp"
#include "missile.hpp"
#include "music-player.hpp"
#include "object-manager.hpp"
#include "owl-calls.hpp"
#include "pause-screen.hpp"
#include "random.hpp"
#include "resources.hpp"
#include "saw.hpp"
#include "screen-regions.hpp"
#include "settings.hpp"
#include "slime.hpp"
#include "sound-player.hpp"
#include "spiked-ball.hpp"
#include "state-machine.hpp"
#include "states.hpp"
#include "stats-display.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>

#include <memory>

namespace halloween
{

    class GameLoop
    {
      public:
        GameLoop(const Settings & settings);

        void play();

      private:
        void setup();
        void frameLoop();
        void handleSleepUntilEndOfFrame(const float elapsedTimeSec);
        void handlePerSecondTasks();
        void handleEvents();
        void update(const float frameTimeSec);
        void draw();

      private:
        Settings m_settings;
        util::Random m_random;
        util::SoundPlayer m_audio;
        util::MusicPlayer m_music;
        OwlCalls m_owlCalls;
        sf::RenderWindow m_window;
        LevelFileLoader m_loader;
        Resources m_media;
        ScreenRegions m_layout;
        Avatar m_avatar;
        PauseScreen m_pauseScreen;
        StateMachine m_stateMachine;
        Level m_level;
        Missiles m_missiles;
        Coins m_coins;
        Darts m_darts;
        SpikedBalls m_spikedBalls;
        FireSpouts m_fireSpouts;
        Saws m_saws;
        Slimes m_slimes;
        Ghosts m_ghosts;
        InfoRegion m_infoRegion;
        ObjectManagerList m_managers;

        std::vector<std::size_t> m_delayLoopCounts;
        std::vector<std::size_t> m_framesPerSecond;
        sf::Clock m_perSecondClock;
        std::unique_ptr<util::GraphDisplay<std::size_t>> m_graphDisplayUPtr;

        Context m_context;
    };
} // namespace halloween

#endif // GAME_LOOP_HPP_INCLUDED
