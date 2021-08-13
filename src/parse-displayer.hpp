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
#ifndef WORDSTACKER_PARSEDISPLAYER_HPP_INCLUDED
#define WORDSTACKER_PARSEDISPLAYER_HPP_INCLUDED
//
// parse-displayer.hpp
//
#include "args-parser.hpp"
#include "file-parser.hpp"
#include "color-range.hpp"
#include "word-list.hpp"
#include "report-maker.hpp"

#include <SFML/Graphics.hpp>

#include <string>
#include <vector>
#include <sstream>
#include <cstddef> //for std::size_t


namespace word_stacker
{

    using TextVec_t = std::vector<sf::Text>;
    using RectVec_t = std::vector<sf::FloatRect>;


    struct FreqStats;


    //Responsible for displaying a parsed file graphically as stacks of words.
    class ParseDisplayer
    {
    public:
        explicit ParseDisplayer(const ArgsParser & ARGS);

        void draw(
            const ArgsParser & ARGS,
            const FileParser & PARSED_FILE,
            sf::RenderTarget & target,
            sf::RenderStates states) const;

        void setup(
            ReportMaker &      reportMaker,
            const ArgsParser & ARGS,
            const FileParser & PARSED_FILE,
            const WordList &   COMMON_WORDS,
            const WordList &   FLAGGED_WORDS,
            const bool         WILL_SHOW_COUNTS,
            const bool         WILL_SHOW_COLUMS,
            const bool         WILL_SHOW_LINELEN_GRAPH = false);

    private:
        std::size_t setupAsColumns(
            const ArgsParser & ARGS,
            const WordList &   COMMON_WORDS,
            const WordList &   FLAGGED_WORDS,
            const bool         WILL_SHOW_COUNTS,
            WordCountVec_t &   wordCounts);

        std::size_t setupAsColumnsAtFontSize(
            const ArgsParser & ARGS,
            const WordList &   COMMON_WORDS,
            const WordList &   FLAGGED_WORDS,
            const bool         WILL_SHOW_COUNTS,
            const unsigned     FONT_SIZE_MAX,
            WordCountVec_t &   wordCounts);

        std::size_t setupAsJumble(
            const ArgsParser & ARGS,
            const WordList &   COMMON_WORDS,
            const WordList &   FLAGGED_WORDS,
            const bool         WILL_SHOW_COUNTS,
            WordCountVec_t &   wordCounts);

        std::size_t setupAsJumbleAtFontSize(
            const ArgsParser & ARGS,
            const WordList &   COMMON_WORDS,
            const WordList &   FLAGGED_WORDS,
            const bool         WILL_SHOW_COUNTS,
            const unsigned     FONT_SIZE_MAX,
            WordCountVec_t &   wordCounts);

        bool placeNextJumbledWord(
            const ArgsParser &     ARGS,
            const WordList &       COMMON_WORDS,
            const WordList &       FLAGGED_WORDS,
            const bool             WILL_SHOW_COUNTS,
            RectVec_t &            rects,
            const WordCountVec_t & WORDS,
            const std::size_t      WORDS_INDEX,
            const unsigned         FONT_SIZE_MAX);

        const sf::Vector2f findNextJumblePosition(
            const ArgsParser &   ARGS,
            const sf::Vector2f & SIZE,
            const RectVec_t &    INVALID_RECTS) const;

        //returns zero if the given TEST_RECT is already in a valid position
        float findNextLowestValidPosition(
            const sf::FloatRect & TEST_RECT,
            const RectVec_t &     INVALID_RECTS) const;
        
        void loadFont(const ArgsParser & ARGS);

        void setupColumn(
            const ArgsParser &     ARGS,
            float &                posLeft,
            const WordCountVec_t & WORDS,
            std::size_t &          wordIndex,
            const WordList &       COMMON_WORDS,
            const WordList &       FLAGGED_WORDS,
            const bool             WILL_DISPLAY_COUNTS,
            const unsigned         FONT_SIZE_MAX);

        void logStatistics(ReportMaker &, const FreqStats &);

        void drawLineLengthGraph(
            const ArgsParser & ARGS,
            const FileParser & PARSED_FILE,
            sf::RenderTarget & target,
            sf::RenderStates states) const;

    private:
        static const std::size_t M_GRADIENT_DEPTH;
        ColorRange m_uniqueColors;
        ColorRange m_commonColors;
        sf::Font m_font;
        TextVec_t m_texts;
        bool m_willShowLineLengthGraph;
    };

}

#endif //WORDSTACKER_PARSEDISPLAYER_HPP_INCLUDED
