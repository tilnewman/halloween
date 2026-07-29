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
    class Level;
    class Coins;
    class Darts;
    class Slimes;
    class Ghosts;
    class Avatar;
    class Missiles;
    class OwlCalls;
    struct Settings;
    struct Resources;
    class InfoRegion;
    class FireSpouts;
    struct LevelStats;
    class SpikedBalls;
    class StateMachine;
    class MushroomBoss;
    class ScreenRegions;
    class ObjectManagerList;

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
            Resources & t_resources,
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
            MushroomBoss & t_mushrooomBoos)
            : settings{ t_settings }
            , window{ t_window }
            , random{ t_random }
            , audio{ t_soundPlayer }
            , music{ t_musicPlayer }
            , bats{ t_bats }
            , owl_calls{ t_owlCalls }
            , media{ t_resources }
            , layout{ t_screenReigons }
            , avatar{ t_avatar }
            , state{ t_stateMachine }
            , level{ t_level }
            , missiles{ t_missles }
            , coins{ t_coins }
            , darts{ t_darts }
            , balls{ t_spikedBalls }
            , spouts{ t_fireSpouts }
            , saws{ t_saw }
            , slimes{ t_slimes }
            , ghosts{ t_ghosts }
            , info_region{ t_infoRegion }
            , managers{ t_objectManagerList }
            , stats{ t_levelStats }
            , boss{ t_mushrooomBoos }
        {}

        const Settings & settings;
        const sf::Window & window;
        util::Random & random;
        util::SoundPlayer & audio;
        util::MusicPlayer & music;
        Bats & bats;
        OwlCalls & owl_calls;
        Resources & media;
        ScreenRegions & layout;
        Avatar & avatar;
        StateMachine & state;
        Level & level;
        Missiles & missiles;
        Coins & coins;
        Darts & darts;
        SpikedBalls & balls;
        FireSpouts & spouts;
        Saws & saws;
        Slimes & slimes;
        Ghosts & ghosts;
        InfoRegion & info_region;
        ObjectManagerList & managers;
        LevelStats & stats;
        MushroomBoss & boss;
    };

} // namespace halloween

#endif // CONTEXT_HPP_INCLUDED
