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
    // Motion is fastest (bounce-like) when startAt=0.5.
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

        explicit SliderRatio(const T speed, const T startAt = T(0))
            : isMoving_(true)
            , speed_(0.0)
            , value_(0.0)
            , radians_(0.0)
        {
            restart(speed, startAt);
        }

        inline T radians() const { return radians_; }
        inline T value() const { return value_; }
        inline T speed() const { return speed_; }
        inline void speed(const T newSpeed) { speed_ = newSpeed; }
        inline bool isMoving() const { return isMoving_; }
        inline void stop() { isMoving_ = false; }

        void restart(const T speed, const T startAt = T(0))
        {
            speed_ = speed;
            value_ = std::clamp(startAt, T(0), T(1));
            radians_ = (radiansFrom_ + (T(3.1415926) * value_));
            update(T(0));
        }

        T update(const T adjustment)
        {
            if (isMoving_)
            {
                radians_ += (adjustment * speed_);
                value_ = static_cast<T>((T(2.0) - (sin(radians_) + T(1))) * T(0.5));
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
        inline static constexpr T radiansFrom_{ T(3.1415926) * T(0.5) };
        inline static constexpr T radiansTo_{ T(3.1415926) * T(1.5) };
    };

    //
    // Slides Value() over [from, to] smoothly using sine motion and then stops.
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

        explicit SliderFromTo(const Value_t from, const Value_t to, const Math_t speed)
            : from_(from)
            , to_(to)
            , max_(util::max(from, to))
            , min_(util::min(from, to))
            , diff_(to - from)
            , speed_(speed)
            , value_(from)
            , slider_(speed_)
        {}

        inline Math_t radians() const { return slider_.radians(); }
        inline Value_t from() const { return from_; }
        inline Value_t to() const { return to_; }
        inline Value_t value() const { return value_; }
        inline Math_t speed() const { return speed_; }
        inline bool isMoving() const { return slider_.isMoving(); }
        inline void stop() { slider_.stop(); }
        inline Math_t ratio() const { return slider_.value(); }

        Value_t update(const Math_t adjustment)
        {
            const Math_t ratio = slider_.update(adjustment);
            value_ = (from_ + static_cast<Value_t>(static_cast<Math_t>(diff_ * ratio)));
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
    // Slides Value() back and forth over [from, to] smoothly using sine motion.
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

        // Use this constructor to start Value() at from.
        explicit SliderOscillator(const Value_t from, const Value_t to, const Math_t speed)
            : from_(Value_t(0))
            , to_(Value_t(0))
            , slider_()
        {
            restart(from, to, speed, from);
        }

        // Use this constructor if you want to specify the starting value.
        explicit SliderOscillator(
            const Value_t from, const Value_t to, const Math_t speed, const Value_t startAt)
            : from_(Value_t(0))
            , to_(Value_t(0))
            , slider_()
        {
            restart(from, to, speed, startAt);
        }

        inline Math_t radians() const { return slider_.radians(); }
        inline Value_t from() const { return from_; }
        inline Value_t to() const { return to_; }
        inline Math_t speed() const { return slider_.speed(); }
        inline void speed(const Math_t newSpeed) { slider_.speed(newSpeed); }
        inline Value_t value() const { return slider_.value(); }
        inline bool isMoving() const { return slider_.isMoving(); }
        inline void stop() { slider_.stop(); }

        Value_t update(const Math_t adjustment)
        {
            if (slider_.isMoving())
            {
                slider_.update(adjustment);

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

        void
            restart(const Value_t from, const Value_t to, const Math_t speed, const Value_t startAt)
        {
            from_ = from;
            to_ = to;

            // If StartAtClamp() set value_ to to then start reversed
            if (isRealClose(startAt, to))
            {
                slider_ = SliderFromTo<Value_t, Math_t>(to_, from_, speed);
            }
            else
            {
                slider_ = SliderFromTo<Value_t, Math_t>(startAt, to, speed);
            }
        }

        Value_t from_;
        Value_t to_;
        SliderFromTo<Value_t, Math_t> slider_;
    };

    //
    // Slides between random locations over [from, to]
    //
    template <typename Value_t, typename Math_t = float>
    class SliderDrift
    {
      public:
        SliderDrift()
            : valueRange_(Value_t(0), Value_t(0))
            , speedRange_(Math_t(0), Math_t(0))
            , slider_()
        {}

        SliderDrift(
            const Random & random,
            const std::pair<Value_t, Value_t> & valueRange,
            const std::pair<Math_t, Math_t> & speedRange)
            : valueRange_(valueRange)
            , speedRange_(speedRange)
            , slider_(
                  random.fromTo(valueRange.first, valueRange.second),
                  random.fromTo(valueRange.first, valueRange.second),
                  random.fromTo(speedRange_.first, speedRange_.second))
        {}

        void update(const Random & random, const Math_t adjustment)
        {
            slider_.update(adjustment);

            if (!slider_.isMoving())
            {
                restart(random);
            }
        }

        inline Math_t speed() const { return slider_.speed(); }
        inline void speed(const Math_t newSpeed) { slider_.speed(newSpeed); }
        inline Value_t value() const { return slider_.value(); }
        inline bool isMoving() const { return slider_.isMoving(); }
        inline void stop() { slider_.stop(); }

        inline float ratio() const
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
