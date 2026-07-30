// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// smoke.cpp
//
#include "smoke.hpp"

#include "check-macros.hpp"
#include "context.hpp"
#include "filesystem-util.hpp"
#include "info-region.hpp"
#include "level-stats.hpp"
#include "level.hpp"
#include "random.hpp"
#include "screen-regions.hpp"
#include "settings.hpp"
#include "sfml-util.hpp"
#include "sound-player.hpp"
#include "texture-loader.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

#include <filesystem>

namespace halloween
{

    SmokeDetails::SmokeDetails(const std::string & t_details)
        : type{ SmokeType::Full } // any default works here
        , alpha{ 100 }            // any default works here too
    {
        std::string typeStr;
        std::string alphaStr;

        bool hasReachedComma{ false };
        for (const char ch : t_details)
        {
            if (',' == ch)
            {
                hasReachedComma = true;
                continue;
            }

            if (hasReachedComma)
            {
                alphaStr += ch;
            }
            else
            {
                typeStr += ch;
            }
        }

        if (("full" == typeStr) || ("Full" == typeStr))
        {
            type = SmokeType::Full;
        }
        else if (("still" == typeStr) || ("Still" == typeStr))
        {
            type = SmokeType::Still;
        }
        else if (("top" == typeStr) || ("Top" == typeStr))
        {
            type = SmokeType::Top;
        }
        else
        {
            M_LOG(
                "Unable to parse smoke details \"" << t_details
                                                   << "\" into a \"type,alpha\" string.");
        }

        alpha = static_cast<std::uint8_t>(std::atoi(alphaStr.c_str()));
    }

    //

    Smoke::Smoke()
        : m_textures{}
        , m_animations{}
    {
        // probably only half a dozen in any typical map
        m_animations.reserve(16);
    }

    void Smoke::setup(const Context & t_context)
    {
        const auto paths{ util::findFilesInDirectory(
            (t_context.settings.media_path / "image" / "smoke"), ".png") };

        for (const auto & path : paths)
        {
            sf::Texture & texture{ m_textures.emplace_back() };
            util::TextureLoader::load(texture, path, true);
        }
    }

    void Smoke::add(
        const Context & t_context, const sf::FloatRect & t_region, const std::string & t_details)
    {
        const std::size_t frameIndex{ t_context.random.zeroToOneLessThan(m_textures.size()) };
        sf::Sprite sprite{ m_textures.at(frameIndex) };
        util::scaleAndCenterInside(sprite, t_region);

        const SmokeDetails details{ t_details };
        sprite.setColor(sf::Color(255, 255, 255, details.alpha));

        m_animations.emplace_back(details.type, sprite, frameIndex);
    }

    void Smoke::update(const Context &, const float t_elapsedTimeSec)
    {
        for (SmokeAnim & anim : m_animations)
        {
            if (SmokeType::Still == anim.type)
            {
                continue;
            }

            anim.elapsed_time_sec += t_elapsedTimeSec;
            const float timeBetweenFramesSec{ 0.0875f };
            if (anim.elapsed_time_sec > timeBetweenFramesSec)
            {
                anim.elapsed_time_sec -= timeBetweenFramesSec;

                if (++anim.frame_index >= m_textures.size())
                {
                    anim.frame_index = 0;
                }

                anim.sprite.setTexture(m_textures.at(anim.frame_index));
            }
        }
    }

    void Smoke::draw(
        const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        const sf::FloatRect mapRect{ t_context.layout.mapRegion() };
        for (const SmokeAnim & anim : m_animations)
        {
            if (mapRect.findIntersection(anim.sprite.getGlobalBounds()))
            {
                t_target.draw(anim.sprite, t_states);
            }
        }
    }

    void Smoke::moveWithMap(const sf::Vector2f & t_move)
    {
        for (SmokeAnim & anim : m_animations)
        {
            anim.sprite.move(t_move);
        }
    }

} // namespace halloween
