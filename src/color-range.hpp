#ifndef WORDSTACKER_COLORRANGE_HPP_INCLUDED
#define WORDSTACKER_COLORRANGE_HPP_INCLUDED
//
// color-range.hpp
//
#include <SFML/Graphics.hpp>

#include <cstddef> //for std::size_t
#include <vector>

namespace word_stacker
{

    using ColorVec_t = std::vector<sf::Color>;

    class ColorRange;
    using ColorRangeVec_t = std::vector<ColorRange>;

    // Responsible for storing a collection of colors that make a gradient.
    class ColorRange
    {
      public:
        ColorRange();

        ColorRange(
            const sf::Color & FIRST_COLOR, const sf::Color & LAST_COLOR, const std::size_t LENGTH);

        explicit ColorRange(const ColorVec_t &);

        explicit ColorRange(const ColorRangeVec_t &);

        static const ColorVec_t makeGradient(
            const sf::Color & FIRST_COLOR, const sf::Color & LAST_COLOR, const std::size_t LENGTH);

        static sf::Uint8 clampToUint8(const float);

        inline const ColorVec_t & colors() const { return m_colors; }
        inline std::size_t length() const { return m_colors.size(); }

        const sf::Color colorAtRatio(const float RATIO_PARAM) const;

        void Draw(sf::RenderTarget & target, sf::RenderStates states, sf::FloatRect rect) const;

      private:
        ColorVec_t m_colors;
    };

} // namespace word_stacker

#endif // WORDSTACKER_COLORRANGE_HPP_INCLUDED
