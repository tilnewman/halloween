#ifndef UTIL_SLIDERS_HPP_INCLUDED
#define UTIL_SLIDERS_HPP_INCLUDED
//
// sliders.hpp
//
#include "random.hpp"
#include "util.hpp"

#include <utility>

namespace util
{
    // All Sliders:
    //  - Use sin() to achieve smooth motion called "sliding" here.
    //  - Start automatically at the given value and isMoving() returns true.
    //  - Call Update() with a fraction of a second to move the value towards the target.
    //  - Update() will call stop() automatically when target value is reached or IsRealClose().
    //  - Continues moving until the target value is exceeded, or IsRealClose(), or until Stop().
    //  - Calling Stop() leaves value unchanged, and then only restart() will get it moving again.
    //  - Default construction sets everything to zero/false and isMoving() returns false.
    //  - Given speeds are clamped to 0.0f or greater.

    //
    // Slides a value from zero to one using smooth sin() motion.
    // Motion is fastest (bounce-like) when START_AT=0.5.
    // Will not work with negative speeds or update(...).
    //
    template <typename T = float>
    class SliderRatio
    {
        static_assert(std::is_floating_point_v<T>);

      public:
        SliderRatio()
            : isMoving_(false)
            , speed_(0.0)
            , value_(0.0)
            , radians_(0.0)
        {}

        explicit SliderRatio(const T SPEED, const T START_AT = T(0))
            : isMoving_(true)
            , speed_(0.0)
            , value_(0.0)
            , radians_(0.0)
        {
            restart(SPEED, START_AT);
        }

        T radians() const { return radians_; }
        T value() const { return value_; }
        T speed() const { return speed_; }
        void speed(const T NEW_SPEED) { speed_ = NEW_SPEED; }
        bool isMoving() const { return isMoving_; }
        void stop() { isMoving_ = false; }

        void restart(const T SPEED, const T START_AT = T(0))
        {
            speed_ = SPEED;
            value_ = std::clamp(START_AT, T(0), T(1));
            radians_ = (radiansFrom_ + (T(3.1415926) * value_));
            update(T(0));
        }

        T update(const T ADJUSTMENT)
        {
            if (isMoving_)
            {
                radians_ += (ADJUSTMENT * speed_);
                value_ = static_cast<float>((T(2.0) - (sin(radians_) + T(1))) * T(0.5));
                value_ = std::clamp(value_, T(0), T(1));

                if ((radians_ > radiansTo_) || isRealClose(radians_, radiansTo_))
                {
                    radians_ = radiansTo_;
                    value_ = T(1);
                    stop();
                }
            }

            return value_;
        }

      private:
        bool isMoving_;
        T speed_;
        T value_;
        T radians_;
        inline static constexpr T radiansFrom_ = (T(3.1415926) * T(0.5));
        inline static constexpr T radiansTo_ = (T(3.1415926) * T(1.5));
    };

    //
    // Slides Value() over [FROM, TO] smoothly using sine motion and then stops.
    //
    template <typename Value_t, typename Math_t = float>
    class SliderFromTo
    {
        static_assert(std::is_floating_point_v<Math_t>);

      public:
        SliderFromTo()
            : from_(Value_t(0))
            , to_(Value_t(0))
            , max_(Value_t(0))
            , min_(Value_t(0))
            , diff_(Value_t(0))
            , speed_(Math_t(0))
            , value_(Value_t(0))
            , slider_()
        {}

        SliderFromTo(const Value_t FROM, const Value_t TO, const Math_t SPEED)
            : from_(FROM)
            , to_(TO)
            , max_(util::max(FROM, TO))
            , min_(util::min(FROM, TO))
            , diff_(TO - FROM)
            , speed_(SPEED)
            , value_(FROM)
            , slider_(speed_)
        {}

        Math_t radians() const { return slider_.radians(); }
        Value_t from() const { return from_; }
        Value_t to() const { return to_; }
        Value_t value() const { return value_; }
        Math_t speed() const { return speed_; }
        bool isMoving() const { return slider_.isMoving(); }
        void stop() { slider_.stop(); }
        Math_t ratio() const { return slider_.value(); }

        Value_t update(const Math_t ADJUSTMENT)
        {
            const Math_t RATIO = slider_.update(ADJUSTMENT);
            value_ = (from_ + static_cast<Value_t>(static_cast<Math_t>(diff_ * RATIO)));
            value_ = std::clamp(value_, min_, max_);
            return value_;
        }

      private:
        Value_t from_;
        Value_t to_;
        Value_t max_;
        Value_t min_;
        Value_t diff_;
        Math_t speed_;
        Value_t value_;
        SliderRatio<Math_t> slider_;
    };

    //
    // Slides Value() back and forth over [FROM, TO] smoothly using sine motion.
    //
    template <typename Value_t, typename Math_t = float>
    class SliderOscillator
    {
      public:
        SliderOscillator()
            : from_(Value_t(0))
            , to_(Value_t(0))
            , slider_()
        {}

        // Use this constructor to start Value() at FROM.
        SliderOscillator(const Value_t FROM, const Value_t TO, const Math_t SPEED)
            : from_(Value_t(0))
            , to_(Value_t(0))
            , slider_()
        {
            restart(FROM, TO, SPEED, FROM);
        }

        // Use this constructor if you want to specify the starting value.
        SliderOscillator(
            const Value_t FROM, const Value_t TO, const Math_t SPEED, const Value_t START_AT)
            : from_(Value_t(0))
            , to_(Value_t(0))
            , slider_()
        {
            restart(FROM, TO, SPEED, START_AT);
        }

        Math_t radians() const { return slider_.radians(); }
        Value_t from() const { return from_; }
        Value_t to() const { return to_; }
        Math_t speed() const { return slider_.speed(); }
        void speed(const Math_t NEW_SPEED) { slider_.speed(NEW_SPEED); }
        Value_t value() const { return slider_.value(); }
        bool isMoving() const { return slider_.isMoving(); }
        void stop() { slider_.stop(); }

        Value_t update(const Math_t ADJUSTMENT)
        {
            if (slider_.isMoving())
            {
                slider_.update(ADJUSTMENT);

                if (!slider_.isMoving())
                {
                    if (isRealClose(slider_.to(), to_))
                    {
                        slider_ = SliderFromTo<Value_t, Math_t>(to_, from_, speed());
                    }
                    else
                    {
                        slider_ = SliderFromTo<Value_t, Math_t>(from_, to_, speed());
                    }
                }
            }

            return slider_.value();
        }

        void restart(
            const Value_t FROM, const Value_t TO, const Math_t SPEED, const Value_t START_AT)
        {
            from_ = FROM;
            to_ = TO;

            // If StartAtClamp() set value_ to TO then start reversed
            if (isRealClose(START_AT, TO))
            {
                slider_ = SliderFromTo<Value_t, Math_t>(to_, from_, SPEED);
            }
            else
            {
                slider_ = SliderFromTo<Value_t, Math_t>(START_AT, TO, SPEED);
            }
        }

        Value_t from_;
        Value_t to_;
        SliderFromTo<Value_t, Math_t> slider_;
    };

    //
    // Slides between random locations over [FROM, TO]
    //
    template <typename Value_t, typename Math_t = float>
    class SliderDrift
    {
      public:
        SliderDrift()
            : valueRange_(0.0f, 0.0f)
            , speedRange_(0.0f, 0.0f)
            , slider_()
        {}

        SliderDrift(
            const Random & random,
            const std::pair<Value_t, Value_t> & VALUE_RANGE,
            const std::pair<Math_t, Math_t> & SPEED_RANGE)
            : valueRange_(VALUE_RANGE)
            , speedRange_(SPEED_RANGE)
            , slider_(
                  random.fromTo(VALUE_RANGE.first, VALUE_RANGE.second),
                  random.fromTo(VALUE_RANGE.first, VALUE_RANGE.second),
                  random.fromTo(speedRange_.first, speedRange_.second))
        {}

        void update(const Random & random, const Math_t ADJUSTMENT)
        {
            slider_.update(ADJUSTMENT);

            if (!slider_.isMoving())
            {
                restart(random);
            }
        }

        Math_t speed() const { return slider_.speed(); }
        void speed(const Math_t NEW_SPEED) { slider_.speed(NEW_SPEED); }
        Value_t value() const { return slider_.value(); }
        bool isMoving() const { return slider_.isMoving(); }
        void stop() { slider_.stop(); }

        float ratio() const
        {
            return util::mapToRatio(slider_.value(), valueRange_.first, valueRange_.second);
        }

        void restart(const Random & random)
        {
            slider_ = util::SliderFromTo<Math_t>(
                slider_.value(),
                random.fromTo(valueRange_.first, valueRange_.second),
                random.fromTo(speedRange_.first, speedRange_.second));
        }

      private:
        std::pair<Value_t, Value_t> valueRange_;
        std::pair<Math_t, Math_t> speedRange_;
        util::SliderFromTo<Math_t> slider_;
    };

} // namespace util

#endif // UTIL_SLIDERS_HPP_INCLUDED
