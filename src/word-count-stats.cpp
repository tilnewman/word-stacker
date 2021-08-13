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
// word-count-stats.cpp
//
#include "word-count-stats.hpp"
#include "word-count.hpp"
#include "assert-or-throw.hpp"

#include <cmath>
#include <algorithm>


namespace word_stacker
{

    const FreqStats Statistics::calculate(
        WordCountVec_t &    wordCounts,
        const std::string & FREQ_LIST_PREFIX,
        const std::size_t   FREQ_LIST_LENGTH)
    {
        if (wordCounts.empty())
        {
            return FreqStats();
        }
        
        std::sort(
            wordCounts.begin(),
            wordCounts.end(),
            [](const auto & A, const auto & B)
        {
            return (A.count() > B.count());
        });
        
        auto min{ wordCounts[0].count() };
        auto max{ min };

        std::size_t sum{ 0 };
        for (auto const & WORD_COUNT : wordCounts)
        {
            auto const COUNT{ WORD_COUNT.count() };

            if (min > COUNT)
            {
                min = COUNT;
            }

            if (max < COUNT)
            {
                max = COUNT;
            }

            sum += COUNT;
        }

        auto const AVERAGE{ (sum / wordCounts.size()) };

        std::size_t stdDev{ 0 };

        if (wordCounts.size() > 1)
        {
            double deviationSum{ 0.0 };
            auto const DEV_SUM_ITERATIONS{ wordCounts.size() - 1 };
            for (std::size_t i(1); i < DEV_SUM_ITERATIONS; ++i)
            {
                deviationSum += std::pow(static_cast<double>(
                    static_cast<long long>(wordCounts[i].count()) -
                        static_cast<long long>(AVERAGE)), 2);
            }

            stdDev = static_cast<std::size_t>(
                std::sqrt(deviationSum / static_cast<double>(DEV_SUM_ITERATIONS)) );
        }

        auto const MEDIAN{ wordCounts[wordCounts.size() / 2].count() };

        auto const COUNT{ std::min(wordCounts.size(), FREQ_LIST_LENGTH) };

        StrVec_t freqs;
        for (std::size_t i(0); i < COUNT; ++i)
        {
            std::ostringstream ss;
            ss << FREQ_LIST_PREFIX << wordCounts[i].count() << " " << wordCounts[i].word();
            freqs.push_back(ss.str());
        }

        return FreqStats(wordCounts.size(), sum, min, max, AVERAGE, MEDIAN, stdDev, freqs);
    }

}
