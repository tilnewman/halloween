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

    GameLoop::GameLoop(const Settings & settings)
        : m_settings(settings)
        , m_random()
        , m_audio(m_random)
        , m_music()
        , m_bats()
        , m_owlCalls()
        , m_window()
        , m_loader()
        , m_media()
        , m_layout()
        , m_avatarUPtr()
        , m_pauseScreenUPtr()
        , m_stateMachineUPtr()
        , m_level()
        , m_missiles()
        , m_coins()
        , m_darts()
        , m_spikedBalls()
        , m_fireSpouts()
        , m_saws()
        , m_slimes()
        , m_ghosts()
        , m_infoRegionUPtr()
        , m_managers()
        , m_stats()
        , m_bossUPtr()
        , m_delayLoopCounts()
        , m_framesPerSecond()
        , m_perSecondClock()
        , m_graphDisplayUPtr()
        , m_contextUPtr()
    {}

    void GameLoop::play()
    {
        setup();
        frameLoop();
        std::cout << "final score " << m_infoRegionUPtr->score() << '\n';
        teardown();
    }

    void GameLoop::setup()
    {
        M_CHECK(
            std::filesystem::exists(m_settings.media_path),
            "The media folder could not be found at \"" << m_settings.media_path << "\"");

        const auto videoMode = util::findVideoModeClosestTo(sf::VideoMode(
            { m_settings.target_screen_res.x, m_settings.target_screen_res.y },
            sf::VideoMode::getDesktopMode().bitsPerPixel));

        m_window.create(videoMode, "Halloween", sf::State::Fullscreen);
        M_CHECK(m_window.isOpen(), "Could not open graphics window.");
        std::cout << "resolution is " << m_window.getSize() << std::endl;

        m_window.setMouseCursorVisible(false);

        util::SfmlDefaults::instance().setup();

        m_audio.mediaPath(m_settings.media_path / "sfx");
        m_audio.loadAll();
        m_audio.willLoop("walk", true);

        m_music.setup(m_settings.media_path / "music");

        m_avatarUPtr = std::make_unique<Avatar>();
        m_pauseScreenUPtr = std::make_unique<PauseScreen>();
        m_stateMachineUPtr = std::make_unique<StateMachine>();
        m_infoRegionUPtr = std::make_unique<InfoRegion>();
        m_bossUPtr = std::make_unique<MushroomBoss>();

        m_contextUPtr = std::make_unique<Context>(
            m_settings,
            m_window,
            m_random,
            m_audio,
            m_music,
            m_bats,
            m_owlCalls,
            m_loader,
            m_media,
            m_layout,
            *m_avatarUPtr,
            *m_pauseScreenUPtr,
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

        m_managers.setupAll(m_settings);

        m_avatarUPtr->setup(m_settings);
        m_pauseScreenUPtr->setup(m_window.getSize(), m_media);
        m_infoRegionUPtr->setup(*m_contextUPtr);
    }

    void GameLoop::teardown()
    {
        //m_managers.clearAll();
        m_contextUPtr.reset();
        m_bossUPtr.reset();
        m_infoRegionUPtr.reset();
        m_stateMachineUPtr.reset();
        m_pauseScreenUPtr.reset();
        m_avatarUPtr.reset();
        util::SfmlDefaults::instance().teardown();
    }

    void GameLoop::frameLoop()
    {
        m_perSecondClock.restart();

        sf::Clock frameClock;
        while (m_window.isOpen() && !m_contextUPtr->will_quit)
        {
            frameClock.restart();

            handlePerSecondTasks();
            handleEvents();
            update(1.0f / m_settings.frame_rate);
            m_stateMachineUPtr->changeIfPending(*m_contextUPtr);
            draw();

            const float frameTimeSec = frameClock.getElapsedTime().asSeconds();
            m_framesPerSecond.push_back(static_cast<std::size_t>(std::round(1.0f / frameTimeSec)));
            handleSleepUntilEndOfFrame(frameTimeSec);
        }
    }

    void GameLoop::handleSleepUntilEndOfFrame(const float elapsedTimeSec)
    {
        float timeRemainingSec = ((1.0f / m_settings.frame_rate) - elapsedTimeSec);

        sf::Clock delayClock;
        std::size_t loopCounter = 0;
        while (timeRemainingSec > 0.0f)
        {
            ++loopCounter;

            delayClock.restart();
            sf::sleep(sf::microseconds(100));
            timeRemainingSec -= delayClock.getElapsedTime().asSeconds();
        }

        m_delayLoopCounts.push_back(loopCounter);
    }

    void GameLoop::handlePerSecondTasks()
    {
        const float elapsedTimeSec = m_perSecondClock.getElapsedTime().asSeconds();
        if (elapsedTimeSec < 1.0f)
        {
            return;
        }

        if (m_settings.will_display_fps)
        {
            std::string str;
            str = " FPS: ";
            str += util::makeStats(m_framesPerSecond).toString();
            str += "     Spin: ";
            str += util::makeStats(m_delayLoopCounts).toString();
            m_media.fps_text.setString(str);

            util::setOriginToPosition(m_media.fps_text);
            m_media.fps_text.setFillColor(sf::Color(195, 160, 126));

            util::fitAndCenterInside(
                m_media.fps_text,
                util::scaleRectInPlaceCopy(m_layout.infoRegion(), { 1.0f, 0.375f }));

            m_media.fps_text.setPosition({ 0.0f, (m_layout.wholeSize().y - 50.0f) });
        }

        if (m_settings.will_display_fps_graph)
        {
            std::sort(std::begin(m_framesPerSecond), std::end(m_framesPerSecond));

            m_graphDisplayUPtr = std::make_unique<util::GraphDisplay<std::size_t>>(
                m_framesPerSecond, sf::Vector2u{ 500, 200 });
        }

        m_perSecondClock.restart();
        m_delayLoopCounts.clear();
        m_framesPerSecond.clear();
    }

    void GameLoop::handleEvents()
    {
        while (const auto eventOpt = m_window.pollEvent())
        {
            m_stateMachineUPtr->state().handleEvent(*m_contextUPtr, eventOpt.value());
        }
    }

    void GameLoop::update(const float frameTimeSec)
    {
        m_stateMachineUPtr->state().update(*m_contextUPtr, frameTimeSec);
    }

    void GameLoop::draw()
    {
        m_window.clear();

        sf::RenderStates states;
        m_stateMachineUPtr->state().draw(*m_contextUPtr, m_window, states);

        if (m_settings.will_display_fps)
        {
            m_window.draw(m_media.fps_text);
        }

        if (m_settings.will_display_fps_graph && m_graphDisplayUPtr)
        {
            m_window.draw(*m_graphDisplayUPtr, states);
        }

        m_window.display();
    }

} // namespace halloween
