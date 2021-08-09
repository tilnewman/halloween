// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// resolution-picker.cpp
//
#include "resolution-picker.hpp"

#include "check-macros.hpp"
#include "util.hpp"

#include <algorithm>
#include <cmath>
#include <vector>

namespace slnghn
{

    sf::VideoMode ResolutionPicker::pickCloseTo(const sf::VideoMode & TARGET)
    {
        // M_LOG("Current desktop resolution is " << desktop);

        auto modes = sf::VideoMode::getFullscreenModes();

        M_CHECK(!modes.empty(), "No fullscreen video modes available.  Bail.");

        // remove all modes with color depths different from the current desktop depth
        modes.erase(
            std::remove_if(
                std::begin(modes),
                std::end(modes),
                [&](const sf::VideoMode & mode) {
                    return (mode.bitsPerPixel != TARGET.bitsPerPixel);
                }),
            std::end(modes));

        // M_LOG(
        //    "There are " << modes.size() << " full-screen resolutions at "
        //                 << desktop.bitsPerPixel << "bpp to choose from...");

        M_CHECK(
            !modes.empty(),
            "No fullscreen video modes available at the current color depth.  Bail.");

        // the target resolution is a nice and wide and standard 1920x1080
        auto diffFromTarget = [&](const sf::VideoMode & M) {
            const int horizDiff =
                std::abs(static_cast<int>(TARGET.width) - static_cast<int>(M.width));

            const int vertDiff =
                std::abs(static_cast<int>(TARGET.height) - static_cast<int>(M.height));

            return (horizDiff + vertDiff);
        };

        // sort by closest to target resolution
        std::sort(
            std::begin(modes),
            std::end(modes),
            [&](const sf::VideoMode & A, const sf::VideoMode & B) {
                return diffFromTarget(A) < diffFromTarget(B);
            });

        return modes.front();
    }

} // namespace slnghn
