///////////////////////////////////////////////////////////////////////////////
//
// Word-Stacker - Open-source, non-commercial, word frequency analysis tool.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
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


    //Responsible for storing a collection of colors that make a gradient.
    class ColorRange
    {
    public:
        ColorRange();

        ColorRange(
            const sf::Color & FIRST_COLOR,
            const sf::Color & LAST_COLOR,
            const std::size_t LENGTH);

        explicit ColorRange(const ColorVec_t &);

        explicit ColorRange(const ColorRangeVec_t &);

        static const ColorVec_t makeGradient(
            const sf::Color & FIRST_COLOR,
            const sf::Color & LAST_COLOR,
            const std::size_t LENGTH);

        static sf::Uint8 clampToUint8(const float);

        inline const ColorVec_t & colors() const    { return m_colors; }
        inline std::size_t length() const           { return m_colors.size(); }

        const sf::Color colorAtRatio(const float RATIO_PARAM) const;

        void Draw(
            sf::RenderTarget & target,
            sf::RenderStates   states,
            sf::FloatRect      rect) const;

    private:
        ColorVec_t m_colors;
    };

}

#endif //WORDSTACKER_COLORRANGE_HPP_INCLUDED
