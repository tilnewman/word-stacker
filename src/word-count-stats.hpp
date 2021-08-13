#ifndef WORDSTACKER_WORDCOUNTSTATS_HPP_INCLUDED
#define WORDSTACKER_WORDCOUNTSTATS_HPP_INCLUDED
//
// word-count-stats.hpp
//
#include <cstddef> //for std::size_t
#include <string>
#include <vector>

namespace word_stacker
{

    class WordCount;
    using WordCountVec_t = std::vector<WordCount>;

    using StrVec_t = std::vector<std::string>;

    // Responsible for wrapping all frequency and statistical information about a WordCountVec_t.
    struct FreqStats
    {
        explicit FreqStats(
            const std::size_t UNIQUE = 0,
            const std::size_t SUM = 0,
            const std::size_t MIN = 0,
            const std::size_t MAX = 0,
            const std::size_t AVERAGE = 0,
            const std::size_t MEDIAN = 0,
            const std::size_t STD_DEV = 0,
            const StrVec_t & FREQS = StrVec_t())
            : unique(UNIQUE)
            , sum(SUM)
            , min(MIN)
            , max(MAX)
            , average(AVERAGE)
            , median(MEDIAN)
            , stddev(STD_DEV)
            , freqs(FREQS)
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

    // Responsible for wrapping all frequency and statistical functions.
    struct Statistics
    {
        static const FreqStats calculate(
            WordCountVec_t & wordCounts,
            const std::string & FREQ_LIST_PREFIX,
            const std::size_t FREQ_LIST_LENGTH);
    };

} // namespace word_stacker

#endif // WORDSTACKER_WORDCOUNTSTATS_HPP_INCLUDED
