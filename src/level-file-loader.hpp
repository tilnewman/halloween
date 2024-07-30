#ifndef LEVEL_FILE_LOADER_HPP_INCLUDED
#define LEVEL_FILE_LOADER_HPP_INCLUDED
//
// level-file-loader.hpp
//
#include "json.hpp"
#include "tileset.hpp"

#include <string>
#include <vector>

#include <SFML/Graphics/Rect.hpp>

namespace halloween
{
    struct Context;
    struct Settings;
    using Json = nlohmann::json;

    //

    class LevelFileLoader
    {
      public:
        LevelFileLoader();

        void load(Context & context, const std::size_t levelNumber);

      private:
        static void parseLevelDetails(Context & context, Json & json);

        void parseLayers(Context & context, Json & json);

        void parseTileLayer(Context & context, const TileImage image, Json & json);

        static void
            parseRectLayer(Context & context, Json & json, std::vector<sf::FloatRect> & rects);

        static const sf::FloatRect parseAndConvertRect(const Context & context, Json & json);

        void parseSpawnLayer(Context & context, Json & json);

        static void parseCoinLayer(Context & context, Json & json);

      private:
        std::string m_path;
    };

} // namespace halloween

#endif // LEVEL_FILE_LOADER_HPP_INCLUDED
