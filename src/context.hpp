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
    class SpikedBalls;
    class PauseScreen;
    class ScreenRegions;
    class LevelFileLoader;
    struct IStatesPending;

    struct Context
    {
        Context(
            const Settings & set,
            sf::Window & win,
            util::Random & ran,
            util::SoundPlayer & aud,
            util::MusicPlayer & mus,
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
            Slimes & sli,
            Ghosts & gho,
            InfoRegion & inf)
            : settings(set)
            , window(win)
            , random(ran)
            , audio(aud)
            , music(mus)
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
            , slimes(sli)
            , ghosts(gho)
            , info_region(inf)
            , is_paused(false)
            , will_quit(false)
            , level_number(1)
        {}

        const Settings & settings;
        sf::Window & window;
        util::Random & random;
        util::SoundPlayer & audio;
        util::MusicPlayer & music;
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
        Slimes & slimes;
        Ghosts & ghosts;
        InfoRegion & info_region;

        bool is_paused;
        bool will_quit;
        std::size_t level_number;
    };

} // namespace halloween

#endif // CONTEXT_HPP_INCLUDED
