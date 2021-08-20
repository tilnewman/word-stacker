#ifndef WORDSTACKER_ARGSPARSER_HPP_INCLUDED
#define WORDSTACKER_ARGSPARSER_HPP_INCLUDED
//
// args-parser.hpp
//
#include "parse-type-enum.hpp"
#include "report-maker.hpp"

#include <SFML/Window/VideoMode.hpp>

#include <cstddef> //for std::size_t
#include <string>
#include <vector>

namespace word_stacker
{

    using StrVec_t = std::vector<std::string>;

    // Responsible for parsing command line arguments into a queryable interface.
    class ArgsParser
    {
      public:
        ArgsParser(
            const sf::VideoMode & VIDEO_MODE,
            ReportMaker & reportMaker,
            const std::size_t ARGC,
            const char * const ARGV[]);

        void ParseCommandLineArguments(
            ReportMaker & reportMaker, const std::size_t ARGC, const char * const ARGV[]);

        void EnsureValidStartingValues(ReportMaker &);

        inline ParseType::Enum parseAs() const { return m_parseType; }

        inline float screenWidthF() const { return m_screenWidth; }
        inline float screenHeightF() const { return m_screenHeight; }

        inline unsigned screenWidthU() const { return static_cast<unsigned>(m_screenWidth); }
        inline unsigned screenHeightU() const { return static_cast<unsigned>(m_screenHeight); }

        inline unsigned bitsPerPixel() const { return m_bitsPerPixel; }

        inline bool isPathADirectory() const { return m_isPathADireectory; }

        inline unsigned fontSizeMin() const { return m_fontSizeMin; }

        inline unsigned fontSizeMax() const { return m_fontSizeMax; }

        inline bool willSkipDisplay() const { return m_willSkipDisplay; }

        inline bool willVerbose() const { return m_willVerbose; }

        inline bool willParseHTML() const { return m_willParseHTML; }

        inline const StrVec_t & parsePaths() const { return m_paths; }

        inline const StrVec_t & ignoredWordsPaths() const { return m_ignoreWordsPaths; }

        inline const StrVec_t & flaggedWordsPaths() const { return m_flaggedWordsPaths; }

        inline bool willIgnoreCommonWords() const { return m_willIgnoreCommonWords; }

        inline const std::string fontPath() const { return m_fontPath; }

        inline const std::string commonWordsPath() const { return m_commonWordsPath; }

      private:
        bool parseCommandLineArgFlag(
            const std::string & ARG,
            bool & memberVar,
            const std::string & CMD_FULL,
            const std::string & CMD_FLAG) const;

        const std::string parseCommandLineArgFile(
            const std::size_t ARGC,
            const char * const ARGV[],
            std::size_t & i,
            const std::string & CMD_FULL,
            const std::string & CMD_FLAG) const;

        const std::string attemptToFindFont() const;

        const std::string findLocalFont() const;

      private:
        static const std::string M_ARG_PARSE_AS;
        static const std::string M_ARG_PARSE_AS_SHORT;
        static const std::string M_ARG_FONT_FILE;
        static const std::string M_ARG_FONT_FILE_SHORT;
        static const std::string M_ARG_COMMON_WORD_FILE;
        static const std::string M_ARG_COMMON_WORD_FILE_SHORT;
        static const std::string M_ARG_IGNORE_COMMON;
        static const std::string M_ARG_IGNORE_COMMON_SHORT;
        static const std::string M_ARG_IGNORE_FILE;
        static const std::string M_ARG_IGNORE_FILE_SHORT;
        static const std::string M_ARG_FONT_SIZE_MAX;
        static const std::string M_ARG_FONT_SIZE_MAX_SHORT;
        static const std::string M_ARG_FONT_SIZE_MIN;
        static const std::string M_ARG_FONT_SIZE_MIN_SHORT;
        static const std::string M_ARG_HELP;
        static const std::string M_ARG_HELP_SHORT;
        static const std::string M_ARG_SKIP_DISPLAY;
        static const std::string M_ARG_SKIP_DISPLAY_SHORT;
        static const std::string M_ARG_VERBOSE;
        static const std::string M_ARG_VERBOSE_SHORT;
        static const std::string M_ARG_FLAGGED_WORDS_FILE;
        static const std::string M_ARG_FLAGGED_WORDS_FILE_SHORT;
        static const std::string M_ARG_PARSE_HTML;
        static const std::string M_ARG_PARSE_HTML_SHORT;
        static const int M_FONT_SIZE_MAX;

        ParseType::Enum m_parseType;
        float m_screenWidth;
        float m_screenHeight;
        unsigned m_bitsPerPixel;
        StrVec_t m_paths;
        std::string m_fontPath;
        std::string m_commonWordsPath;
        bool m_willIgnoreCommonWords;
        StrVec_t m_ignoreWordsPaths;
        bool m_isPathADireectory;
        unsigned m_fontSizeMin;
        unsigned m_fontSizeMax;
        bool m_willSkipDisplay;
        bool m_willVerbose;
        StrVec_t m_flaggedWordsPaths;
        bool m_willParseHTML;
    };

} // namespace word_stacker

#endif // WORDSTACKER_ARGSPARSER_HPP_INCLUDED
