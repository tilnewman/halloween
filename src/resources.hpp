#ifndef RESOURCES_HPP_INCLUDED
#define RESOURCES_HPP_INCLUDED
//
// resources.hpp
//
#include "tileset.hpp"

#include <string>

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace halloween
{
    struct Settings;

    //

    struct TileTexture
    {
        TileTexture()
            : which(TileImage::Ground) // any default works here
            , size()
            , texture()
            , gid(0)
        {}

        TileImage which;
        sf::Vector2i size;
        sf::Texture texture;
        int gid;
    };

    struct Resources
    {
        Resources();

        void setup(const Settings & settings);

        const sf::Text
            makeText(unsigned int charSize, const std::string & str, const sf::Color & color) const;

        sf::Font font;
        sf::Text fps_text;

        sf::Texture bg_texture;
        sf::Sprite bg_sprite;

        TileTexture ground_texture;
        TileTexture object_texture1;
        TileTexture object_texture2;

        const TileTexture & tileTexture(const TileImage image)
        {
            // clang-format off
            switch (image)
            {
                case TileImage::Ground:  { return ground_texture;  }
                case TileImage::Object1: { return object_texture1; }
                case TileImage::Object2: 
                default:                 { return object_texture2; }
            }
            // clang-format on
        }
    };

} // namespace halloween

#endif // RESOURCES_HPP_INCLUDED
