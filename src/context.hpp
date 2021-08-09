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
    class SoundPlayer;
} // namespace util

namespace halloween
{
    struct Level;
    class Coins;
    class Avatar;
    class Missiles;
    struct Settings;
    struct Resources;
    class InfoRegion;
    class PauseScreen;
    class ScreenRegions;
    class LevelFileLoader;
    struct IStatesPending;

    struct Context
    {
        Context(
            Settings & set,
            sf::Window & win,
            util::Random & ran,
            util::SoundPlayer & aud,
            LevelFileLoader & loa,
            Resources & res,
            ScreenRegions & scr,
            Avatar & ava,
            PauseScreen & pau,
            IStatesPending & sta,
            Level & lev,
            Missiles & mis,
            Coins & con,
            InfoRegion & inf)
            : settings(set)
            , window(win)
            , random(ran)
            , audio(aud)
            , loader(loa)
            , media(res)
            , layout(scr)
            , avatar(ava)
            , pause_screen(pau)
            , state(sta)
            , level(lev)
            , missiles(mis)
            , coins(con)
            , info_region(inf)
            , is_paused(false)
            , will_quit(false)
        {}

        Settings & settings;
        sf::Window & window;
        util::Random & random;
        util::SoundPlayer & audio;
        LevelFileLoader & loader;
        Resources & media;
        ScreenRegions & layout;
        Avatar & avatar;
        PauseScreen & pause_screen;
        IStatesPending & state;
        Level & level;
        Missiles & missiles;
        Coins & coins;
        InfoRegion & info_region;

        bool is_paused;
        bool will_quit;
    };

} // namespace halloween

#endif // CONTEXT_HPP_INCLUDED
