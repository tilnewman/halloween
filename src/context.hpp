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
    struct Level;
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
    class PauseScreen;
    class MushroomBoss;
    class ScreenRegions;
    class LevelFileLoader;
    struct IStatesPending;
    class ObjectManagerList;

    struct Context
    {
        Context(
            const Settings & set,
            sf::Window & win,
            util::Random & ran,
            util::SoundPlayer & aud,
            util::MusicPlayer & mus,
            Bats & bat,
            OwlCalls & owl,
            LevelFileLoader & loa,
            Resources & res,
            ScreenRegions & scr,
            Avatar & ava,
            PauseScreen & pau,
            IStatesPending & sta,
            Level & lev,
            Missiles & mis,
            Coins & con,
            Darts & dar,
            SpikedBalls & spi,
            FireSpouts & fir,
            Saws & saw,
            Slimes & sli,
            Ghosts & gho,
            InfoRegion & inf,
            ObjectManagerList & man,
            LevelStats & ls,
            MushroomBoss & mb)
            : settings(set)
            , window(win)
            , random(ran)
            , audio(aud)
            , music(mus)
            , bats(bat)
            , owl_calls(owl)
            , loader(loa)
            , media(res)
            , layout(scr)
            , avatar(ava)
            , pause_screen(pau)
            , state(sta)
            , level(lev)
            , missiles(mis)
            , coins(con)
            , darts(dar)
            , balls(spi)
            , spouts(fir)
            , saws(saw)
            , slimes(sli)
            , ghosts(gho)
            , info_region(inf)
            , managers(man)
            , stats(ls)
            , boss(mb)
            , is_paused(false)
            , will_quit(false)
            , level_number(3)
        {}

        const Settings & settings;
        sf::Window & window;
        util::Random & random;
        util::SoundPlayer & audio;
        util::MusicPlayer & music;
        Bats & bats;
        OwlCalls & owl_calls;
        LevelFileLoader & loader;
        Resources & media;
        ScreenRegions & layout;
        Avatar & avatar;
        PauseScreen & pause_screen;
        IStatesPending & state;
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

        bool is_paused;
        bool will_quit;
        std::size_t level_number;
    };

} // namespace halloween

#endif // CONTEXT_HPP_INCLUDED
