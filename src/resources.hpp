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

    struct Context;
    struct Settings;

    //

    class Resources
    {
      public:
        Resources();

        void setup(const Settings & t_settings);

        const sf::Text makeText(
            const unsigned int t_charSize,
            const std::string & t_message,
            const sf::Color & t_color) const;

        const MapTexture & mapTexture(const TileImage t_image) const;

        constexpr void setGidGround(const int t_gid) { m_mapTextureGround.gid = t_gid; }
        constexpr void setGidObject1(const int t_gid) { m_mapTextureObject1.gid = t_gid; }
        constexpr void setGidObject2(const int t_gid) { m_mapTextureObject2.gid = t_gid; }
        constexpr void setGidObject3(const int t_gid) { m_mapTextureObject3.gid = t_gid; }
        constexpr void setGidJungleMisc(const int t_gid) { m_mapTextureJungleMisc.gid = t_gid; }
        constexpr void setGidJungleTrees(const int t_gid) { m_mapTextureJungleTrees.gid = t_gid; }

        constexpr void setGidJungleTreesFlip(const int t_gid)
        {
            m_mapTextureJungleTreesFlip.gid = t_gid;
        }

      private:
        sf::Font m_font;
        MapTexture m_mapTextureGround;
        MapTexture m_mapTextureObject1;
        MapTexture m_mapTextureObject2;
        MapTexture m_mapTextureObject3;
        MapTexture m_mapTextureJungleTrees;
        MapTexture m_mapTextureJungleTreesFlip;
        MapTexture m_mapTextureJungleMisc;
    };

} // namespace halloween

#endif // RESOURCES_HPP_INCLUDED
