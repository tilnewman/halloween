#ifndef AVATAR_ANIM_HPP_INCLUDED
#define AVATAR_ANIM_HPP_INCLUDED
//
// avatar-anim.hpp
//
#include <filesystem>
#include <string>
#include <vector>

#include <SFML/Graphics/Texture.hpp>

namespace slnghn
{

    class AvatarAnim
    {
      public:
        AvatarAnim();

        void setup(
            const std::filesystem::path & MEDIA_PATH,
            const std::string & NAME,
            const std::size_t FRAME_COUNT,
            const float TIME_PER_FRAME_SEC,
            const bool WILL_LOOP);

        void restart();

        bool isFinished() const { return m_isFinished; }

        // return true if the texture changed
        bool update(const float FRAME_TIME_SEC);

        const sf::Texture & texture() const { return m_textures.at(m_index); }

      private:
        bool m_willLoop;
        bool m_isFinished;
        float m_elapsedTimeSec;
        float m_timePerFrameSec;
        std::size_t m_index;
        std::size_t m_frameCount;
        std::vector<sf::Texture> m_textures;
    };

} // namespace slnghn

#endif // AVATAR_ANIM_HPP_INCLUDED
