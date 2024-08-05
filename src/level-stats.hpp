#ifndef LEVELSTATS_HPP_INCLUDED
#define LEVELSTATS_HPP_INCLUDED
//
// level-stats.hpp
//
#include <cstddef>

namespace halloween
{

    struct LevelStats
    {
        bool has_player_died{ false };
        std::size_t enemy_total{ 0 };
        std::size_t enemy_killed{ 0 };
        std::size_t coin_total{ 0 };
        std::size_t coin_collected{ 0 };
    };

} // namespace halloween

#endif // LEVELSTATS_HPP_INCLUDED
