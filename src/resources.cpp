// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// resources.cpp
//
#include "resources.hpp"

#include "check-macros.hpp"
#include "context.hpp"
#include "screen-regions.hpp"
#include "settings.hpp"
#include "sfml-util.hpp"
#include "texture-loader.hpp"

namespace halloween
{

    Resources::Resources()
        : m_font{}
        , m_mapTextureGround{}
        , m_mapTextureObject1{}
        , m_mapTextureObject2{}
        , m_mapTextureObject3{}
        , m_mapTextureJungleTrees{}
        , m_mapTextureJungleTreesFlip{}
        , m_mapTextureJungleMisc{}
        , m_bgTexture1{}
        , m_bgTexture2{}
        , m_bgTexture3{}
        , m_bgSprite{ m_bgTexture1 }
    {}

    void Resources::setup(const Settings & t_settings)
    {
        // font
        const std::string fontPathStr{
            (t_settings.media_path / "font" / "mops-antiqua.ttf").string()
        };

        const bool fontLoadSuccess{ m_font.openFromFile(fontPathStr) };
        M_CHECK(fontLoadSuccess, "Failed to load font: " << fontPathStr);

        // background image
        util::TextureLoader::load(
            m_bgTexture1, (t_settings.media_path / "image" / "background-1.png"), true);

        util::TextureLoader::load(
            m_bgTexture2, (t_settings.media_path / "image" / "background-2.png"), true);

        util::TextureLoader::load(
            m_bgTexture3, (t_settings.media_path / "image" / "background-3.png"), true);

        m_bgSprite.setTexture(m_bgTexture1, true);

        //
        const auto imagePath{ t_settings.media_path / "image" / "map" };
        m_mapTextureGround.setup(TileImage::Ground, imagePath);
        m_mapTextureObject1.setup(TileImage::Object1, imagePath);
        m_mapTextureObject2.setup(TileImage::Object2, imagePath);
        m_mapTextureObject3.setup(TileImage::Object3, imagePath);
        m_mapTextureJungleTrees.setup(TileImage::JungleTrees, imagePath);
        m_mapTextureJungleTreesFlip.setup(TileImage::JungleTreesFlip, imagePath);
        m_mapTextureJungleMisc.setup(TileImage::JungleMisc, imagePath);
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

    const MapTexture & Resources::mapTexture(const TileImage image) const
    {
        // clang-format off
        switch (image)
        {
            case TileImage::Ground:          { return m_mapTextureGround;          }
            case TileImage::Object1:         { return m_mapTextureObject1;         }
            case TileImage::Object2:         { return m_mapTextureObject2;         }
            case TileImage::Object3:         { return m_mapTextureObject3;         }
            case TileImage::JungleTrees:     { return m_mapTextureJungleTrees;     }
            case TileImage::JungleTreesFlip: { return m_mapTextureJungleTreesFlip; }
            case TileImage::JungleMisc:      { return m_mapTextureJungleMisc;      }
            default:
            {
                throw std::runtime_error("Resources::tileTexture() given an invalid TileImage enum.");
            }
        }
        // clang-format on
    }

    void Resources::setupBackgroundSprite(const Context & t_context, const int t_bgImageNumber)
    {
        if (1 == t_bgImageNumber)
        {
            m_bgSprite.setTexture(m_bgTexture1);
        }
        else if (2 == t_bgImageNumber)
        {
            m_bgSprite.setTexture(m_bgTexture2);
        }
        else if (3 == t_bgImageNumber)
        {
            m_bgSprite.setTexture(m_bgTexture3);
        }
        else
        {
            M_LOG(
                "Error: This map file has an invalid custom background property value="
                << t_bgImageNumber << ", so the default background image 1 will be used.");

            m_bgSprite.setTexture(m_bgTexture1);
        }

        util::growAndCenterInside(m_bgSprite, t_context.layout.wholeRegion());
    }

} // namespace halloween
