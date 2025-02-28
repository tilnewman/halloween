#ifndef TEXTURE_LOADER_HPP_INCLUDED
#define TEXTURE_LOADER_HPP_INCLUDED
//
// texture-loader.hpp
//
#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Image.hpp>

namespace util
{

    class TextureLoader
    {
      public:
        static void load(
            sf::Texture & t_texture,
            const std::filesystem::path & t_path,
            const bool t_willSmooth = false)
        {
            load(t_texture, t_path.string(), t_willSmooth);
        }

        static void load(
            sf::Texture & t_texture, const std::string & t_pathStr, const bool t_willSmooth = false)
        {
            if (t_texture.loadFromFile(t_pathStr))
            {
                t_texture.setSmooth(t_willSmooth);

                ++m_fileCount;

                m_byteCount +=
                    static_cast<std::size_t>(t_texture.getSize().x * t_texture.getSize().y * 4u);
            }
            else
            {
                // SFML already prints a very nice message including paths upon failure

                static const sf::Image image({64,64}, sf::Color::Red);
                if (!t_texture.loadFromImage(image))
                {
                    std::clog << "Not even the default solid red image could be loaded!\n";
                }
            }
        }

        static inline std::size_t filesLoaded() { return m_fileCount; }
        static inline std::size_t bytesLoaded() { return m_byteCount; }

        static void dumpInfo()
        {
            std::ostringstream ss;
            ss.imbue(std::locale("")); // this is only to put commas in the big numbers
            ss << m_fileCount << " texture files loaded for a total of " << m_byteCount << " bytes.";
            std::clog << ss.str() << '\n';
        }

      private:
        static inline std::size_t m_fileCount{ 0 };
        static inline std::size_t m_byteCount{ 0 };
    };

} // namespace util

#endif // TEXTURE_LOADER_HPP_INCLUDED
