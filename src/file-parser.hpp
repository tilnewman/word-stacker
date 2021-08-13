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
#ifndef WORDSTACKER_FILEPARSER_HPP_INCLUDED
#define WORDSTACKER_FILEPARSER_HPP_INCLUDED
//
// file-parser.hpp
//
#include "parse-type-enum.hpp"
#include "args-parser.hpp"
#include "word-count.hpp"
#include "word-list.hpp"
#include "report-maker.hpp"

#include <map>
#include <string>
#include <vector>
#include <cstddef> //for std::size_t
#include <sstream>


namespace word_stacker
{

    struct FreqStats;


    using WordCountMap_t = std::map<std::string, std::size_t>;
    using LengthCountMap_t = std::map<std::size_t, std::size_t>;


    //Responsible for wrapping objects commonly required by the FileParser.
    struct ParseSupplies
    {
        ParseSupplies(
            ReportMaker &      reportMaker,
            const ArgsParser & ARGS,
            const WordList &   COMMON_WORDS,
            const WordList &   IGNORED_WORDS,
            const WordList &   FLAGGED_WORDS)
        :
            m_reportMaker   (reportMaker),
            M_ARGS          (ARGS),
            M_COMMON_WORDS  (COMMON_WORDS),
            M_IGNORED_WORDS (IGNORED_WORDS),
            M_FLAGGED_WORDS (FLAGGED_WORDS),
            m_wordCountMap  ()
        {}

        ReportMaker      & m_reportMaker;
        const ArgsParser & M_ARGS;
        const WordList   & M_COMMON_WORDS;
        const WordList   & M_IGNORED_WORDS;
        const WordList   & M_FLAGGED_WORDS;
        WordCountMap_t     m_wordCountMap;
    };


    //responsible for parsing files into a sorted vector of word/count pairs.
    class FileParser
    {
    public:
        explicit FileParser(
            ReportMaker &      reportMaker,
            const ArgsParser & ARGS,
            const WordList &   COMMON_WORDS,
            const WordList &   IGNORED_WORDS,
            const WordList &   FLAGGED_WORDS);

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
        void parseWord(ParseSupplies & supplies, const std::string &   WORD);
        void logStatistics(ReportMaker &, const FreqStats &);
        bool doesFilenameMatchParseType(ParseSupplies & supplies, const std::string &) const;

        void changeInvalidCharactersToSpaces(
            std::string &       s,
            const std::string & CHARS_TO_KEEP) const;

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
}

#endif //WORDSTACKER_FILEPARSER_HPP_INCLUDED
