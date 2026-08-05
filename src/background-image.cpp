// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// background-image.cpp
//
#include "background-image.hpp"

#include "context.hpp"
#include "screen-regions.hpp"
#include "settings.hpp"
#include "sfml-util.hpp"
#include "texture-loader.hpp"

#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

namespace halloween
{

    BackgroundImage::BackgroundImage()
        : m_texture{}
        , m_sprite{ m_texture }
    {}

    void BackgroundImage::setup(const Context & t_context, const int t_backgroundImageNumber)
    {
        const std::string filename{ std::to_string(t_backgroundImageNumber).append(".png") };
        const auto path{ t_context.settings.media_path / "image" / "background" / filename };
        util::TextureLoader::load(m_texture, path, true);
        m_sprite.setTexture(m_texture, true);
        util::growAndCenterInside(m_sprite, t_context.layout.wholeRegion());
    }

    void BackgroundImage::draw(sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        t_target.draw(m_sprite, t_states);
    }

} // namespace halloween
