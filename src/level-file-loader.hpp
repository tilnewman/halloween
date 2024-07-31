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

        bool load(Context & context);

      private:
        void parseLevelDetails(Context & c, Json & json);
        void parseLayers(Context & c, Json & json);
        void parseTileLayer(Context & c, const TileImage image, Json & json);
        void parseRectLayer(Context & c, Json & json, std::vector<sf::FloatRect> & rects);
        const sf::FloatRect parseAndConvertRect(const Context & c, Json & json);
        void parseSpawnLayer(Context & c, Json & json);
        void parseCoinLayer(Context & c, Json & json);
        void parseGhostLayer(Context & c, Json & json);
        void parseSlimeGreenLayer(Context & c, Json & json);
        void parseSlimeOrangeLayer(Context & c, Json & json);

      private:
        std::string m_pathStr;
    };

} // namespace halloween

#endif // LEVEL_FILE_LOADER_HPP_INCLUDED
