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
        const std::filesystem::path & MEDIA_PATH,
        const std::string & NAME,
        const std::size_t FRAME_COUNT,
        const float TIME_PER_FRAME_SEC,
        const bool WILL_LOOP)
    {
        m_timePerFrameSec = TIME_PER_FRAME_SEC;
        m_willLoop = WILL_LOOP;

        for (std::size_t i{ 0 }; i < FRAME_COUNT; ++i)
        {
            const std::string FILE_NAME = (NAME + "-" + std::to_string(i).append(".png"));
            const std::string FILE_PATH = (MEDIA_PATH / "image/avatar" / FILE_NAME).string();

            sf::Texture & texture = m_textures.emplace_back();
            texture.loadFromFile(FILE_PATH);
            texture.setSmooth(true);
        }

        m_frameCount = FRAME_COUNT;
    }

    void AvatarAnim::restart()
    {
        m_index = 0;
        m_elapsedTimeSec = 0.0f;
        m_isFinished = false;
    }

    bool AvatarAnim::update(const float FRAME_TIME_SEC)
    {
        m_elapsedTimeSec += FRAME_TIME_SEC;
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
