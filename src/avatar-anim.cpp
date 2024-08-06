// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// avatar-anim.cpp
//
#include "avatar-anim.hpp"

#include "check-macros.hpp"
#include "settings.hpp"

#include "util.hpp"

namespace halloween
{

    AvatarAnim::AvatarAnim()
        : m_willLoop(false)
        , m_isFinished(false)
        , m_elapsedTimeSec(0.0f)
        , m_timePerFrameSec(0.0f)
        , m_index(0)
        , m_frameCount(0)
        , m_textures()
    {}

    void AvatarAnim::setup(
        const std::filesystem::path & mediaPath,
        const std::string & name,
        const std::size_t frameCount,
        const float timePerFrameSec,
        const bool willLoop)
    {
        m_timePerFrameSec = timePerFrameSec;
        m_willLoop = willLoop;

        for (std::size_t i{ 0 }; i < frameCount; ++i)
        {
            const std::string filename = (name + "-" + std::to_string(i).append(".png"));
            const std::string filePath = (mediaPath / filename).string();

            sf::Texture & texture = m_textures.emplace_back();
            texture.loadFromFile(filePath);
            texture.setSmooth(true);
        }

        m_frameCount = frameCount;
    }

    void AvatarAnim::restart()
    {
        m_index = 0;
        m_elapsedTimeSec = 0.0f;
        m_isFinished = false;
    }

    bool AvatarAnim::update(const float frameTimeSec)
    {
        m_elapsedTimeSec += frameTimeSec;
        if (m_elapsedTimeSec < m_timePerFrameSec)
        {
            return false;
        }

        m_elapsedTimeSec -= m_timePerFrameSec;

        ++m_index;
        if (m_index >= m_frameCount)
        {
            if (m_willLoop)
            {
                m_index = 0;
            }
            else
            {
                m_index = (m_frameCount - 1);
                m_isFinished = true;
            }
        }

        return true;
    }

} // namespace halloween
