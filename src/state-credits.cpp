// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// state-credits.cpp
//
#include "state-credits.hpp"

#include "check-macros.hpp"
#include "context.hpp"
#include "fonts.hpp"
#include "info-region.hpp"
#include "music-player.hpp"
#include "screen-regions.hpp"
#include "settings.hpp"
#include "sfml-defaults.hpp"
#include "sfml-util.hpp"
#include "sound-player.hpp"
#include "state-machine.hpp"
#include "texture-loader.hpp"

#include <string>

#include <SFML/Window/Event.hpp>

namespace halloween
{

    CreditAnim::CreditAnim(
        const Context & t_context,
        const std::string & t_imageFilePath,
        const float t_imageScale,
        const std::string & t_name,
        const std::string & t_description,
        const float t_vertPos)
        : texture{}
        , sprite{ texture }
        , name{ t_context.fonts.makeText(Font::Title, 60, t_name, sf::Color(220, 220, 220)) }
        , description{ t_context.fonts.makeText(
              Font::General, 30, t_description, sf::Color(160, 160, 160)) }
    {
        util::TextureLoader::load(texture, t_imageFilePath, true);

        sprite.setTexture(texture, true);
        sprite.scale({ t_imageScale, t_imageScale });

        const sf::FloatRect screenRect{ t_context.layout.wholeRegion() };

        sprite.setPosition(
            { (util::center(screenRect).x - (sprite.getGlobalBounds().size.x * 0.5f)), t_vertPos });

        //

        // this is the vertical empty space between images and lines of text
        const float vertPad{ screenRect.size.y * 0.015f };

        name.setPosition(
            { (util::center(screenRect).x - (name.getGlobalBounds().size.x * 0.5f)),
              (util::bottom(sprite) + vertPad) });

        description.setPosition(
            { (util::center(screenRect).x - (description.getGlobalBounds().size.x * 0.5f)),
              (util::bottom(name) + (vertPad * 0.35f)) });
    }

    void CreditAnim::move(const float t_amount)
    {
        sprite.move({ 0.0f, t_amount });
        name.move({ 0.0f, t_amount });
        description.move({ 0.0f, t_amount });
    }

    float CreditAnim::bottom() const { return util::bottom(description); }

    void CreditAnim::draw(sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        t_target.draw(sprite, t_states);
        t_target.draw(name, t_states);
        t_target.draw(description, t_states);
    }

    //

    StateCredits::StateCredits()
        : StateBase{ State::Credits, State::Quit }
        , m_titleText{ util::SfmlDefaults::instance().font() }
        , m_credits{}
        , m_bgTexture{}
        , m_bgSprite{ m_bgTexture }
    {}

    void StateCredits::update(const Context & t_context, const float t_frameTimeSec)
    {
        const float speed{ 45.0f };
        const float moveAmount{ -1.0f * speed * t_frameTimeSec };
        m_titleText.move({ 0.0f, moveAmount });

        for (CreditAnim & anim : m_credits)
        {
            anim.move(moveAmount);
        }

        if (m_credits.back().bottom() < -50.0f)
        {
            t_context.state.setChangePending(State::Quit);
        }
    }

    void StateCredits::draw(
        const Context &, sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        t_target.draw(m_bgSprite, t_states);
        t_target.draw(m_titleText, t_states);

        for (const CreditAnim & anim : m_credits)
        {
            anim.draw(t_target, t_states);
        }
    }

    bool StateCredits::handleEvent(const Context & t_context, const sf::Event & t_event)
    {
        if (t_event.is<sf::Event::KeyPressed>())
        {
            t_context.state.setChangePending(State::Quit);
        }

        return t_context.state.isChangePending();
    }

    void StateCredits::onEnter(const Context & t_context)
    {
        const sf::FloatRect screenRect{ t_context.layout.wholeRegion() };

        //
        util::TextureLoader::load(
            m_bgTexture, (t_context.settings.media_path / "image" / "straw-mat.png"), true);

        m_bgTexture.setRepeated(true);

        m_bgSprite.setTexture(m_bgTexture, true);
        m_bgSprite.setTextureRect(sf::IntRect({ 0, 0 }, sf::Vector2i{ screenRect.size }));
        m_bgSprite.setColor(sf::Color(255, 255, 255, 92));

        //
        const float vertPad{ screenRect.size.y * 0.11f };

        m_titleText =
            t_context.fonts.makeText(Font::Title, 120, "Credits", sf::Color(220, 220, 220));

        m_titleText.setPosition(
            { (util::center(screenRect).x - (m_titleText.getGlobalBounds().size.x * 0.5f)),
              screenRect.size.y });

        //
        m_credits.reserve(16);

        m_credits.emplace_back(
            t_context,
            (t_context.settings.media_path / "image" / "credit" / "cpp.png").string(),
            0.5f,
            "Ziesche Til Newman",
            "Software (C++, SFML, CMake)",
            (util::bottom(m_titleText) + vertPad));

        m_credits.emplace_back(
            t_context,
            (t_context.settings.media_path / "image" / "credit" / "sfml.png").string(),
            0.85f,
            "Simple Fast Multimedia Library",
            "Thanks to Laurent Gomila for this amazing library!",
            m_credits.back().bottom() + vertPad);

        m_credits.emplace_back(
            t_context,
            (t_context.settings.media_path / "image" / "credit" / "ninja-girl-sprites.png")
                .string(),
            0.9f,
            "www.gameart2d.com",
            "A free sprite set with all kinds of spunky energy.",
            m_credits.back().bottom() + vertPad);

        m_credits.emplace_back(
            t_context,
            (t_context.settings.media_path / "image" / "credit" / "graveyard-tiles.png").string(),
            0.9f,
            "www.gameart2d.com",
            "A free spooky graveyard tileset I made heavy use of.",
            m_credits.back().bottom() + vertPad);

        m_credits.emplace_back(
            t_context,
            (t_context.settings.media_path / "image" / "credit" / "tiled.png").string(),
            0.9f,
            "www.mapeditor.org",
            "A simple, easy, and free map editor.",
            m_credits.back().bottom() + vertPad);

        m_credits.emplace_back(
            t_context,
            (t_context.settings.media_path / "image" / "credit" / "freesound.png").string(),
            0.95f,
            "www.freesound.org Website",
            "My secret weapon in the hunt for sound effects.",
            m_credits.back().bottom() + vertPad);

        m_credits.emplace_back(
            t_context,
            (t_context.settings.media_path / "image" / "credit" / "2d-platformer-kit.png").string(),
            0.75f,
            "2d Platformer Forest Kit",
            "I used a bunch of images from this excellent kit.",
            m_credits.back().bottom() + vertPad);

        m_credits.emplace_back(
            t_context,
            (t_context.settings.media_path / "image" / "credit" / "ryzom-textures.png").string(),
            0.75f,
            "Ryzom Textures Pack",
            "I used two of these fantastic seamless tile images.",
            m_credits.back().bottom() + vertPad);

        m_credits.emplace_back(
            t_context,
            (t_context.settings.media_path / "image" / "credit" / "font.png").string(),
            0.5f,
            "Mops Antiqua Font",
            "Uwe Borchert (SIL Open Font License)",
            m_credits.back().bottom() + vertPad);

        m_credits.emplace_back(
            t_context,
            (t_context.settings.media_path / "image" / "credit" / "font.png").string(),
            0.5f,
            "Gentium Plus",
            "J. Victor Gaultney, Annie Olsen, Iska Routamaa, Becca Hirsbrunner (SIL Open Font)",
            m_credits.back().bottom() + vertPad);

        t_context.music.start("credits.ogg");
    }

    void StateCredits::onExit(const Context &) {}
} // namespace halloween
