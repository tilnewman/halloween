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
        : m_horizDrifter()
        , m_vertDrifter()
    {}

    void PositionDrifter::setup(
        const Random & random, const sf::FloatRect & region, const std::pair<float, float> & speed)
    {
        m_horizDrifter = SliderDrift<float, float>(
            random, std::make_pair(region.position.x, util::right(region)), speed);

        m_horizDrifter.restart(random);

        m_vertDrifter = SliderDrift<float, float>(
            random, std::make_pair(region.position.y, util::bottom(region)), speed);

        m_vertDrifter.restart(random);
    }

    const sf::Vector2f PositionDrifter::position() const
    {
        return { m_horizDrifter.value(), m_vertDrifter.value() };
    }

    void PositionDrifter::update(const Random & random, const float frameTimeSec)
    {
        m_horizDrifter.update(random, frameTimeSec);
        m_vertDrifter.update(random, frameTimeSec);
    }

} // namespace util
