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
#ifndef WORDSTACKER_WORDCOUNTSTATS_HPP_INCLUDED
#define WORDSTACKER_WORDCOUNTSTATS_HPP_INCLUDED
//
// word-count-stats.hpp
//
#include <string>
#include <vector>
#include <cstddef> //for std::size_t


namespace word_stacker
{

    class WordCount;
    using WordCountVec_t = std::vector<WordCount>;


    using StrVec_t = std::vector<std::string>;


    //Responsible for wrapping all frequency and statistical information about a WordCountVec_t.
    struct FreqStats
    {
        explicit FreqStats(
            const std::size_t UNIQUE  = 0,
            const std::size_t SUM     = 0,
            const std::size_t MIN     = 0,
            const std::size_t MAX     = 0,
            const std::size_t AVERAGE = 0,
            const std::size_t MEDIAN  = 0,
            const std::size_t STD_DEV = 0,
            const StrVec_t &  FREQS   = StrVec_t())
        :
            unique(UNIQUE),
            sum(SUM),
            min(MIN),
            max(MAX),
            average(AVERAGE),
            median(MEDIAN),
            stddev(STD_DEV),
            freqs(FREQS)
        {}

        std::size_t unique;
        std::size_t sum;
        std::size_t min;
        std::size_t max;
        std::size_t average;
        std::size_t median;
        std::size_t stddev;
        StrVec_t freqs;
    };


    //Responsible for wrapping all frequency and statistical functions.
    struct Statistics
    {
        static const FreqStats calculate(
            WordCountVec_t &    wordCounts,
            const std::string & FREQ_LIST_PREFIX,
            const std::size_t   FREQ_LIST_LENGTH);
    };

}

#endif //WORDSTACKER_WORDCOUNTSTATS_HPP_INCLUDED
