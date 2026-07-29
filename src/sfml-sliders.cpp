// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// sfml-sliders.cpp
//
#include "sfml-sliders.hpp"

#include "sfml-util.hpp"

namespace util
{
    PositionDrifter::PositionDrifter()
        : m_horizDrifter{}
        , m_vertDrifter{}
    {}

    void PositionDrifter::setup(
        const Random & t_random,
        const sf::FloatRect & t_region,
        const std::pair<float, float> & t_speed)
    {
        m_horizDrifter = SliderDrift<float, float>(
            t_random, std::make_pair(t_region.position.x, util::right(t_region)), t_speed);

        m_horizDrifter.restart(t_random);

        m_vertDrifter = SliderDrift<float, float>(
            t_random, std::make_pair(t_region.position.y, util::bottom(t_region)), t_speed);

        m_vertDrifter.restart(t_random);
    }

    const sf::Vector2f PositionDrifter::position() const
    {
        return { m_horizDrifter.value(), m_vertDrifter.value() };
    }

    void PositionDrifter::update(const Random & t_random, const float t_frameTimeSec)
    {
        m_horizDrifter.update(t_random, t_frameTimeSec);
        m_vertDrifter.update(t_random, t_frameTimeSec);
    }

} // namespace util
