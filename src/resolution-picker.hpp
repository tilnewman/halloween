#ifndef RESOLUTION_PICKER_HPP_INCLUDED
#define RESOLUTION_PICKER_HPP_INCLUDED
//
// resolution-picker.hpp
//
#include <SFML/Window/VideoMode.hpp>

namespace halloween
{

    struct ResolutionPicker
    {
        static sf::VideoMode pickCloseTo(const sf::VideoMode & TARGET);
    };

} // namespace halloween

#endif // RESOLUTION_PICKER_HPP_INCLUDED
