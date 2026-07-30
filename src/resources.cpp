// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// resources.cpp
//
#include "resources.hpp"

#include "check-macros.hpp"
#include "settings.hpp"
#include "sfml-util.hpp"
#include "texture-loader.hpp"

namespace halloween
{

    Resources::Resources()
        : m_font{}
        , bg_texture1{}
        , bg_texture2{}
        , bg_texture3{}
        , bg_sprite{ bg_texture1 }
        , ground_texture{}
        , object_texture1{}
        , object_texture2{}
        , object_texture3{}
    {}

    void Resources::setup(const Settings & t_settings)
    {
        // font
        if (!m_font.openFromFile((t_settings.media_path / "font" / "mops-antiqua.ttf").string()))
        {
            std::cout << "Failed to load font mops-antiqua.ttf!\n";
        }

        // background image
        util::TextureLoader::load(
            bg_texture1, (t_settings.media_path / "image" / "background-1.png"), true);

        util::TextureLoader::load(
            bg_texture2, (t_settings.media_path / "image" / "background-2.png"), true);

        util::TextureLoader::load(
            bg_texture3, (t_settings.media_path / "image" / "background-3.png"), true);

        bg_sprite.setTexture(bg_texture1, true);

        const std::string imagePath{ (t_settings.media_path / "image" / "map/").string() };

        util::TextureLoader::load(ground_texture.texture, (imagePath + "tile-ground.png"));
        ground_texture.which = TileImage::Ground;
        ground_texture.size = sf::Vector2i(ground_texture.texture.getSize());

        util::TextureLoader::load(object_texture1.texture, (imagePath + "tile-object-1.png"));
        object_texture1.which = TileImage::Object1;
        object_texture1.size = sf::Vector2i(object_texture1.texture.getSize());

        util::TextureLoader::load(object_texture2.texture, (imagePath + "tile-object-2.png"));
        object_texture2.which = TileImage::Object2;
        object_texture2.size = sf::Vector2i(object_texture2.texture.getSize());

        util::TextureLoader::load(object_texture3.texture, (imagePath + "tile-object-3.png"));
        object_texture3.which = TileImage::Object3;
        object_texture3.size = sf::Vector2i(object_texture3.texture.getSize());

        util::TextureLoader::load(object_texture4.texture, (imagePath + "tile-object-4.png"));
        object_texture4.which = TileImage::Object4;
        object_texture4.size = sf::Vector2i(object_texture4.texture.getSize());

        util::TextureLoader::load(object_texture5.texture, (imagePath + "tile-object-5.png"));
        object_texture5.which = TileImage::Object5;
        object_texture5.size = sf::Vector2i(object_texture5.texture.getSize());
    }

    const sf::Text Resources::makeText(
        const unsigned int t_charSize,
        const std::string & t_message,
        const sf::Color & t_color) const
    {
        sf::Text text(m_font, t_message, t_charSize);
        text.setFillColor(t_color);
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
            case TileImage::Object4: { return object_texture4; }
            case TileImage::Object5: { return object_texture5; }
            default:
            {
                throw std::runtime_error("Resources::tileTexture() given an invalid TileImage enum.");
            }
        }
        // clang-format on
    }

} // namespace halloween
