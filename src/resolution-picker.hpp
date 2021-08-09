#ifndef RESOLUTION_PICKER_HPP_INCLUDED
#define RESOLUTION_PICKER_HPP_INCLUDED
//
// resolution-picker.hpp
//
#include <SFML/Window/VideoMode.hpp>

namespace slnghn
{

    struct ResolutionPicker
    {
        static sf::VideoMode pickCloseTo(const sf::VideoMode & TARGET);
    };

} // namespace slnghn

#endif // RESOLUTION_PICKER_HPP_INCLUDED
