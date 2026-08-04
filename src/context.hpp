#ifndef CONTEXT_HPP_INCLUDED
#define CONTEXT_HPP_INCLUDED
//
// context.hpp
//

namespace sf
{
    class Window;
}

namespace util
{
    class Random;
    class MusicPlayer;
    class SoundPlayer;
} // namespace util

namespace halloween
{
    class Bats;
    class Saws;
    class Smoke;
    class Level;
    class Coins;
    class Darts;
    class Slimes;
    class Ghosts;
    class Avatar;
    class Missiles;
    class OwlCalls;
    struct Settings;
    class InfoRegion;
    class FireSpouts;
    class FontManager;
    struct LevelStats;
    class SpikedBalls;
    class StateMachine;
    class MushroomBoss;
    class ScreenRegions;
    class MovingPlatforms;
    class FlyObjectManager;
    class FlyTextureManager;
    class ObjectManagerList;
    class FrogObjectManager;
    class FrogTextureManager;
    class SpiderObjectManager;
    class ZombieObjectManager;
    class ZombieTextureManager;
    class SpiderTextureManager;
    class FireSpoutLargeManager;

    struct Context
    {
        Context(
            const Settings & t_settings,
            sf::Window & t_window,
            util::Random & t_random,
            util::SoundPlayer & t_soundPlayer,
            util::MusicPlayer & t_musicPlayer,
            Bats & t_bats,
            OwlCalls & t_owlCalls,
            FontManager & t_fontManager,
            ScreenRegions & t_screenReigons,
            Avatar & t_avatar,
            StateMachine & t_stateMachine,
            Level & t_level,
            Missiles & t_missles,
            Coins & t_coins,
            Darts & t_darts,
            SpikedBalls & t_spikedBalls,
            FireSpouts & t_fireSpouts,
            Saws & t_saw,
            Slimes & t_slimes,
            Ghosts & t_ghosts,
            InfoRegion & t_infoRegion,
            ObjectManagerList & t_objectManagerList,
            LevelStats & t_levelStats,
            MushroomBoss & t_mushrooomBoos,
            Smoke & t_smoke,
            MovingPlatforms & t_movingPlatforms,
            FireSpoutLargeManager & t_fireSpoutLargeManager,
            ZombieObjectManager & t_zombieObjectManager,
            ZombieTextureManager & t_zombieTextureManager,
            FrogTextureManager & t_frogTextureManager,
            FrogObjectManager & t_frogObjectManager,
            SpiderTextureManager & t_spiderTextureManager,
            SpiderObjectManager & t_spiderObjectManager,
            FlyTextureManager & t_flyTextureManager,
            FlyObjectManager & t_flyObjectManager)
            : settings{ t_settings }
            , window{ t_window }
            , random{ t_random }
            , audio{ t_soundPlayer }
            , music{ t_musicPlayer }
            , bats{ t_bats }
            , owl_calls{ t_owlCalls }
            , fonts{ t_fontManager }
            , layout{ t_screenReigons }
            , avatar{ t_avatar }
            , state{ t_stateMachine }
            , level{ t_level }
            , missiles{ t_missles }
            , coins{ t_coins }
            , darts{ t_darts }
            , balls{ t_spikedBalls }
            , spouts{ t_fireSpouts }
            , spouts_large{ t_fireSpoutLargeManager }
            , saws{ t_saw }
            , slimes{ t_slimes }
            , ghosts{ t_ghosts }
            , info_region{ t_infoRegion }
            , managers{ t_objectManagerList }
            , stats{ t_levelStats }
            , boss{ t_mushrooomBoos }
            , smoke{ t_smoke }
            , platforms{ t_movingPlatforms }
            , zombies{ t_zombieObjectManager }
            , zombie_textures{ t_zombieTextureManager }
            , frog_textures{ t_frogTextureManager }
            , frogs{ t_frogObjectManager }
            , spider_textures{ t_spiderTextureManager }
            , spiders{ t_spiderObjectManager }
            , fly_textures{ t_flyTextureManager }
            , flies{ t_flyObjectManager }
        {}

        const Settings & settings;
        const sf::Window & window;
        util::Random & random;
        util::SoundPlayer & audio;
        util::MusicPlayer & music;
        Bats & bats;
        OwlCalls & owl_calls;
        FontManager & fonts;
        ScreenRegions & layout;
        Avatar & avatar;
        StateMachine & state;
        Level & level;
        Missiles & missiles;
        Coins & coins;
        Darts & darts;
        SpikedBalls & balls;
        FireSpouts & spouts;
        FireSpoutLargeManager & spouts_large;
        Saws & saws;
        Slimes & slimes;
        Ghosts & ghosts;
        InfoRegion & info_region;
        ObjectManagerList & managers;
        LevelStats & stats;
        MushroomBoss & boss;
        Smoke & smoke;
        MovingPlatforms & platforms;
        ZombieObjectManager & zombies;
        ZombieTextureManager & zombie_textures;
        FrogTextureManager & frog_textures;
        FrogObjectManager & frogs;
        SpiderTextureManager & spider_textures;
        SpiderObjectManager & spiders;
        FlyTextureManager & fly_textures;
        FlyObjectManager & flies;
    };

} // namespace halloween

#endif // CONTEXT_HPP_INCLUDED
