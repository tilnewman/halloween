#ifndef TEXT_LAYOUT_HPP_INCLUDED
#define TEXT_LAYOUT_HPP_INCLUDED
//
// text-layout.hpp
//
#include "fonts.hpp"

#include <string>
#include <vector>

#include <SFML/Graphics/Text.hpp>

namespace halloween
{

    struct Context;

    //

    struct TextLayoutPack
    {
        sf::FloatRect rect_orig{}; // the original rect you provide typeset()
        sf::FloatRect rect_padded{}; // the original rect with inner pad applied
        sf::FloatRect rect_actual{}; // the actual rect the text ended up in
        std::vector<sf::Text> texts{};
    };

    //

    class TextLayout
    {
      public:
        [[nodiscard]] static const TextLayoutPack typeset(
            const Context & t_context,
            const std::string & t_text,
            const sf::FloatRect & t_rect,
            const TextDetails & t_details);

      private:
        [[nodiscard]] static const std::vector<std::string>
            splitIntoWords(const std::string & t_text);

        static void centerText(
            TextLayoutPack & layout, const bool t_willCenterHoriz, const bool t_willCenterVert);
    };

} // namespace bramblefore

#endif // TEXT_LAYOUT_HPP_INCLUDED
