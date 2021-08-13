// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
//
// color-range.cpp
//
#include "color-range.hpp"
#include "assert-or-throw.hpp"

#include <cassert>
#include <algorithm>


namespace word_stacker
{

    ColorRange::ColorRange()
    :
        m_colors()
    {}


    ColorRange::ColorRange(
        const sf::Color & FIRST_COLOR,
        const sf::Color & LAST_COLOR,
        const std::size_t LENGTH)
    :
        m_colors( makeGradient(FIRST_COLOR, LAST_COLOR, LENGTH) )
    {}


    ColorRange::ColorRange(const ColorVec_t & COLOR_VEC)
    :
        m_colors(COLOR_VEC)
    {}


    ColorRange::ColorRange(const ColorRangeVec_t & COLOR_RANGE_VEC)
    :
        m_colors()
    {
        for (auto const & COLOR_RANGE : COLOR_RANGE_VEC)
        {
            auto const & SUB_COLOR_RANGE{ COLOR_RANGE.colors() };

            std::copy(
                SUB_COLOR_RANGE.begin(),
                SUB_COLOR_RANGE.end(),
                std::back_inserter(m_colors));
        }
    }


    const ColorVec_t ColorRange::makeGradient(
        const sf::Color & FIRST_COLOR,
        const sf::Color & LAST_COLOR,
        const std::size_t LENGTH)
    {
        ColorVec_t colors;

        auto red{ static_cast<float>(FIRST_COLOR.r) };
        auto green{ static_cast<float>(FIRST_COLOR.g) };
        auto blue{ static_cast<float>(FIRST_COLOR.b) };
        auto alpha{ static_cast<float>(FIRST_COLOR.a) };

        auto redStep{ (static_cast<float>(LAST_COLOR.r) - red) /
            static_cast<float>(LENGTH) };

        auto greenStep{ (static_cast<float>(LAST_COLOR.g) - green) /
            static_cast<float>(LENGTH) };

        auto blueStep{ (static_cast<float>(LAST_COLOR.b) - blue) /
            static_cast<float>(LENGTH) };

        auto alphaStep{ (static_cast<float>(LAST_COLOR.a) - alpha) /
            static_cast<float>(LENGTH) };

        for (std::size_t i(0); i < LENGTH; ++i)
        {
            auto const FLOAT_I{ static_cast<float>(i) };

            auto const NEXT_RED{ clampToUint8(red + (redStep * FLOAT_I)) };
            auto const NEXT_GREEN{ clampToUint8(green + (greenStep * FLOAT_I)) };
            auto const NEXT_BLUE{ clampToUint8(blue + (blueStep * FLOAT_I)) };
            auto const NEXT_ALPHA{ clampToUint8(alpha + (alphaStep * FLOAT_I)) };

            colors.push_back( sf::Color(NEXT_RED, NEXT_GREEN, NEXT_BLUE, NEXT_ALPHA) );
        }

        return colors;
    }


    sf::Uint8 ColorRange::clampToUint8(const float F)
    {
        if (F < 0.0f)
        {
            return 0;
        }
        else if (F > 255.0f)
        {
            return 255;
        }
        else
        {
            return static_cast<sf::Uint8>(F);
        }
    }


    void ColorRange::Draw(
        sf::RenderTarget & target,
        sf::RenderStates   states,
        sf::FloatRect      rect) const
    {
        M_LOG_AND_ASSERT_OR_THROW((rect.width > 1.5f),
            "word-stacker::ColorRange::Draw(rect.width="
            << rect.width << ") rect.width was less than 2.0f.");

        std::vector<sf::Vertex> verts;
        verts.reserve( static_cast<std::size_t>(rect.width) );

        for (int i(0); i < static_cast<int>(rect.width); ++i)
        {
            auto const FLOAT_I{ static_cast<float>(i) };

            auto const COLOR{ colorAtRatio(FLOAT_I / rect.width) };

            verts.push_back( sf::Vertex(
                sf::Vector2f(rect.left + FLOAT_I, rect.top), COLOR) );

            verts.push_back( sf::Vertex(
                sf::Vector2f(rect.left + FLOAT_I, rect.top + rect.height), COLOR) );
        }

        if (verts.empty() == false)
        {
            target.draw( & verts[0], verts.size(), sf::Lines, states);
        }
    }


    const sf::Color ColorRange::colorAtRatio(const float RATIO_PARAM) const
    {
        M_LOG_AND_ASSERT_OR_THROW((m_colors.empty() == false),
            "word-stacker::ColorRange::colorAtRatio(ratio="
            << RATIO_PARAM << ") m_colors vector was empty.");

        auto const MAX_INDEX{ static_cast<int>(m_colors.size() - 1) };

        auto index{ static_cast<int>(
            RATIO_PARAM * static_cast<float>(MAX_INDEX)) };

        if (index < 0)
        {
            index = 0;
        }
        else if (index > MAX_INDEX)
        {
            index = MAX_INDEX;
        }
        
        return m_colors.at(static_cast<std::size_t>(index));
    }

}
