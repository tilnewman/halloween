#ifndef OWL_CALLS_HPP_INCLUDED
#define OWL_CALLS_HPP_INCLUDED
//
// owl-calls.hpp
//
#include <vector>

#include <SFML/System/Clock.hpp>

namespace halloween
{
    struct Context;
    struct Settings;

    //

    class OwlCalls
    {
      public:
        OwlCalls();

        void start(const Context & t_context);
        void stop();
        void update(const Context & t_context, const float t_frameTimeSec);

      private:
        float randomTimeUntilNextCall(const Context & t_context) const;

      private:
        bool m_isRunning;
        sf::Clock m_clock;
        float m_elapsedTimeSec;
        float m_timeUntilSec;
    };

} // namespace halloween

#endif // OWL_CALLS_HPP_INCLUDED
