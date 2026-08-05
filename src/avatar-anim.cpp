// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// avatar-anim.cpp
//
#include "avatar-anim.hpp"

#include "check-macros.hpp"
#include "filesystem-util.hpp"
#include "settings.hpp"
#include "texture-loader.hpp"

#include "util.hpp"

namespace halloween
{

    AvatarAnim::AvatarAnim()
        : m_willLoop{ false }
        , m_isFinished{ false }
        , m_elapsedTimeSec{ 0.0f }
        , m_timePerFrameSec{ 0.0f }
        , m_index{ 0 }
        , m_textures{}
    {}

    void AvatarAnim::setup(
        const std::filesystem::path & t_mediaPath,
        const std::string & t_name,
        const float t_timePerFrameSec,
        const bool t_willLoop)
    {
        m_timePerFrameSec = t_timePerFrameSec;
        m_willLoop = t_willLoop;

        const auto dirPath{ t_mediaPath / t_name };
        const auto imagePaths{ util::findFilesInDirectory(dirPath, ".png") };
        M_CHECK(not imagePaths.empty(), "Failed to find any PNG images in: " << dirPath);

        for (const auto & path : imagePaths)
        {
            util::TextureLoader::load(m_textures.emplace_back(), path);
        }
    }

    void AvatarAnim::restart()
    {
        m_index = 0;
        m_elapsedTimeSec = 0.0f;
        m_isFinished = false;
    }

    bool AvatarAnim::update(const float t_frameTimeSec)
    {
        m_elapsedTimeSec += t_frameTimeSec;
        if (m_elapsedTimeSec < m_timePerFrameSec)
        {
            return false;
        }

        m_elapsedTimeSec -= m_timePerFrameSec;

        ++m_index;
        if (m_index >= m_textures.size())
        {
            if (m_willLoop)
            {
                m_index = 0;
            }
            else
            {
                m_index = (m_textures.size() - 1);
                m_isFinished = true;
            }
        }

        return true;
    }

} // namespace halloween
