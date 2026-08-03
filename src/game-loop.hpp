#ifndef GAME_LOOP_HPP_INCLUDED
#define GAME_LOOP_HPP_INCLUDED
//
// game-loop.hpp
//
#include "avatar.hpp"
#include "bat.hpp"
#include "check-macros.hpp"
#include "coin.hpp"
#include "context.hpp"
#include "dart.hpp"
#include "fire-spout-large.hpp"
#include "fire-spout.hpp"
#include "fonts.hpp"
#include "framerate-display.hpp"
#include "frog-textures.hpp"
#include "frogs.hpp"
#include "ghost.hpp"
#include "info-region.hpp"
#include "level-stats.hpp"
#include "level.hpp"
#include "missile.hpp"
#include "moving-platforms.hpp"
#include "mushroom-boss.hpp"
#include "music-player.hpp"
#include "object-manager.hpp"
#include "owl-calls.hpp"
#include "random.hpp"
#include "saw.hpp"
#include "screen-regions.hpp"
#include "settings.hpp"
#include "slime.hpp"
#include "smoke.hpp"
#include "sound-player.hpp"
#include "spider-textures.hpp"
#include "spiked-ball.hpp"
#include "state-machine.hpp"
#include "states.hpp"
#include "zombie-textures.hpp"
#include "zombies.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>

#include <memory>

namespace halloween
{

    class GameLoop
    {
      public:
        GameLoop(const Settings & t_settings);

        void play();

      private:
        void setup();
        void teardown();
        void frameLoop();
        void handleSleepUntilEndOfFrame(const float t_elapsedTimeSec);
        void handleEvents();
        void update(const float t_frameTimeSec);
        void draw();

      private:
        sf::RenderStates m_states;
        Settings m_settings;
        std::unique_ptr<util::Random> m_randomUPtr;
        std::unique_ptr<util::SoundPlayer> m_audioUPtr;
        std::unique_ptr<util::MusicPlayer> m_musicUPtr;
        std::unique_ptr<Bats> m_batsUPtr;
        std::unique_ptr<OwlCalls> m_owlCallsUPtr;
        std::unique_ptr<sf::RenderWindow> m_windowUPtr;
        std::unique_ptr<FontManager> m_fontManagerUPtr;
        std::unique_ptr<ScreenRegions> m_layoutUPtr;
        std::unique_ptr<Avatar> m_avatarUPtr;
        std::unique_ptr<StateMachine> m_stateMachineUPtr;
        std::unique_ptr<Level> m_levelUPtr;
        std::unique_ptr<Missiles> m_missilesUPtr;
        std::unique_ptr<Coins> m_coinsUPtr;
        std::unique_ptr<Darts> m_dartsUPtr;
        std::unique_ptr<SpikedBalls> m_spikedBallsUPtr;
        std::unique_ptr<FireSpouts> m_fireSpoutsUPtr;
        std::unique_ptr<FireSpoutLargeManager> m_fireSpoutLargeManagerUPtr;
        std::unique_ptr<Saws> m_sawsUPtr;
        std::unique_ptr<Slimes> m_slimesUPtr;
        std::unique_ptr<Ghosts> m_ghostsUPtr;
        std::unique_ptr<InfoRegion> m_infoRegionUPtr;
        std::unique_ptr<ObjectManagerList> m_managersUPtr;
        std::unique_ptr<LevelStats> m_statsUPtr;
        std::unique_ptr<MushroomBoss> m_bossUPtr;
        std::unique_ptr<FramerateDisplay> m_framerateDisplayUPtr;
        std::unique_ptr<Smoke> m_smokeUPtr;
        std::unique_ptr<MovingPlatforms> m_movingPlatformsUPtr;
        std::unique_ptr<ZombieObjectManager> m_zombiesUPtr;
        std::unique_ptr<ZombieTextureManager> m_zombieTexturesUPtr;
        std::unique_ptr<FrogTextureManager> m_frogTexturesUPtr;
        std::unique_ptr<FrogObjectManager> m_frogsUPtr;
        std::unique_ptr<SpiderTextureManager> m_spiderTexturesUPtr;

        std::unique_ptr<Context> m_contextUPtr;
    };

} // namespace halloween

#endif // GAME_LOOP_HPP_INCLUDED
