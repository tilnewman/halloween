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
        , m_randomUPtr{}
        , m_audioUPtr{}
        , m_musicUPtr{}
        , m_batsUPtr{}
        , m_owlCallsUPtr{}
        , m_windowUPtr{}
        , m_fontManagerUPtr{}
        , m_layoutUPtr{}
        , m_avatarUPtr{}
        , m_stateMachineUPtr{}
        , m_levelUPtr{}
        , m_missilesUPtr{}
        , m_coinsUPtr{}
        , m_dartsUPtr{}
        , m_spikedBallsUPtr{}
        , m_fireSpoutsUPtr{}
        , m_sawsUPtr{}
        , m_slimesUPtr{}
        , m_ghostsUPtr{}
        , m_infoRegionUPtr{}
        , m_managersUPtr{}
        , m_statsUPtr{}
        , m_bossUPtr{}
        , m_framerateDisplayUPtr{}
        , m_smokeUPtr{}
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

        //
        m_windowUPtr = std::make_unique<sf::RenderWindow>();

        const auto videoMode{ util::findVideoModeClosestTo(
            sf::VideoMode(
                { m_settings.target_screen_res.x, m_settings.target_screen_res.y },
                sf::VideoMode::getDesktopMode().bitsPerPixel)) };

        m_windowUPtr->create(videoMode, "Halloween", sf::State::Fullscreen);
        M_CHECK(m_windowUPtr->isOpen(), "Could not open graphics window.");

        m_windowUPtr->setMouseCursorVisible(false);

        //
        util::SfmlDefaults::instance().setup();

        m_fontManagerUPtr = std::make_unique<FontManager>();
        m_avatarUPtr = std::make_unique<Avatar>();
        m_stateMachineUPtr = std::make_unique<StateMachine>();
        m_infoRegionUPtr = std::make_unique<InfoRegion>();
        m_bossUPtr = std::make_unique<MushroomBoss>();
        m_framerateDisplayUPtr = std::make_unique<FramerateDisplay>();
        m_randomUPtr = std::make_unique<util::Random>();
        m_audioUPtr = std::make_unique<util::SoundPlayer>(*m_randomUPtr);
        m_musicUPtr = std::make_unique<util::MusicPlayer>();
        m_batsUPtr = std::make_unique<Bats>();
        m_owlCallsUPtr = std::make_unique<OwlCalls>();
        m_layoutUPtr = std::make_unique<ScreenRegions>();
        m_levelUPtr = std::make_unique<Level>();
        m_missilesUPtr = std::make_unique<Missiles>();
        m_coinsUPtr = std::make_unique<Coins>();
        m_dartsUPtr = std::make_unique<Darts>();
        m_spikedBallsUPtr = std::make_unique<SpikedBalls>();
        m_fireSpoutsUPtr = std::make_unique<FireSpouts>();
        m_sawsUPtr = std::make_unique<Saws>();
        m_slimesUPtr = std::make_unique<Slimes>();
        m_ghostsUPtr = std::make_unique<Ghosts>();
        m_managersUPtr = std::make_unique<ObjectManagerList>();
        m_statsUPtr = std::make_unique<LevelStats>();
        m_smokeUPtr = std::make_unique<Smoke>();

        m_audioUPtr->mediaPath(m_settings.media_path / "sfx");
        m_audioUPtr->loadAll();
        m_audioUPtr->willLoop("walk", true);

        m_musicUPtr->setup(m_settings.media_path / "music");

        m_contextUPtr = std::make_unique<Context>(
            m_settings,
            *m_windowUPtr,
            *m_randomUPtr,
            *m_audioUPtr,
            *m_musicUPtr,
            *m_batsUPtr,
            *m_owlCallsUPtr,
            *m_fontManagerUPtr,
            *m_layoutUPtr,
            *m_avatarUPtr,
            *m_stateMachineUPtr,
            *m_levelUPtr,
            *m_missilesUPtr,
            *m_coinsUPtr,
            *m_dartsUPtr,
            *m_spikedBallsUPtr,
            *m_fireSpoutsUPtr,
            *m_sawsUPtr,
            *m_slimesUPtr,
            *m_ghostsUPtr,
            *m_infoRegionUPtr,
            *m_managersUPtr,
            *m_statsUPtr,
            *m_bossUPtr,
            *m_smokeUPtr);

        m_layoutUPtr->setup(m_windowUPtr->getSize());
        m_fontManagerUPtr->setup(m_settings);
        m_missilesUPtr->setup(m_settings);

        m_managersUPtr->add(*m_coinsUPtr);
        m_managersUPtr->add(*m_spikedBallsUPtr);
        m_managersUPtr->add(*m_fireSpoutsUPtr);
        m_managersUPtr->add(*m_sawsUPtr);
        m_managersUPtr->add(*m_dartsUPtr);
        m_managersUPtr->add(*m_ghostsUPtr);
        m_managersUPtr->add(*m_slimesUPtr);
        m_managersUPtr->add(*m_batsUPtr);
        m_managersUPtr->add(*m_bossUPtr);
        m_managersUPtr->add(*m_smokeUPtr);
        m_managersUPtr->setupAll(*m_contextUPtr);

        m_avatarUPtr->setup(m_settings);
        m_infoRegionUPtr->setup(*m_contextUPtr);
        m_levelUPtr->setup(*m_contextUPtr);

        m_stateMachineUPtr->setChangePending(State::Credits);
    }

    void GameLoop::teardown()
    {
        m_contextUPtr.reset();

        m_managersUPtr.reset();

        m_smokeUPtr.reset();
        m_statsUPtr.reset();
        m_ghostsUPtr.reset();
        m_slimesUPtr.reset();
        m_sawsUPtr.reset();
        m_fireSpoutsUPtr.reset();
        m_spikedBallsUPtr.reset();
        m_dartsUPtr.reset();
        m_coinsUPtr.reset();
        m_missilesUPtr.reset();
        m_levelUPtr.reset();
        m_batsUPtr.reset();
        m_owlCallsUPtr.reset();
        m_bossUPtr.reset();
        m_infoRegionUPtr.reset();
        m_stateMachineUPtr.reset();
        m_avatarUPtr.reset();
        m_fontManagerUPtr.reset();
        m_layoutUPtr.reset();
        m_audioUPtr.reset();
        m_musicUPtr.reset();

        m_randomUPtr.reset(); // this must happen ater m_audioUPtr.reset()

        util::SfmlDefaults::instance().teardown();
    }

    void GameLoop::frameLoop()
    {
        sf::Clock frameClock;
        while (m_windowUPtr->isOpen() && (m_stateMachineUPtr->stateEnum() != State::Quit))
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
        while (const auto eventOpt = m_windowUPtr->pollEvent())
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
        m_windowUPtr->clear();
        m_stateMachineUPtr->state().draw(*m_contextUPtr, *m_windowUPtr, m_states);
        m_framerateDisplayUPtr->draw(*m_contextUPtr, *m_windowUPtr, m_states);
        m_windowUPtr->display();
    }

} // namespace halloween
