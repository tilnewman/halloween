// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// music-player.cpp
//
#include "music-player.hpp"

#include <algorithm>
#include <iostream>

namespace util
{
    MusicPlayer::MusicPlayer()
        : m_path()
        , m_entrys()
    {}

    MusicPlayer::~MusicPlayer() { reset(); }

    void MusicPlayer::setup(const std::filesystem::path & path)
    {
        m_path = path;

        if (!std::filesystem::exists(m_path) || !std::filesystem::is_directory(m_path))
        {
            std::cerr << "MusicPlayer::setup() given a path that is not an existing directory: "
                      << m_path << std::endl;
        }
    }

    void MusicPlayer::reset()
    {
        stopAll();
        m_entrys.clear();
    }

    void MusicPlayer::start(const std::string & filename, const float volume)
    {
        const std::filesystem::path path = (m_path / filename);
        if (!std::filesystem::exists(path) || !std::filesystem::is_regular_file(path))
        {
            std::cerr << "MusicPlayer::start(\"" << filename << "\", " << volume
                      << ") -but that file does not exist!" << m_path << std::endl;

            return;
        }

        // start if already loaded
        for (auto & entryUPtr : m_entrys)
        {
            if (entryUPtr->filename == filename)
            {
                entryUPtr->music.setVolume(volume);

                if (entryUPtr->music.getStatus() != sf::SoundSource::Status::Playing)
                {
                    entryUPtr->music.setLooping(true);
                    entryUPtr->music.play();
                }

                return;
            }
        }

        // load and start new
        auto & entryUPtr = m_entrys.emplace_back(std::make_unique<Entry>());

        if (!entryUPtr->music.openFromFile(path.string()))
        {
            std::cerr << "MusicPlayer::start(\"" << filename << "\", " << volume
                      << ") found the file but was unable to load it." << m_path << std::endl;

            m_entrys.pop_back();
            return;
        }

        entryUPtr->filename = filename;
        entryUPtr->music.setVolume(volume);
        entryUPtr->music.setLooping(true);
        entryUPtr->music.play();
    }

    void MusicPlayer::pause(const std::string & filename)
    {
        for (auto & entryUPtr : m_entrys)
        {
            if (entryUPtr->filename == filename)
            {
                if (entryUPtr->music.getStatus() == sf::SoundSource::Status::Paused)
                {
                    entryUPtr->music.play();
                }
                else
                {
                    entryUPtr->music.pause();
                }

                return;
            }
        }
    }

    void MusicPlayer::pauseAll()
    {
        for (auto & entryUPtr : m_entrys)
        {
            if (entryUPtr->music.getStatus() == sf::SoundSource::Status::Paused)
            {
                entryUPtr->music.play();
            }
            else
            {
                entryUPtr->music.pause();
            }
        }
    }

    void MusicPlayer::stop(const std::string & filename)
    {
        for (auto & entryUPtr : m_entrys)
        {
            if (entryUPtr->filename == filename)
            {
                entryUPtr->music.stop();
                return;
            }
        }
    }

    void MusicPlayer::stopAll()
    {
        for (auto & entryUPtr : m_entrys)
        {
            entryUPtr->music.stop();
        }
    }

    float MusicPlayer::volume(const std::string & filename) const
    {
        for (auto & entryUPtr : m_entrys)
        {
            if (entryUPtr->filename == filename)
            {
                return entryUPtr->music.getVolume();
            }
        }

        std::cerr << "MusicPlayer::getVolume(\"" << filename
                  << "\") -but there are none with that filename!" << m_path << std::endl;

        return 0.0f;
    }

    void MusicPlayer::volume(const std::string & filename, const float newVolume)
    {
        for (auto & entryUPtr : m_entrys)
        {
            if (entryUPtr->filename == filename)
            {
                entryUPtr->music.setVolume(newVolume);
                return;
            }
        }

        std::cerr << "MusicPlayer::setVolume(\"" << filename << "\", " << newVolume
                  << ") -but there are none with that filename!" << m_path << std::endl;
    }

} // namespace util
