#ifndef GAME_LOOP_HPP_INCLUDED
#define GAME_LOOP_HPP_INCLUDED
//
// game-loop.hpp
//
#include "avatar.hpp"
#include "check-macros.hpp"
#include "coin.hpp"
#include "context.hpp"
#include "info-region.hpp"
#include "level-file-loader.hpp"
#include "level.hpp"
#include "missile.hpp"
#include "pause-screen.hpp"
#include "random.hpp"
#include "resources.hpp"
#include "screen-regions.hpp"
#include "settings.hpp"
#include "sound-player.hpp"
#include "states.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>

namespace halloween
{

    class GameLoop
    {
      public:
        GameLoop();

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
        InfoRegion m_infoRegion;

        std::vector<std::size_t> m_delayLoopCounts;
        std::vector<std::size_t> m_framesPerSecond;
        sf::Clock m_perSecondClock;

        Context m_context;
    };
} // namespace halloween

#endif // GAME_LOOP_HPP_INCLUDED
