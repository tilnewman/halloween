#ifndef MUSIC_PLAYER_HPP_INCLUDED
#define MUSIC_PLAYER_HPP_INCLUDED
//
// msuic-player.hpp
//
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include <SFML/Audio/Music.hpp>

namespace util
{
    class MusicPlayer
    {
      public:
        MusicPlayer();
        ~MusicPlayer();

        void setup(const std::filesystem::path & path);

        void reset();

        void start(const std::string & filename, const float volume = 100.0f);

        void pause(const std::string & filename);
        void pauseAll();

        void stop(const std::string & filename);
        void stopAll();

        float volume(const std::string & filename) const;
        void volume(const std::string & filename, const float newVolume);

      private:
        struct Entry
        {
            std::string filename{};
            sf::Music music{};
        };

      private:
        std::filesystem::path m_path;
        std::vector<std::unique_ptr<Entry>> m_entrys;
    };
} // namespace util

#endif // MUSIC_PLAYER_HPP_INCLUDED
