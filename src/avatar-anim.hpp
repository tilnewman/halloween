#ifndef AVATAR_ANIM_HPP_INCLUDED
#define AVATAR_ANIM_HPP_INCLUDED
//
// avatar-anim.hpp
//
#include <filesystem>
#include <string>
#include <vector>

#include <SFML/Graphics/Texture.hpp>

namespace halloween
{

    class AvatarAnim
    {
      public:
        AvatarAnim();

        void setup(
            const std::filesystem::path & mediaPath,
            const std::string & name,
            const std::size_t frameCount,
            const float timePerFrameSec,
            const bool willLoop);

        void restart();
        inline bool isFinished() const { return m_isFinished; }
        bool update(const float frameTimeSec); // returns true if the texture changed
        inline const sf::Texture & texture() const { return m_textures.at(m_index); }
        inline std::size_t index() const { return m_index; }

      private:
        bool m_willLoop;
        bool m_isFinished;
        float m_elapsedTimeSec;
        float m_timePerFrameSec;
        std::size_t m_index;
        std::size_t m_frameCount;
        std::vector<sf::Texture> m_textures;
    };

} // namespace halloween

#endif // AVATAR_ANIM_HPP_INCLUDED
