// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// resources.cpp
//
#include "resources.hpp"

#include "check-macros.hpp"
#include "settings.hpp"
#include "sfml-util.hpp"

namespace halloween
{

    Resources::Resources()
        : font()
        , fps_text("", font, 60)
        , bg_texture1()
        , bg_texture2()
        , bg_texture3()
        , bg_sprite()
        , ground_texture()
        , object_texture1()
        , object_texture2()
    {}

    void Resources::setup(const Settings & settings)
    {
        // font
        font.loadFromFile((settings.media_path / "font/mops-antiqua.ttf").string());
        fps_text.setFont(font);

        // background image
        bg_texture1.loadFromFile((settings.media_path / "image/").string() + "background-1.png");
        bg_texture1.setSmooth(true);

        bg_texture2.loadFromFile((settings.media_path / "image/").string() + "background-2.png");
        bg_texture2.setSmooth(true);

        bg_texture3.loadFromFile((settings.media_path / "image/").string() + "background-3.png");
        bg_texture3.setSmooth(true);

        bg_sprite.setTexture(bg_texture1);

        const std::string imagePath = (settings.media_path / "image/map/").string();

        ground_texture.which = TileImage::Ground;
        ground_texture.texture.loadFromFile(imagePath + "tile-ground.png");
        ground_texture.texture.setSmooth(settings.will_smooth_tile_textures);
        ground_texture.size = sf::Vector2i(ground_texture.texture.getSize());
        ground_texture.gid = 1;

        object_texture1.texture.loadFromFile(imagePath + "tile-object-1.png");
        object_texture1.which = TileImage::Object1;
        object_texture1.texture.setSmooth(settings.will_smooth_tile_textures);
        object_texture1.size = sf::Vector2i(object_texture1.texture.getSize());
        object_texture1.gid = 513;

        object_texture2.texture.loadFromFile(imagePath + "tile-object-2.png");
        object_texture2.which = TileImage::Object2;
        object_texture2.texture.setSmooth(settings.will_smooth_tile_textures);
        object_texture2.size = sf::Vector2i(object_texture2.texture.getSize());
        object_texture2.gid = 257;

        object_texture3.texture.loadFromFile(imagePath + "tile-object-3.png");
        object_texture3.which = TileImage::Object3;
        object_texture3.texture.setSmooth(settings.will_smooth_tile_textures);
        object_texture3.size = sf::Vector2i(object_texture3.texture.getSize());
        object_texture3.gid = 769;
    }

    const sf::Text Resources::makeText(
        unsigned int charSize, const std::string & str, const sf::Color & color) const
    {
        sf::Text text(str, font, charSize);
        text.setFillColor(color);
        util::setOriginToPosition(text);
        return text;
    }

    const TileTexture & Resources::tileTexture(const TileImage image) const
    {
        // clang-format off
        switch (image)
        {
            case TileImage::Ground:  { return ground_texture;  }
            case TileImage::Object1: { return object_texture1; }
            case TileImage::Object2: { return object_texture2; }
            case TileImage::Object3: { return object_texture3; }
            default:
            {
                throw std::runtime_error("Resources::tileTexture() given an invalid image enum.");
            }
        }
        // clang-format on
    }

} // namespace halloween
