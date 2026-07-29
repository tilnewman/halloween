// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "game-loop.hpp"

#include "check-macros.hpp"
#include "sfml-defaults.hpp"
#include "sfml-util.hpp"

#include <iostream>

#include <SFML/Window/Event.hpp>
#include <SFML/Window/VideoMode.hpp>

namespace halloween
{

    GameLoop::GameLoop(const Settings & t_settings)
        : m_settings{ t_settings }
        , m_random{}
        , m_audio{ m_random }
        , m_music{}
        , m_bats{}
        , m_owlCalls{}
        , m_window{}
        , m_media{}
        , m_layout{}
        , m_avatarUPtr{}
        , m_stateMachineUPtr{}
        , m_level{}
        , m_missiles{}
        , m_coins{}
        , m_darts{}
        , m_spikedBalls{}
        , m_fireSpouts{}
        , m_saws{}
        , m_slimes{}
        , m_ghosts{}
        , m_infoRegionUPtr{}
        , m_managers{}
        , m_stats{}
        , m_bossUPtr{}
        , m_framerateDisplayUPtr{}
        , m_contextUPtr{}
    {}

    void GameLoop::play()
    {
        setup();
        frameLoop();
        teardown();
    }

    void GameLoop::setup()
    {
        M_CHECK(
            std::filesystem::exists(m_settings.media_path),
            "The media folder could not be found at \"" << m_settings.media_path << "\"");

        const auto videoMode{ util::findVideoModeClosestTo(
            sf::VideoMode(
                { m_settings.target_screen_res.x, m_settings.target_screen_res.y },
                sf::VideoMode::getDesktopMode().bitsPerPixel)) };

        m_window.create(videoMode, "Halloween", sf::State::Fullscreen);
        M_CHECK(m_window.isOpen(), "Could not open graphics window.");

        m_window.setMouseCursorVisible(false);

        util::SfmlDefaults::instance().setup();

        m_audio.mediaPath(m_settings.media_path / "sfx");
        m_audio.loadAll();
        m_audio.willLoop("walk", true);

        m_music.setup(m_settings.media_path / "music");

        m_avatarUPtr = std::make_unique<Avatar>();
        m_stateMachineUPtr = std::make_unique<StateMachine>();
        m_infoRegionUPtr = std::make_unique<InfoRegion>();
        m_bossUPtr = std::make_unique<MushroomBoss>();
        m_framerateDisplayUPtr = std::make_unique<FramerateDisplay>();

        m_contextUPtr = std::make_unique<Context>(
            m_settings,
            m_window,
            m_random,
            m_audio,
            m_music,
            m_bats,
            m_owlCalls,
            m_media,
            m_layout,
            *m_avatarUPtr,
            *m_stateMachineUPtr,
            m_level,
            m_missiles,
            m_coins,
            m_darts,
            m_spikedBalls,
            m_fireSpouts,
            m_saws,
            m_slimes,
            m_ghosts,
            *m_infoRegionUPtr,
            m_managers,
            m_stats,
            *m_bossUPtr);

        m_layout.setup(m_window.getSize());
        m_media.setup(m_settings);
        m_missiles.setup(m_settings);

        m_managers.add(m_coins);
        m_managers.add(m_spikedBalls);
        m_managers.add(m_fireSpouts);
        m_managers.add(m_saws);
        m_managers.add(m_darts);
        m_managers.add(m_ghosts);
        m_managers.add(m_slimes);
        m_managers.add(m_bats);
        m_managers.add(*m_bossUPtr);

        m_managers.setupAll(*m_contextUPtr);

        m_avatarUPtr->setup(m_settings);
        m_infoRegionUPtr->setup(*m_contextUPtr);
    }

    void GameLoop::teardown()
    {
        m_contextUPtr.reset();
        m_bossUPtr.reset();
        m_infoRegionUPtr.reset();
        m_stateMachineUPtr.reset();
        m_avatarUPtr.reset();

        util::SfmlDefaults::instance().teardown();
    }

    void GameLoop::frameLoop()
    {
        sf::Clock frameClock;
        while (m_window.isOpen() && (m_stateMachineUPtr->stateEnum() != State::Quit))
        {
            frameClock.restart();

            handleEvents();
            update(1.0f / m_settings.frame_rate);
            m_stateMachineUPtr->changeIfPending(*m_contextUPtr);
            draw();

            const float elapsedFrameTimeSec{ frameClock.getElapsedTime().asSeconds() };
            handleSleepUntilEndOfFrame(elapsedFrameTimeSec);
            m_framerateDisplayUPtr->update(*m_contextUPtr, elapsedFrameTimeSec);
        }
    }

    void GameLoop::handleSleepUntilEndOfFrame(const float t_elapsedTimeSec)
    {
        float timeRemainingSec{ (1.0f / m_settings.frame_rate) - t_elapsedTimeSec };

        sf::Clock delayClock;
        while (timeRemainingSec > 0.0f)
        {
            delayClock.restart();
            sf::sleep(sf::microseconds(100));
            timeRemainingSec -= delayClock.getElapsedTime().asSeconds();
        }
    }

    void GameLoop::handleEvents()
    {
        while (const auto eventOpt = m_window.pollEvent())
        {
            m_stateMachineUPtr->state().handleEvent(*m_contextUPtr, eventOpt.value());
        }
    }

    void GameLoop::update(const float t_frameTimeSec)
    {
        m_stateMachineUPtr->state().update(*m_contextUPtr, t_frameTimeSec);
    }

    void GameLoop::draw()
    {
        m_window.clear();
        m_stateMachineUPtr->state().draw(*m_contextUPtr, m_window, m_states);
        m_framerateDisplayUPtr->draw(*m_contextUPtr, m_window, m_states);
        m_window.display();
    }

} // namespace halloween
