// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "game-loop.hpp"

#include "resolution-picker.hpp"

#include <sstream>

#include <SFML/Window/VideoMode.hpp>

namespace halloween
{

    GameLoop::GameLoop()
        : m_settings()
        , m_random()
        , m_audio(m_random)
        , m_window()
        , m_loader()
        , m_media()
        , m_layout()
        , m_avatar()
        , m_pauseScreen()
        , m_stateMachine()
        , m_level()
        , m_missiles()
        , m_coins()
        , m_infoRegion()
        , m_delayLoopCounts()
        , m_framesPerSecond()
        , m_perSecondClock()
        , m_context(
              m_settings,
              m_window,
              m_random,
              m_audio,
              m_loader,
              m_media,
              m_layout,
              m_avatar,
              m_pauseScreen,
              m_stateMachine,
              m_level,
              m_missiles,
              m_coins,
              m_infoRegion)
    {}

    void GameLoop::play()
    {
        setup();
        frameLoop();
    }

    void GameLoop::setup()
    {
        m_framesPerSecond.reserve(static_cast<std::size_t>(m_settings.frame_rate));
        m_delayLoopCounts.reserve(static_cast<std::size_t>(m_settings.frame_rate));

        M_CHECK(
            std::filesystem::exists(m_settings.media_path),
            "The media folder could not be found at \"" << m_settings.media_path << "\"");

        m_audio.mediaPath(m_settings.media_path / "audio");
        m_audio.loadAll();
        m_audio.willLoop("walk", true);

        const auto videoMode = ResolutionPicker::pickCloseTo(sf::VideoMode(
            m_settings.target_res.x,
            m_settings.target_res.y,
            sf::VideoMode::getDesktopMode().bitsPerPixel));

        m_window.create(videoMode, "Halloween", sf::Style::Fullscreen);
        M_CHECK(m_window.isOpen(), "Could not open graphics window.");
        M_LOG("resolution is " << m_window.getSize());

        m_media.setup(m_settings);
        m_layout.setup(m_context, m_window.getSize());
        m_missiles.setup(m_settings);
        m_coins.setup(m_settings);
        m_avatar.setup(m_settings);
        m_pauseScreen.setup(m_window.getSize(), m_media);
        m_infoRegion.setup(m_context);

        m_level.load(m_context, 1);
    }

    void GameLoop::frameLoop()
    {
        m_perSecondClock.restart();

        sf::Clock frameClock;
        sf::Clock actualClock;
        while (m_window.isOpen() && !m_context.will_quit)
        {
            frameClock.restart();
            const float actualFrameTimeSec = actualClock.restart().asSeconds();

            handlePerSecondTasks();
            handleEvents();
            update(actualFrameTimeSec);
            draw();
            m_stateMachine.changeIfPending(m_context);

            handleSleepUntilEndOfFrame(frameClock.getElapsedTime().asSeconds());
        }
    }

    void GameLoop::handleSleepUntilEndOfFrame(const float elapsedTimeSec)
    {
        m_framesPerSecond.push_back(static_cast<std::size_t>(std::round(1.0f / elapsedTimeSec)));

        float timeRemainingSec = ((1.0f / m_settings.frame_rate) - elapsedTimeSec);

        std::size_t delayLoopCounter = 0;
        while (timeRemainingSec > 0.0001f)
        {
            ++delayLoopCounter;

            sf::Clock delayClock;
            sf::sleep(sf::microseconds(100));
            timeRemainingSec -= delayClock.getElapsedTime().asSeconds();
        }

        m_delayLoopCounts.push_back(delayLoopCounter);
    }

    void GameLoop::handlePerSecondTasks()
    {
        const float elapsedTimeSec = m_perSecondClock.getElapsedTime().asSeconds();
        if (elapsedTimeSec > 1.0f)
        {
            const auto fpsStats = util::makeStats(m_framesPerSecond);
            const auto spinStats = util::makeStats(m_delayLoopCounts);

            std::ostringstream ss;
            ss << " FPS: " << fpsStats << "      Spins: " << spinStats;
            m_media.fps_text.setString(ss.str());
            util::setOriginToPosition(m_media.fps_text);
            m_media.fps_text.setFillColor(sf::Color(195, 160, 126));

            util::fitAndCenterInside(
                m_media.fps_text,
                util::scaleRectInPlaceCopy(m_layout.infoRegion(), { 1.0f, 0.375f }));

            m_media.fps_text.setPosition(0.0f, (m_layout.wholeSize().y - 50.0f));

            m_perSecondClock.restart();
            m_delayLoopCounts.clear();
            m_framesPerSecond.clear();
        }
    }

    void GameLoop::handleEvents()
    {
        sf::Event event;
        while (m_window.pollEvent(event))
        {
            m_stateMachine.state().handleEvent(m_context, event);
        }
    }

    void GameLoop::update(const float FRAME_TIME_SEC)
    {
        m_stateMachine.state().update(m_context, FRAME_TIME_SEC);
    }

    void GameLoop::draw()
    {
        m_window.clear();

        sf::RenderStates states;
        m_stateMachine.state().draw(m_context, m_window, states);

        // TODO remove after testing
        m_window.draw(m_media.fps_text);

        m_window.display();
    }

} // namespace halloween
