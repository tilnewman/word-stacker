#ifndef WORDSTACKER_FILEPARSER_HPP_INCLUDED
#define WORDSTACKER_FILEPARSER_HPP_INCLUDED
//
// file-parser.hpp
//
#include "args-parser.hpp"
#include "parse-type-enum.hpp"
#include "report-maker.hpp"
#include "word-count.hpp"
#include "word-list.hpp"

#include <cstddef> //for std::size_t
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace word_stacker
{

    struct FreqStats;

    using WordCountMap_t = std::map<std::string, std::size_t>;
    using LengthCountMap_t = std::map<std::size_t, std::size_t>;

    // Responsible for wrapping objects commonly required by the FileParser.
    struct ParseSupplies
    {
        ParseSupplies(
            ReportMaker & reportMaker,
            const ArgsParser & ARGS,
            const WordList & COMMON_WORDS,
            const WordList & IGNORED_WORDS,
            const WordList & FLAGGED_WORDS)
            : m_reportMaker(reportMaker)
            , M_ARGS(ARGS)
            , M_COMMON_WORDS(COMMON_WORDS)
            , M_IGNORED_WORDS(IGNORED_WORDS)
            , M_FLAGGED_WORDS(FLAGGED_WORDS)
            , m_wordCountMap()
        {}

        ReportMaker & m_reportMaker;
        const ArgsParser & M_ARGS;
        const WordList & M_COMMON_WORDS;
        const WordList & M_IGNORED_WORDS;
        const WordList & M_FLAGGED_WORDS;
        WordCountMap_t m_wordCountMap;
    };

    // responsible for parsing files into a sorted vector of word/count pairs.
    class FileParser
    {
      public:
        explicit FileParser(
            ReportMaker & reportMaker,
            const ArgsParser & ARGS,
            const WordList & COMMON_WORDS,
            const WordList & IGNORED_WORDS,
            const WordList & FLAGGED_WORDS);

        inline const WordCountVec_t & words() const { return m_wordCounts; }

        const WordCount & wordCountObj(const std::size_t INDEX) const;

        inline std::size_t wordCount(const std::size_t INDEX) const
        {
            return wordCountObj(INDEX).count();
        }

        inline const LengthCountMap_t & lengthCountMap() const { return m_lengthCountMap; }

      private:
        void parseDirectoryOrFile(ParseSupplies & supplies, const std::string & PATH_STR);
        void openFileAndParse(ParseSupplies & supplies, const std::string & FILE_PATH);
        void parseFileContentsText(ParseSupplies & supplies, std::ifstream & file);
        void parseFileContentsCode(ParseSupplies & supplies, std::ifstream & file);
        void parseWord(ParseSupplies & supplies, const std::string & WORD);
        void logStatistics(ReportMaker &, const FreqStats &);
        bool doesFilenameMatchParseType(ParseSupplies & supplies, const std::string &) const;

        void changeInvalidCharactersToSpaces(
            std::string & s, const std::string & CHARS_TO_KEEP) const;

      private:
        WordCountVec_t m_wordCounts;
        std::size_t m_lineCount;
        std::size_t m_unCLineCount;
        std::size_t m_singleCount;
        std::size_t m_ignoredCount;
        std::size_t m_fileCount;
        std::size_t m_dirCount;
        std::size_t m_flaggedCount;
        LengthCountMap_t m_lengthCountMap;
    };
} // namespace word_stacker

#endif // WORDSTACKER_FILEPARSER_HPP_INCLUDED
