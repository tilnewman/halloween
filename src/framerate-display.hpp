#ifndef FRAMERATE_DISPLAY_HPP_INCLUDED
#define FRAMERATE_DISPLAY_HPP_INCLUDED
//
// framerate-display.hpp
//
#include "stats-display.hpp"

#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Clock.hpp>

#include <vector>

namespace halloween
{

    struct Context;

    //

    class FramerateDisplay
    {
      public:
        FramerateDisplay();

        void update(const Context & t_context, const float t_elapsedTimeSec);

        void draw(const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states)
            const;

      private:
        std::vector<std::size_t> m_framesPerSecond;
        std::unique_ptr<util::GraphDisplay<std::size_t>> m_graphDisplayUPtr;
    };

} // namespace halloween

#endif // FRAMERATE_DISPLAY_HPP_INCLUDED
