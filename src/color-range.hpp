#ifndef COLOR_RANGE_HPP_INCLUDED
#define COLOR_RANGE_HPP_INCLUDED
//
// color-range.hpp
//
#include "random.hpp"
#include "util.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <vector>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Window.hpp>

namespace colors
{

    using Diff_t = std::int_fast32_t;

    // color value diffs

    template <typename T = Diff_t>
    [[nodiscard]] constexpr T diff(const std::uint8_t left, const std::uint8_t right) noexcept
    {
        return (static_cast<T>(left) - static_cast<T>(right));
    }

    template <typename T = Diff_t>
    [[nodiscard]] constexpr T diffAbs(const std::uint8_t left, const std::uint8_t right) noexcept
    {
        return util::abs(diff<T>(left, right));
    }

    [[nodiscard]] inline constexpr float
        diffRatio(const std::uint8_t left, const std::uint8_t right) noexcept
    {
        return (diffAbs<float>(left, right) / 255.0f);
    }

    // whole color diffs

    [[nodiscard]] inline Diff_t
        diffMagnitudeCount(const sf::Color & left, const sf::Color & right) noexcept
    {
        return (
            diffAbs(left.r, right.r) + diffAbs(left.g, right.g) + diffAbs(left.b, right.b) +
            diffAbs(left.a, right.a));
    }

    [[nodiscard]] inline Diff_t
        diffMagnitudeCountOpaque(const sf::Color & left, const sf::Color & right) noexcept
    {
        return (diffAbs(left.r, right.r) + diffAbs(left.g, right.g) + diffAbs(left.b, right.b));
    }

    [[nodiscard]] inline float
        diffMagnitude(const sf::Color & left, const sf::Color & right) noexcept
    {
        return (static_cast<float>(diffMagnitudeCount(left, right)) / static_cast<float>(255 * 4));
    }

    [[nodiscard]] inline float
        diffMagnitudeOpaque(const sf::Color & left, const sf::Color & right) noexcept
    {
        return (
            static_cast<float>(diffMagnitudeCountOpaque(left, right)) /
            static_cast<float>(255 * 3));
    }

    [[nodiscard]] inline float diffEuclid(const sf::Color & left, const sf::Color & right) noexcept
    {
        const Diff_t diffRed{ diffAbs(left.r, right.r) };
        const Diff_t diffGrn{ diffAbs(left.g, right.g) };
        const Diff_t diffBlu{ diffAbs(left.b, right.b) };
        const Diff_t diffAlp{ diffAbs(left.a, right.a) };

        const double diff{ std::sqrt(static_cast<double>(
            (diffRed * diffRed) + (diffGrn * diffGrn) + (diffBlu * diffBlu) +
            (diffAlp * diffAlp))) };

        const double ratioCorrection{ std::sqrt(static_cast<double>(255 * 255 * 4)) };

        return static_cast<float>(diff / ratioCorrection);
    }

    [[nodiscard]] inline float diffEuclidOpaque(sf::Color left, sf::Color right) noexcept
    {
        const Diff_t diffRed{ diffAbs(left.r, right.r) };
        const Diff_t diffGrn{ diffAbs(left.g, right.g) };
        const Diff_t diffBlu{ diffAbs(left.b, right.b) };

        const double diff{ std::sqrt(
            static_cast<double>((diffRed * diffRed) + (diffGrn * diffGrn) + (diffBlu * diffBlu))) };

        const double ratioCorrection{ std::sqrt(static_cast<double>(255 * 255 * 3)) };

        return static_cast<float>(diff / ratioCorrection);
    }

    // whole color diffs (that attempt to look correct to the human eye)

    // from https://www.compuphase.com/cmetric.htm
    [[nodiscard]] inline float
        diffWeightedEuclidOpaque(const sf::Color & left, const sf::Color & right) noexcept
    {
        const Diff_t avgRed{ (static_cast<Diff_t>(left.r) + static_cast<Diff_t>(right.r)) / 2 };

        const Diff_t diffRed{ diff(left.r, right.r) };
        const Diff_t diffGrn{ diff(left.g, right.g) };
        const Diff_t diffBlu{ diff(left.b, right.b) };

        const Diff_t bitsRed{ ((512 + avgRed) * diffRed * diffRed) >> 8 };
        const Diff_t bitsGreen{ 4 * diffGrn * diffGrn };
        const Diff_t bitsBlue{ ((767 - avgRed) * diffBlu * diffBlu) >> 8 };

        const double diff{ std::sqrt(static_cast<double>(bitsRed + bitsGreen + bitsBlue)) };
        const double ratioCorrection{ std::sqrt(584970.0) };

        return static_cast<float>(diff / ratioCorrection);
    }

    // blends

    [[nodiscard]] inline constexpr std::uint8_t
        blend(const float ratio, const std::uint8_t from, const std::uint8_t to) noexcept
    {
        return static_cast<std::uint8_t>(
            util::mapRatioTo(ratio, static_cast<float>(from), static_cast<float>(to)));
    }

    [[nodiscard]] inline sf::Color
        blend(const float ratio, const sf::Color & from, const sf::Color & to) noexcept
    {
        return sf::Color(
            blend(ratio, from.r, to.r),
            blend(ratio, from.g, to.g),
            blend(ratio, from.b, to.b),
            blend(ratio, from.a, to.a));
    }

    template <typename Container_t>
    [[nodiscard]] inline sf::Color blend(const float ratio, const Container_t & src)
    {
        const std::size_t srcSize{ src.size() };

        if (0 == srcSize)
        {
            return sf::Color::Transparent;
        }

        const auto srcBegin{ std::begin(src) };

        if ((1 == srcSize) || !(ratio > 0.0f))
        {
            return *srcBegin;
        }

        if (!(ratio < 1.0f))
        {
            return *(srcBegin + static_cast<std::ptrdiff_t>(srcSize - 1));
        }

        const float srcSizeMinusOneF{ static_cast<float>(srcSize - 1) };
        const auto fromIndex{ static_cast<std::ptrdiff_t>(ratio * srcSizeMinusOneF) };

        const float colorSpread{ 1.0f / srcSizeMinusOneF };
        const float colorRatioMin{ static_cast<float>(fromIndex) * colorSpread };
        const float colorRatio{ (ratio - colorRatioMin) / colorSpread };

        return blend(colorRatio, *(srcBegin + fromIndex), *(srcBegin + fromIndex + 1_st));
    }

    //
    // HSL
    //
    //  HSL and HSV are alternative representations of the RGB color model designed in the
    //  1970s. They are better than RGB, but still make a trade-off of perceptual relevance
    //  for computation speed.  They work well enough when humans need to pick a single
    //  distinct color in isolation, but they are a poor choice when humans need to measure
    //  of how similar/different multiple colors appear to be.
    //
    //  From 1970-2000 there were few displays that could accurately display colors.  So
    //  humans were forced to use HSL/HSV numbers for colors.  They were better than RGB,
    //  but still fell far short of how humans see color.  Now that displays capable of
    //  showing accurate colors, humans can finally use their eyes to pick
    //  related/similar/distinct colors on screen.  So now the limitations of HSL and HSV
    //  are obvious, and significant enough to require better, more perceptual models.
    //  Luckily, computers are also now fast enough use such models that are far more
    //  computationally expensive, such as CIE-Lab/Luv.  But, uh, the HSL and Hsla are
    //  still here for some reason.
    //
    //  So not a real color space, just a handy re-arranged RGB for color picking.
    //
    struct Hsla
    {
        Hsla() = default;

        Hsla(
            const float hue,
            const float saturation,
            const float lightness,
            const std::uint8_t alpha = 255)
            : h(hue)
            , s(saturation)
            , l(lightness)
            , a(alpha)
        {}

        explicit Hsla(const sf::Color & color)
        {
            a = color.a;

            const float redRatio{ static_cast<float>(color.r) / 255.0f };
            const float grnRatio{ static_cast<float>(color.g) / 255.0f };
            const float bluRatio{ static_cast<float>(color.b) / 255.0f };

            const float min{ std::min(std::min(redRatio, grnRatio), bluRatio) };
            const float max{ std::max(std::max(redRatio, grnRatio), bluRatio) };
            const float diff{ max - min };

            l = ((max + min) * 0.5f);

            if (util::isRealCloseOrLess(diff, 0.0f))
            {
                return;
            }

            if (l < 0.5f)
            {
                s = (diff / (max + min));
            }
            else
            {
                s = (diff / (2.0f - max - min));
            }

            const float redDiff{ (((max - redRatio) / 6.0f) + (diff / 2.0f)) / diff };
            const float grnDiff{ (((max - grnRatio) / 6.0f) + (diff / 2.0f)) / diff };
            const float bluDiff{ (((max - bluRatio) / 6.0f) + (diff / 2.0f)) / diff };

            if (util::isRealClose(redRatio, max))
            {
                h = (bluDiff - grnDiff);
            }
            else if (util::isRealClose(grnRatio, max))
            {
                h = (((1.0f / 3.0f) + redDiff) - bluDiff);
            }
            else if (util::isRealClose(bluRatio, max))
            {
                h = (((2.0f / 3.0f) + grnDiff) - redDiff);
            }

            h = hueRatioCorrect(h);
        }

        sf::Color sfColor() const
        {
            sf::Color color;

            if (util::isRealClose(s, 0.0f))
            {
                color.r = static_cast<std::uint8_t>(l * 255.0f);
                color.g = static_cast<std::uint8_t>(l * 255.0f);
                color.b = static_cast<std::uint8_t>(l * 255.0f);
            }
            else
            {
                const float var2{ (l < 0.5f) ? (l * (1 + s)) : ((l + s) - (s * l)) };
                const float var1{ (2 * l) - var2 };

                color.r = hueRatioToRgb(var1, var2, (h + (1.0f / 3.0f)));
                color.g = hueRatioToRgb(var1, var2, h);
                color.b = hueRatioToRgb(var1, var2, (h - (1.0f / 3.0f)));
            }

            color.a = a;
            return color;
        }

        std::string toString() const
        {
            std::ostringstream ss;

            ss << '[' << h << ',' << s << ',' << l;

            if (a < 255)
            {
                ss << ',' << static_cast<Diff_t>(a);
            }

            ss << ']';
            return ss.str();
        }

        float h{ 0.0f }; // ratio of degrees [0,360]
        float s{ 0.0f };
        float l{ 0.0f };
        std::uint8_t a{ 255 }; // same as sf::Color.a (transparent=0, opaque=255)

      private:
        static constexpr float hueRatioCorrect(const float hueRatio) noexcept
        {
            float result{ hueRatio };

            if (result < 0.0f)
            {
                result += 1.0f;
            }

            if (result > 1.0f)
            {
                result -= 1.0f;
            }

            return result;
        }

        static constexpr std::uint8_t
            hueRatioToRgb(const float var1, const float var2, const float hueParam) noexcept
        {
            const float hue{ hueRatioCorrect(hueParam) };

            float result{ 0.0f };

            if ((6.0f * hue) < 1.0f)
            {
                result = (var1 + (var2 - var1) * 6.0f * hue);
            }
            else if ((2.0f * hue) < 1.0f)
            {
                result = var2;
            }
            else if ((3.0f * hue) < 2.0f)
            {
                result = (var1 + (var2 - var1) * ((2.0f / 3.0f) - hue) * 6.0f);
            }
            else
            {
                result = var1;
            }

            result *= 255.0f;

            // fix float math errors
            if ((result < 255.0f) && ((result - std::floor(result)) > 0.99f))
            {
                result = std::ceil(result);
            }

            return static_cast<std::uint8_t>(result);
        }
    };

    inline std::ostream & operator<<(std::ostream & os, const Hsla & hsl)
    {
        os << hsl.toString();
        return os;
    }

    //
    // Brightness
    //   These are all estimates that are flawed when it comes to bright reds
    //   which they all say are darker than they really are.  Luminosity is
    //   the best but slow, while my WeightedMean is far faster and nearly
    //   identical.
    //
    namespace brightness
    {

        // not very good, HSL is for humans picking colors, but fast
        inline float Hsl(const sf::Color & color) { return Hsla(color).l; }

        // my own version, nearly identical to Luminosity
        inline float WeightedMean(const sf::Color & color)
        {
            const float red{ static_cast<float>(color.r) };
            const float green{ static_cast<float>(color.g) };
            const float blue{ static_cast<float>(color.b) };

            const float redFactor{ 0.2126f * red * red };
            const float greenFactor{ 0.7152f * green * green };
            const float blueFactor{ 0.0722f * blue * blue };

            return (redFactor + greenFactor + blueFactor);
        }

        // from https://www.w3.org/TR/AERT/#color-contrast,
        // better than HSL but has glaring problems with bright reds
        inline float W3Perceived(const sf::Color & color)
        {
            const float red{ static_cast<float>(color.r) };
            const float green{ static_cast<float>(color.g) };
            const float blue{ static_cast<float>(color.b) };

            const float redFactor{ 0.299f * red };
            const float greenFactor{ 0.587f * green };
            const float blueFactor{ 0.114f * blue };

            return (redFactor + greenFactor + blueFactor) / 1000.0f;
        }

        // from ?, the best I've found so far for photos
        inline float Luminosity(const sf::Color & color)
        {
            const float redRatio{ static_cast<float>(color.r) / 255.0f };
            const float greenRatio{ static_cast<float>(color.g) / 255.0f };
            const float blueRatio{ static_cast<float>(color.b) / 255.0f };

            const float redFactor{ 0.2126f * powf(redRatio, 2.2f) };
            const float greenFactor{ 0.7152f * powf(greenRatio, 2.2f) };
            const float blueFactor{ 0.0722f * powf(blueRatio, 2.2f) };

            return (redFactor + greenFactor + blueFactor);
        }
    } // namespace brightness

    //
    // helper stuff
    //

    struct ColorAtRatio
    {
        sf::Color color = sf::Color::Transparent;
        float ratio = 0.0f;
    };

    template <typename Container_t>
    void normalize(Container_t & colorsAt)
    {
        // fix order mistakes
        std::stable_sort(
            std::begin(colorsAt),
            std::end(colorsAt),
            [](const ColorAtRatio & left, const ColorAtRatio & right) {
                return (left.ratio < right.ratio);
            });

        // remove colors at the same position
        colorsAt.erase(
            std::unique(
                std::begin(colorsAt),
                std::end(colorsAt),
                [&](const ColorAtRatio & left, const ColorAtRatio & right) {
                    return util::isRealClose(left.ratio, right.ratio);
                }),
            std::end(colorsAt));

        if (colorsAt.empty())
        {
            return;
        }

        // fix if first ratio is not 0.0f by offsetting
        const float firstRatio{ colorsAt.front().ratio };
        if ((firstRatio < 0.0f) || (firstRatio > 0.0f))
        {
            for (ColorAtRatio & colorAt : colorsAt)
            {
                colorAt.ratio -= firstRatio;
            }

            colorsAt.front().ratio = 0.0f;
        }

        if (colorsAt.size() == 1)
        {
            return;
        }

        // fix if last ratio is not 1.0f by scaling
        const float lastRatio{ colorsAt.back().ratio };
        if ((lastRatio < 1.0f) || (lastRatio > 1.0f))
        {
            for (ColorAtRatio & colorAt : colorsAt)
            {
                colorAt.ratio *= (1.0f / lastRatio);
            }

            colorsAt.back().ratio = 1.0f;
        }
    }

    template <typename Container_t>
    Container_t normalizeCopy(const Container_t & colorsAt)
    {
        Container_t cleaned{ colorsAt };
        colorsAtRatioNormalize(cleaned);
        return cleaned;
    }

    template <typename Container_t>
    sf::Color ratioFromClamped(const float ratio, const Container_t & container)
    {
        const std::size_t size{ container.size() };

        if (0 == size)
        {
            return sf::Color::Transparent;
        }

        if (!(ratio > 0.0f))
        {
            return *std::begin(container);
        }

        const std::size_t index{ static_cast<std::size_t>(ratio * static_cast<float>(size)) };

        if (index >= size)
        {
            return *(std::begin(container) + static_cast<std::ptrdiff_t>(size - 1));
        }

        return *(std::begin(container) + static_cast<std::ptrdiff_t>(index));
    }

    template <typename Container_t>
    sf::Color ratioFromRotation(const float ratio, const Container_t & container)
    {
        return ratioFromClamped((ratio - std::floor(ratio)), container);
    }

    //
    // Random Colors
    //

    inline sf::Color random(const util::Random & random, const bool willRandomizeAlpha = false)
    {
        return sf::Color(
            random.ofType<std::uint8_t>(),
            random.ofType<std::uint8_t>(),
            random.ofType<std::uint8_t>(),
            ((willRandomizeAlpha) ? random.ofType<std::uint8_t>() : 255));
    }

    inline sf::Color
        randomVibrant(const util::Random & random, const bool willRandomizeAlpha = false)
    {
        std::array<std::uint8_t, 3> values = { random.fromTo<std::uint8_t>(0, 255),
                                               random.fromTo<std::uint8_t>(0, 255),
                                               random.fromTo<std::uint8_t>(0, 255) };

        if (diff(values[0], values[1]) < 191)
        {
            const auto avg{ (values[0] + values[1]) / 2 };

            if (avg < 127)
            {
                values[2] = random.fromTo<std::uint8_t>(235, 255);
            }
            else
            {
                values[2] = random.fromTo<std::uint8_t>(0, 20);
            }
        }

        random.shuffle(values);

        std::uint8_t alpha{ 255 };
        if (willRandomizeAlpha)
        {
            alpha = random.ofType<std::uint8_t>();
        }

        return sf::Color(values[0], values[1], values[2], alpha);
    }

    //
    // blend fills (where colors are equally/linear spaced)
    //

    template <typename OutColorIter_t>
    void blendFill(
        const OutColorIter_t dstBegin,
        const std::size_t count,
        const sf::Color & from,
        const sf::Color & to)
    {
        const float divisor{ (count <= 2) ? 1.0f : static_cast<float>(count - 1) };

        for (std::size_t i(0); i < count; ++i)
        {
            *(dstBegin + static_cast<std::ptrdiff_t>(i)) =
                blend((static_cast<float>(i) / divisor), from, to);
        }
    }

    template <typename Container_t>
    void blendFill(Container_t & dst, const sf::Color & from, const sf::Color & to)
    {
        blendFill(std::begin(dst), dst.size(), from, to);
    }

    template <typename DstContainer_t, typename SrcContainer_t>
    void blendFill(DstContainer_t & dst, const SrcContainer_t & src)
    {
        if (src.empty() || dst.empty())
        {
            return;
        }

        const std::size_t srcSize{ src.size() };
        const auto srcBegin{ std::begin(src) };

        const float divisor{ (srcSize <= 2) ? 1.0f : static_cast<float>(srcSize - 1) };

        std::vector<ColorAtRatio> colorsAt(srcSize);

        for (std::size_t i(0); i < src.size(); ++i)
        {
            colorsAt[i].color = *(srcBegin + static_cast<std::ptrdiff_t>(i));
            colorsAt[i].ratio = (static_cast<float>(i) / divisor);
        }

        blendFillNonLinear(dst, colorsAt);
    }

    //
    // blend fills (where colors are NOT equally/linear spaced)
    //

    template <typename DstContainer_t, typename SrcContainer_t>
    void blendFillNonLinear(DstContainer_t & dst, const SrcContainer_t & srcOrig)
    {
        if (dst.empty())
        {
            return;
        }

        const std::size_t dstSize{ dst.size() };
        const auto dstBegin{ std::begin(dst) };

        std::vector<ColorAtRatio> src{ srcOrig };
        normalize(src);

        const auto srcBegin{ std::begin(src) };
        const std::size_t srcSize{ src.size() };

        if (src.empty())
        {
            return;
        }

        if ((1 == srcSize) || (1 == dstSize))
        {
            std::fill_n(dstBegin, dstSize, srcBegin->color);
            return;
        }

        if (2 == srcSize)
        {
            blendFill(dst, srcBegin->color, (srcBegin + 1)->color);
            return;
        }

        const std::size_t idealColorsPerBlend{ 512 };
        const std::size_t idealSize{ srcSize * idealColorsPerBlend };

        if (dstSize >= idealSize)
        {
            // this is the normal case where dst was big enough

            std::size_t srcIndex(0);
            std::size_t dstIndex(0);
            while ((srcIndex < (srcSize - 1)) && (dstIndex < dstSize))
            {
                const ColorAtRatio & from{ *(srcBegin + static_cast<std::ptrdiff_t>(srcIndex)) };
                const ColorAtRatio & to{ *(srcBegin + static_cast<std::ptrdiff_t>(srcIndex + 1)) };

                const std::size_t blendCount{ static_cast<std::size_t>(
                    (to.ratio - from.ratio) * static_cast<float>(dstSize)) };

                blendFill(
                    (dstBegin + static_cast<std::ptrdiff_t>(dstIndex)),
                    blendCount,
                    from.color,
                    to.color);

                ++srcIndex;
                dstIndex += blendCount;
            }

            *(dstBegin + static_cast<std::ptrdiff_t>(dstSize - 1)) =
                (srcBegin + static_cast<std::ptrdiff_t>(srcSize - 1))->color;
        }
        else
        {
            // first: recurse and render to temp container "ideal" if dst was too quarter
            DstContainer_t ideal(idealSize);
            blendFillNonLinear(ideal, src);

            const auto idealBegin{ std::begin(ideal) };

            // second: scale down the ideal to whatever size dst actually is
            for (std::size_t dstIndex(0); dstIndex < dstSize; ++dstIndex)
            {
                const float ratio{ static_cast<float>(dstIndex) / static_cast<float>(dstSize - 1) };

                const std::size_t idealIndex{ static_cast<std::size_t>(
                    ratio * static_cast<float>(ideal.size() - 1)) };

                *(dstBegin + static_cast<std::ptrdiff_t>(dstIndex)) =
                    *(idealBegin + static_cast<std::ptrdiff_t>(idealIndex));
            }

            *(dstBegin + static_cast<std::ptrdiff_t>(dstSize - 1)) =
                *(idealBegin + static_cast<std::ptrdiff_t>(ideal.size() - 1));
        }
    }

    // just a container to prevent re-calculating
    class BlendCache
    {
        explicit BlendCache(const std::size_t size)
            : m_colors(size)
        {}

      public:
        BlendCache(const BlendCache &) = default;
        BlendCache(BlendCache &&) = default;
        BlendCache & operator=(const BlendCache &) = default;
        BlendCache & operator=(BlendCache &&) = default;

        //

        bool isEmpty() const noexcept { return m_colors.empty(); }
        std::size_t size() const noexcept { return m_colors.size(); }

        sf::Color atRatioClamped(const float ratio) const
        {
            return ratioFromClamped(ratio, m_colors);
        }

        sf::Color atRatioRotation(const float ratio) const
        {
            return ratioFromRotation(ratio, m_colors);
        }

        sf::Color first() const
        {
            return ((isEmpty()) ? sf::Color::Transparent : m_colors.front());
        }

        sf::Color last() const { return ((isEmpty()) ? sf::Color::Transparent : m_colors.back()); }

        const std::vector<sf::Color> & vector() const noexcept { return m_colors; }

        //

        template <typename Container_t>
        static BlendCache makeFromColors(const std::size_t size, const Container_t & container)
        {
            BlendCache cache(size);
            blendFill(cache.m_colors, container);
            return cache;
        }

        template <typename Container_t>
        static BlendCache
            makeFromColorsNonLinear(const std::size_t size, const Container_t & container)
        {
            BlendCache cache(size);
            blendFillNonLinear(cache.m_colors, container);
            return cache;
        }

        // template <typename Container_t>
        // static BlendCache makeFromBlendCaches(const std::size_t size, const Container_t &
        // container)
        // {
        //     std::size_t total{ 0 };
        //     for (const BlendCache & cache : container)
        //     {
        //         total += cache.size();
        //     }
        //
        //     std::vector<sf::Color> colors;
        //     colors.reserve(total);
        //
        //     for (const BlendCache & cache : container)
        //     {
        //         std::copy(
        //             std::begin(cache.vector()),
        //             std::end(cache.vector()),
        //             std::back_inserter(cache.m_colors));
        //     }
        //
        //     return makeFromColors(size, colors);
        // }

      private:
        std::vector<sf::Color> m_colors;
    };

} // namespace colors

#endif // COLOR_RANGE_HPP_INCLUDED
