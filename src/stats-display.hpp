#ifndef STATSDISPLAY_HPP_INCLUDED
#define STATSDISPLAY_HPP_INCLUDED
//
// stats-display.hpp
//
#include "check-macros.hpp"
#include "sfml-util.hpp"

#include <iostream>
#include <string>

#include <SFML/Graphics.hpp>

namespace util
{

    template <typename data_t>
    class GraphDisplay : public sf::Drawable
    {
      public:
        GraphDisplay(const std::vector<data_t> & data, const sf::Vector2u & size = { 1000, 500 })
            : m_size(size)
            , m_data(data)
            , m_renderTexture()
            , m_backgroundColor(22, 25, 28)
            , m_dataBarColor(38, 120, 254)
            , m_dataBarColorError(255, 32, 32)
            , m_wasDatasetChanged(false)
        {
            // reduce data set to fit in the size given
            while (m_data.size() > static_cast<std::size_t>(m_size.x))
            {
                m_data = makeHalfSizeDataSet(m_data);
            }

            m_wasDatasetChanged = (data.size() != m_data.size());

            // find min/max/avg/std_dev
            const auto stats = util::makeStats(m_data);

            // convert data values into data bar rects
            std::vector<sf::FloatRect> dataBarRects;
            populateDataBarRects(stats.max, dataBarRects);

            // create render texture of the correct size to fit the data -not the m_size.x
            const unsigned dataWidth = static_cast<unsigned>(util::right(dataBarRects.back()));

            if (!m_renderTexture.create(dataWidth, m_size.y))
            {
                std::cout << "GraphDisplay's sf::RenderTexture::create failed for size="
                          << dataWidth << "x" << m_size.y
                          << ". So the graph will show solid magenta.  The color of pure evil."
                          << std::endl;

                m_renderTexture.create(32, 32); // this won't fail...probably
                m_renderTexture.clear(sf::Color::Magenta);
                m_renderTexture.display();
            }
            else
            {
                // draw graph
                m_renderTexture.clear(m_backgroundColor);

                const sf::Color & barColorToUse =
                    ((m_wasDatasetChanged) ? m_dataBarColorError : m_dataBarColor);

                drawGraph(m_renderTexture, barColorToUse, m_backgroundColor, dataBarRects);
                m_renderTexture.display();
            }
        }

        void draw(sf::RenderTarget & renderTarget, sf::RenderStates states) const override
        {
            sf::Sprite sprite(m_renderTexture.getTexture());
            util::scale(sprite, sf::Vector2f{ m_size });
            renderTarget.draw(sprite, states);
        }

        void draw(
            const sf::Vector2f & position,
            sf::RenderTarget & renderTarget,
            const sf::RenderStates states = {}) const
        {
            sf::Sprite sprite(m_renderTexture.getTexture());
            util::scale(sprite, sf::Vector2f{ m_size });
            sprite.setPosition(position);
            renderTarget.draw(sprite, states);
        }

        void saveToFile(const std::string & filename) const
        {
            if (!m_renderTexture.getTexture().copyToImage().saveToFile(filename))
            {
                std::cout << "Error:  StatDisplay's sf::Image::saveToFile(\"" << filename
                          << "\") failed." << std::endl;
            }
        }

        static void makeAndSavePNG(
            const std::string & filename,
            const std::vector<data_t> & data,
            const sf::Vector2u & size = { 1000, 500 })
        {
            const GraphDisplay<data_t> graphDisplay(data, size);
            graphDisplay.saveToFile(filename);
        }

        bool wasDatasetChanged() const { return m_wasDatasetChanged; }
        const sf::Texture & texture() const { return m_renderTexture.getTexture(); }
        const sf::Vector2u size() const { return m_size; }
        const std::vector<data_t> & data() const { return m_data; }

      private:
        const std::vector<data_t> makeHalfSizeDataSet(const std::vector<data_t> & oldValues) const
        {
            std::vector<data_t> newValues;
            newValues.reserve((oldValues.size() / 2) + 1);

            std::size_t oldIndex = 0;
            while (oldIndex < oldValues.size())
            {
                const data_t a = oldValues.at(oldIndex++);

                if (oldIndex >= oldValues.size())
                {
                    newValues.push_back(a);
                    break;
                }

                const data_t b       = oldValues.at(oldIndex++);
                const data_t average = (a + b) / data_t(2);
                newValues.push_back(average);
            }

            return newValues;
        }

        float calcDataBarWidth() const
        {
            const float width =
                std::floor(static_cast<float>(m_size.x) / static_cast<float>(m_data.size()));

            if (width < 1.0f)
            {
                return 1.0f;
            }
            else
            {
                return width;
            }
        }

        void populateDataBarRects(
            const data_t maxValue, std::vector<sf::FloatRect> & dataBarRects) const
        {
            if (maxValue <= data_t(0))
            {
                return;
            }

            dataBarRects.clear();
            dataBarRects.reserve(m_data.size());

            const float dataBarWidth = calcDataBarWidth();

            float posLeft = 0.0f;
            for (const data_t value : m_data)
            {
                const float valueRatio = (static_cast<float>(value) / static_cast<float>(maxValue));

                sf::FloatRect dataBarRect;
                dataBarRect.left   = posLeft;
                dataBarRect.width  = dataBarWidth;
                dataBarRect.height = (m_size.y * valueRatio);
                dataBarRect.top    = (m_size.y - dataBarRect.height);

                dataBarRects.push_back(dataBarRect);

                posLeft += dataBarRect.width;
            }
        }

        void drawGraph(
            sf::RenderTexture & renderTexture,
            const sf::Color & barColor,
            const sf::Color & backgroundColor,
            const std::vector<sf::FloatRect> & dataBarRects) const
        {
            sf::RectangleShape rectangle;

            for (const sf::FloatRect & dataBarRect : dataBarRects)
            {
                rectangle.setSize(util::size(dataBarRect));
                rectangle.setPosition(util::position(dataBarRect));

                rectangle.setFillColor(barColor);
                rectangle.setOutlineColor(sf::Color::Transparent);
                rectangle.setOutlineThickness(0.0f);

                renderTexture.draw(rectangle);

                // if bars are wide enough then draw dark outline around them because it's pretty
                if (dataBarRect.width > 4.0f)
                {
                    rectangle.setOutlineThickness(1.0f);
                    rectangle.setOutlineColor(backgroundColor);
                    rectangle.setFillColor(sf::Color::Transparent);

                    renderTexture.draw(rectangle);
                }
            }
        }

      private:
        sf::Vector2u m_size;
        std::vector<data_t> m_data;
        sf::RenderTexture m_renderTexture;
        sf::Color m_backgroundColor;
        sf::Color m_dataBarColor;
        sf::Color m_dataBarColorError;
        bool m_wasDatasetChanged;
    };

    //
    //
    //

    template <typename data_t>
    class StatsDisplay : public sf::Drawable
    {
      public:
        StatsDisplay(
            const std::string & title,
            const sf::Font & font,
            const std::vector<data_t> & data,
            const sf::Vector2u & graphSize = { 1000, 500 })
            : m_title(title)
            , m_size()
            , m_data(data)
            , m_renderTexture()
            , m_graphDisplay(data, graphSize)
            , m_backgroundColor(27, 31, 35)
            , m_borderColor(94, 104, 114)
            , m_titleTextColor(245, 246, 249)
            , m_subTitleTextColor(132, 139, 148)
        {
            const sf::Vector2f graphPadRatio(0.1f, 0.25f);
            const sf::Vector2f graphSizeF{ graphSize };
            m_size = (graphSizeF + (graphSizeF * graphPadRatio));
            const sf::Vector2u fullSizeU{ m_size };
            const sf::FloatRect fullRect({ 0.0f, 0.0f }, m_size);
            const sf::FloatRect graphRect((graphSizeF * graphPadRatio * 0.5f), graphSizeF);
            const sf::FloatRect borderRect(1.0f, 1.0f, (m_size.x - 2.0f), (m_size.y - 2.0f));

            sf::FloatRect titleRect(2.0f, 2.0f, (m_size.x - 4.0f), (graphRect.top - 3.0f));
            util::scaleRectInPlace(titleRect, 0.6f);
            sf::Text titleText;
            titleText.setFont(font);
            titleText.setCharacterSize(50);
            titleText.setFillColor(m_titleTextColor);
            titleText.setString(title);
            util::setOriginToPosition(titleText);
            util::fitAndCenterInside(titleText, titleRect);

            const auto stats = util::makeStats(m_data);

            sf::FloatRect subTitleRect(
                2.0f, (util::bottom(graphRect) + 1.0f), titleRect.width, titleRect.height);

            util::scaleRectInPlace(subTitleRect, 0.5f);
            sf::Text subTitleText;
            subTitleText.setFont(font);
            subTitleText.setCharacterSize(50);
            subTitleText.setFillColor(m_subTitleTextColor);
            subTitleText.setString(stats.toString());
            util::setOriginToPosition(subTitleText);
            util::fitAndCenterInside(subTitleText, subTitleRect);

            if (!m_renderTexture.create(fullSizeU.x, fullSizeU.y))
            {
                std::cout << "StatDisplay's sf::RenderTexture::create failed for size=" << fullSizeU
                          << ". So the graph will show solid magenta.  The color of pure evil."
                          << std::endl;

                m_renderTexture.create(32, 32); // this won't fail...probably
                m_renderTexture.clear(sf::Color::Magenta);
                m_renderTexture.display();
                return;
            }

            m_renderTexture.clear(m_backgroundColor);
            m_graphDisplay.draw(util::position(graphRect), m_renderTexture);
            util::drawRectangleShape(m_renderTexture, borderRect, false, m_borderColor);
            m_renderTexture.draw(titleText);
            m_renderTexture.draw(subTitleText);
            m_renderTexture.display();
        }

        const sf::Texture & texture() const { return m_renderTexture.getTexture(); }
        const GraphDisplay<data_t> & graph() const { return m_graphDisplay; }
        const sf::Vector2u size() const { return m_size; }
        const std::vector<data_t> & data() const { return m_data; }

        void saveToFile(const std::string & filename) const
        {
            if (!m_renderTexture.getTexture().copyToImage().saveToFile(filename))
            {
                std::cout << "Error: StatDisplay's sf::Image::saveToFile(\"" << filename
                          << "\") failed." << std::endl;
            }
        }

        static void makeAndSavePNG(
            const std::string & title,
            const sf::Font & font,
            const std::vector<data_t> & data,
            const sf::Vector2u & graphSize = { 1000, 500 })
        {
            const StatsDisplay<data_t> statsDisplay(title, font, data, graphSize);
            statsDisplay.saveToFile(title + ".png");
        }

        void draw(sf::RenderTarget & renderTarget, sf::RenderStates states) const override
        {
            sf::Sprite sprite(m_renderTexture.getTexture());
            util::fit(sprite, m_size);
            renderTarget.draw(sprite, states);
        }

        void draw(
            const sf::Vector2f & position,
            sf::RenderTarget & renderTarget,
            const sf::RenderStates states = {}) const
        {
            sf::Sprite sprite(m_renderTexture.getTexture());
            util::fit(sprite, m_size);
            sprite.setPosition(position);
            renderTarget.draw(sprite, states);
        }

      private:
        std::string m_title;
        sf::Vector2f m_size;
        std::vector<data_t> m_data;
        sf::RenderTexture m_renderTexture;
        GraphDisplay<data_t> m_graphDisplay;
        sf::Color m_backgroundColor;
        sf::Color m_borderColor;
        sf::Color m_titleTextColor;
        sf::Color m_subTitleTextColor;
    };

} // namespace util

#endif // STATSDISPLAY_HPP_INCLUDED
