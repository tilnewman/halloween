// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// state-credits.cpp
//
#include "state-credits.hpp"

#include "check-macros.hpp"
#include "context.hpp"
#include "info-region.hpp"
#include "music-player.hpp"
#include "resources.hpp"
#include "screen-regions.hpp"
#include "sfml-defaults.hpp"
#include "sfml-util.hpp"
#include "sound-player.hpp"
#include "state-machine.hpp"

#include <string>

#include <SFML/Window/Event.hpp>

namespace halloween
{

    const float Credit::m_vertPad{ 10.0f };

    Credit::Credit(
        const Context & context,
        const std::string & name,
        const std::string & desc,
        const std::string & license,
        const std::string & extra)
        : m_nameText(util::SfmlDefaults::instance().font())
        , m_descText(util::SfmlDefaults::instance().font())
        , m_licenseText(util::SfmlDefaults::instance().font())
        , m_extraText(util::SfmlDefaults::instance().font())
    {
        const sf::FloatRect screenRect = context.layout.wholeRegion();

        const sf::Color textColor(220, 220, 220);

        m_nameText = context.media.makeText(55, name, textColor);

        m_nameText.setPosition(
            { ((screenRect.size.x * 0.5f) - (m_nameText.getGlobalBounds().size.x * 0.5f)),
              screenRect.size.y });

        m_descText = context.media.makeText(40, desc, textColor);

        m_descText.setPosition(
            { ((screenRect.size.x * 0.5f) - (m_descText.getGlobalBounds().size.x * 0.5f)),
              util::bottom(m_nameText) + m_vertPad });

        m_licenseText = context.media.makeText(25, license, textColor);

        m_licenseText.setPosition(
            { ((screenRect.size.x * 0.5f) - (m_licenseText.getGlobalBounds().size.x * 0.5f)),
              util::bottom(m_descText) + m_vertPad });

        m_extraText = context.media.makeText(25, extra, textColor);

        m_extraText.setPosition(
            { ((screenRect.size.x * 0.5f) - (m_extraText.getGlobalBounds().size.x * 0.5f)),
              util::bottom(m_licenseText) + m_vertPad });
    }

    void Credit::update(const float frameTimeSec)
    {
        const float scrollSpeed = 30.0f;
        m_nameText.move({ 0.0f, -(frameTimeSec * scrollSpeed) });
        m_descText.move({ 0.0f, -(frameTimeSec * scrollSpeed) });
        m_licenseText.move({ 0.0f, -(frameTimeSec * scrollSpeed) });
        m_extraText.move({ 0.0f, -(frameTimeSec * scrollSpeed) });
    }

    void Credit::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        target.draw(m_nameText, states);
        target.draw(m_descText, states);
        target.draw(m_licenseText, states);
        target.draw(m_extraText, states);
    }

    void Credit::vertPosition(const float pos)
    {
        m_nameText.setPosition({ m_nameText.getGlobalBounds().position.x, pos });

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
        : StateBase(State::Credits, State::Quit)
        , m_credits()
    {}

    void StateCredits::onEnter(Context & context)
    {
        context.music.start("credits.ogg", 20.0f);

        const sf::FloatRect screenRect = context.layout.wholeRegion();

        Credit & softwareCredit = m_credits.emplace_back(context, "Ziesche Til Newman", "Software");

        Credit & fontCredit = m_credits.emplace_back(
            context,
            "Mops Antiqua",
            "Font",
            "SIL Open Font License 1.1",
            "www.scripts.sil.org/ofl");

        const float vertSpacer = (screenRect.size.y * 0.125f);
        fontCredit.vertPosition(softwareCredit.bottom() + vertSpacer);

        Credit & sfmlCredit = m_credits.emplace_back(
            context,
            "SFML",
            "Simple Fast Multimedia Library",
            "Special thanks to Laurent Gomila for this excellent library!",
            "www.sfml-dev.org");

        sfmlCredit.vertPosition(fontCredit.bottom() + vertSpacer);

        Credit & tiledCredit = m_credits.emplace_back(
            context,
            "Tiled",
            "An amazing 2D level editor that is easy and intuitive.",
            "www.mapeditor.org",
            "");

        tiledCredit.vertPosition(sfmlCredit.bottom() + vertSpacer);

        Credit & tilesetCredit =
            m_credits.emplace_back(context, "Free Graveyard Tileset", "www.gameart2d.com", "", "");

        tilesetCredit.vertPosition(tiledCredit.bottom() + vertSpacer);

        Credit & spritesheetCredit =
            m_credits.emplace_back(context, "Ninja Girl Free Sprites", "www.gameart2d.com", "", "");

        spritesheetCredit.vertPosition(tilesetCredit.bottom() + vertSpacer);
    }

    void StateCredits::onExit(Context & context) { context.music.stop("credits.ogg"); }

    void StateCredits::update(Context & context, const float frameTimeSec)
    {
        for (Credit & credit : m_credits)
        {
            credit.update(frameTimeSec);
        }

        Credit & lastCredit = m_credits.back();
        if (lastCredit.bottom() < 0.0f)
        {
            context.state.setChangePending(State::Quit);
        }
    }

    void StateCredits::draw(
        const Context &, sf::RenderTarget & target, sf::RenderStates & states) const
    {
        for (const Credit & credit : m_credits)
        {
            target.draw(credit, states);
        }
    }

    bool StateCredits::handleEvent(Context & context, const sf::Event & event)
    {
        // any keypress or mouse click will exit
        if (event.is<sf::Event::MouseButtonPressed>() || event.is<sf::Event::KeyPressed>())
        {
            context.state.setChangePending(State::Quit);
        }

        return false;
    }

} // namespace halloween
