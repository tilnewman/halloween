#ifndef UTIL_SLIDERS_SFML_HPP_INCLUDED
#define UTIL_SLIDERS_SFML_HPP_INCLUDED
//
// sliders-sfml.hpp
//
#include "color-range.hpp"
#include "random.hpp"
#include "sliders.hpp"

namespace util
{
    //
    //
    //
    class PositionDrifter
    {
      public:
        PositionDrifter()
            : valueArea_(0.0f, 0.0f, 0.0f, 0.0f)
            , speedRange_(0.0f, 0.0f)
            , horizDrifter_()
            , vertDrifter_()
        {}

        PositionDrifter(
            const Random & random,
            const sf::FloatRect & VALUE_AREA,
            const sf::Vector2f & SPEED_RANGE)
            : valueArea_(VALUE_AREA)
            , speedRange_(SPEED_RANGE)
            , horizDrifter_()
            , vertDrifter_()
        {
            reset(random);
        }

        void update(const Random & random, const float ADJUSTMENT)
        {
            horizDrifter_.update(random, ADJUSTMENT);
            vertDrifter_.update(random, ADJUSTMENT);
        }

        sf::Vector2f value() const { return { horizDrifter_.value(), vertDrifter_.value() }; }

        void reset(const Random & random)
        {
            horizDrifter_ = util::SliderDrift<float>(
                random,
                { valueArea_.left, util::right(valueArea_) },
                { speedRange_.x, speedRange_.y });

            vertDrifter_ = util::SliderDrift<float>(
                random,
                { valueArea_.top, util::bottom(valueArea_) },
                { speedRange_.x, speedRange_.y });
        }

      private:
        sf::FloatRect valueArea_;
        sf::Vector2f speedRange_;
        util::SliderDrift<float> horizDrifter_;
        util::SliderDrift<float> vertDrifter_;
    };

    //
    //
    //
    class ColorDrifter
    {
      public:
        ColorDrifter()
            : from_(sf::Color::White)
            , to_(sf::Color::White)
            , current_(sf::Color::White)
            , speedRange_(0.0f, 0.0f)
            , slider_()
        {}

        ColorDrifter(const Random & random, const sf::Vector2f & SPEED_RANGE)
            : from_(sf::Color::White)
            , to_(sf::Color::White)
            , current_(sf::Color::White)
            , speedRange_(SPEED_RANGE)
            , slider_()
        {
            reset(random);
        }

        void update(const Random & random, const float ADJUSTMENT)
        {
            slider_.update(ADJUSTMENT);

            const auto RATIO{ slider_.value() };

            const auto RED_DIFF{ static_cast<float>(to_.r - from_.r) };
            const auto GREEN_DIFF{ static_cast<float>(to_.g - from_.g) };
            const auto BLUE_DIFF{ static_cast<float>(to_.b - from_.b) };
            const auto ALPHA_DIFF{ static_cast<float>(to_.a - from_.a) };

            const auto RED{ static_cast<float>(from_.r) + (RED_DIFF * RATIO) };
            const auto GREEN{ static_cast<float>(from_.g) + (GREEN_DIFF * RATIO) };
            const auto BLUE{ static_cast<float>(from_.b) + (BLUE_DIFF * RATIO) };
            const auto ALPHA{ static_cast<float>(from_.a) + (ALPHA_DIFF * RATIO) };

            current_ = sf::Color(
                static_cast<sf::Uint8>(RED),
                static_cast<sf::Uint8>(GREEN),
                static_cast<sf::Uint8>(BLUE),
                static_cast<sf::Uint8>(ALPHA));

            if (!slider_.isMoving())
            {
                reset(random);
            }
        }

        sf::Color value() const { return current_; }

        void reset(const Random & random)
        {
            from_ = to_;
            to_ = colors::randomVibrant(random);
            slider_ = util::SliderRatio(random.fromTo(speedRange_.x, speedRange_.y));
        }

      private:
        sf::Color from_;
        sf::Color to_;
        sf::Color current_;
        sf::Vector2f speedRange_;
        util::SliderRatio<float> slider_;
    };
} // namespace util

#endif // UTIL_SLIDERS_SFML_HPP_INCLUDED
