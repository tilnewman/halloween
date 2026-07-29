// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// owl-calls.cpp
//
#include "owl-calls.hpp"

#include "context.hpp"
#include "random.hpp"
#include "settings.hpp"
#include "sound-player.hpp"

namespace halloween
{
    OwlCalls::OwlCalls()
        : m_isRunning{ false }
        , m_clock{}
        , m_elapsedTimeSec{ 0.0f }
        , m_timeUntilSec{ 0.0f }
    {}

    void OwlCalls::start(const Context & t_context)
    {
        m_isRunning = true;
        m_timeUntilSec = randomTimeUntilNextCall(t_context);
    }

    void OwlCalls::stop() { m_isRunning = false; }

    float OwlCalls::randomTimeUntilNextCall(const Context & t_context) const
    {
        return t_context.random.fromTo(2.0f, 8.0f);
    }

    void OwlCalls::update(const Context & t_context, const float t_frameTimeSec)
    {
        if (!m_isRunning)
        {
            return;
        }

        m_elapsedTimeSec += t_frameTimeSec;
        if (m_elapsedTimeSec > m_timeUntilSec)
        {
            m_elapsedTimeSec = 0.0f;
            m_timeUntilSec = randomTimeUntilNextCall(t_context);
            t_context.audio.play("owl", t_context.random.fromTo(0.5f, 1.0f));
        }
    }

} // namespace halloween
