#ifndef NINJA_TRADER_HPP_INCLUDED
#define NINJA_TRADER_HPP_INCLUDED
//
// ninja-trader.hpp
//
#include "fonts.hpp"
#include "states.hpp"
#include "text-layout.hpp"

#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <vector>

namespace sf
{
    class RenderTarget;
}

namespace halloween
{
    struct Context;

    enum class TraderPhase : unsigned char
    {
        InitailDelay,
        TraderHey,
        TraderHeyDelay,
        PlayerHey,
        PlayerHeyDelay,
        TraderOffer,
        TraderHint
    };

    class TraderState final : public TimedMessageState
    {
      public:
        TraderState();
        ~TraderState() final = default;

        void onEnter(const Context & t_context) final;
        void onExit(const Context & t_context) final;
        void update(const Context & t_context, const float t_frameTimeSec) final;
        bool handleEvent(const Context & t_context, const sf::Event & t_event) final;

        void draw(const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states)
            const final;

      private:
        void updateDialogRect();

      public:
        static inline sf::FloatRect m_traderRect{};

      private:
        sf::Texture m_backgroundTexture;
        sf::Sprite m_backgroundSprite;
        sf::Texture m_diaglogTexture;
        sf::Sprite m_dialogSprite;
        float m_elapsedSec;
        sf::FloatRect m_dialogRect;
        TextDetails m_dialogTextDetails;
        TextLayoutPack m_dialogTextPack;
        int m_dartsToGive;
        int m_coinsToTake;
        std::string m_offerMessage;
        sf::Vector2f m_traderDialogPos;
        sf::Vector2f m_playerDialogPos;
        TraderPhase m_phase;
    };

} // namespace halloween

#endif // NINJA_TRADER_HPP_INCLUDED
