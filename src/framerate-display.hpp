#ifndef FRAMERATE_DISPLAY_HPP_INCLUDED
#define FRAMERATE_DISPLAY_HPP_INCLUDED
//
// framerate-display.hpp
//
#include "stats-display.hpp"

#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Text.hpp>

#include <vector>

namespace sf
{
    class RenderTarget;
}

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
        sf::Text m_text;
        std::vector<std::size_t> m_framesPerSecond;
        std::unique_ptr<util::GraphDisplay<std::size_t>> m_graphDisplayUPtr;
    };

} // namespace halloween

#endif // FRAMERATE_DISPLAY_HPP_INCLUDED
