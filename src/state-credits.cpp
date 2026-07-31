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

#include <string>

#include <SFML/Window/Event.hpp>

namespace halloween
{

    Credit::Credit(
        const Context & t_context,
        const std::string & t_name,
        const std::string & t_desc,
        const std::string & t_license,
        const std::string & t_extra)
        : m_nameText{ util::SfmlDefaults::instance().font() }
        , m_descText{ util::SfmlDefaults::instance().font() }
        , m_licenseText{ util::SfmlDefaults::instance().font() }
        , m_extraText{ util::SfmlDefaults::instance().font() }
    {
        const sf::FloatRect screenRect{ t_context.layout.wholeRegion() };

        const sf::Color textColor(220, 220, 220);

        m_nameText = t_context.fonts.makeText(55, t_name, textColor);

        m_nameText.setPosition(
            { ((screenRect.size.x * 0.5f) - (m_nameText.getGlobalBounds().size.x * 0.5f)),
              screenRect.size.y });

        m_descText = t_context.fonts.makeText(40, t_desc, textColor);

        m_descText.setPosition(
            { ((screenRect.size.x * 0.5f) - (m_descText.getGlobalBounds().size.x * 0.5f)),
              util::bottom(m_nameText) + m_vertPad });

        m_licenseText = t_context.fonts.makeText(25, t_license, textColor);

        m_licenseText.setPosition(
            { ((screenRect.size.x * 0.5f) - (m_licenseText.getGlobalBounds().size.x * 0.5f)),
              util::bottom(m_descText) + m_vertPad });

        m_extraText = t_context.fonts.makeText(25, t_extra, textColor);

        m_extraText.setPosition(
            { ((screenRect.size.x * 0.5f) - (m_extraText.getGlobalBounds().size.x * 0.5f)),
              util::bottom(m_licenseText) + m_vertPad });
    }

    void Credit::update(const Context & t_context, const float t_frameTimeSec)
    {
        const float scrollSpeed{ t_context.settings.credits_scroll_speed };
        m_nameText.move({ 0.0f, -(t_frameTimeSec * scrollSpeed) });
        m_descText.move({ 0.0f, -(t_frameTimeSec * scrollSpeed) });
        m_licenseText.move({ 0.0f, -(t_frameTimeSec * scrollSpeed) });
        m_extraText.move({ 0.0f, -(t_frameTimeSec * scrollSpeed) });
    }

    void Credit::draw(sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        t_target.draw(m_nameText, t_states);
        t_target.draw(m_descText, t_states);
        t_target.draw(m_licenseText, t_states);
        t_target.draw(m_extraText, t_states);
    }

    void Credit::vertPosition(const float t_position)
    {
        m_nameText.setPosition({ m_nameText.getGlobalBounds().position.x, t_position });

        m_descText.setPosition(
            { m_descText.getGlobalBounds().position.x, util::bottom(m_nameText) + m_vertPad });

        m_licenseText.setPosition(
            { m_licenseText.getGlobalBounds().position.x, util::bottom(m_descText) + m_vertPad });

        m_extraText.setPosition(
            { m_extraText.getGlobalBounds().position.x, util::bottom(m_licenseText) + m_vertPad });
    }

    float Credit::bottom() const { return util::bottom(m_licenseText); }

    //

    StateCredits::StateCredits()
        : StateBase{ State::Credits, State::Quit }
        , m_credits{}
    {}

    void StateCredits::onEnter(const Context & t_context)
    {
        t_context.music.start("credits.ogg", 20.0f);

        const sf::FloatRect screenRect{ t_context.layout.wholeRegion() };

        Credit & softwareCredit{ m_credits.emplace_back(
            t_context, "Ziesche Til Newman", "Software") };

        Credit & fontCredit{ m_credits.emplace_back(
            t_context,
            "Mops Antiqua",
            "Font",
            "SIL Open Font License 1.1",
            "www.scripts.sil.org/ofl") };

        const float vertSpacer{ screenRect.size.y * 0.125f };
        fontCredit.vertPosition(softwareCredit.bottom() + vertSpacer);

        Credit & sfmlCredit{ m_credits.emplace_back(
            t_context,
            "SFML",
            "Simple Fast Multimedia Library",
            "Special thanks to Laurent Gomila for this excellent library!",
            "www.sfml-dev.org") };

        sfmlCredit.vertPosition(fontCredit.bottom() + vertSpacer);

        Credit & tiledCredit{ m_credits.emplace_back(
            t_context,
            "Tiled",
            "An amazing free 2D level editor that is easy and intuitive.",
            "www.mapeditor.org",
            "") };

        tiledCredit.vertPosition(sfmlCredit.bottom() + vertSpacer);

        Credit & tilesetCredit{ m_credits.emplace_back(
            t_context, "Free Graveyard Tileset", "www.gameart2d.com", "", "") };

        tilesetCredit.vertPosition(tiledCredit.bottom() + vertSpacer);

        Credit & spritesheetCredit{ m_credits.emplace_back(
            t_context, "Ninja Girl Free Sprites", "www.gameart2d.com", "", "") };

        spritesheetCredit.vertPosition(tilesetCredit.bottom() + vertSpacer);

        Credit & forestPlatformCredit{ m_credits.emplace_back(
            t_context,
            "2d Platformer Forest Kit",
            "www.opengameart.org/content/2d-platformer-forest-kit",
            "Thanks to ArtByte for this great looking art!",
            "") };

        forestPlatformCredit.vertPosition(spritesheetCredit.bottom() + vertSpacer);
    }

    void StateCredits::onExit(const Context & t_context) { t_context.music.stop("credits.ogg"); }

    void StateCredits::update(const Context & t_context, const float t_frameTimeSec)
    {
        for (Credit & credit : m_credits)
        {
            credit.update(t_context, t_frameTimeSec);
        }

        Credit & lastCredit{ m_credits.back() };
        if (lastCredit.bottom() < 0.0f)
        {
            t_context.state.setChangePending(State::Quit);
        }
    }

    void StateCredits::draw(
        const Context &, sf::RenderTarget & t_target, sf::RenderStates & t_states) const
    {
        for (const Credit & credit : m_credits)
        {
            t_target.draw(credit, t_states);
        }
    }

    bool StateCredits::handleEvent(const Context & t_context, const sf::Event & t_event)
    {
        // any keypress or mouse click will exit
        if (t_event.is<sf::Event::MouseButtonPressed>() || t_event.is<sf::Event::KeyPressed>())
        {
            t_context.state.setChangePending(State::Quit);
        }

        return false;
    }

} // namespace halloween
