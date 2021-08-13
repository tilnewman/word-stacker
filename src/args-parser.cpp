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
// args-parser.cpp
//
#include "args-parser.hpp"
#include "assert-or-throw.hpp"
#include "report-maker.hpp"

#include <SFML/Window/VideoMode.hpp>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/algorithm.hpp>

#include <cstdlib>
#include <iostream>


namespace word_stacker
{

    //all argument strings must be lowercase
    const std::string ArgsParser::M_ARG_PARSE_AS                { "--type=" };
    const std::string ArgsParser::M_ARG_PARSE_AS_SHORT          { "-t" };
    const std::string ArgsParser::M_ARG_FONT_FILE               { "--font=" };
    const std::string ArgsParser::M_ARG_FONT_FILE_SHORT         { "-f" };
    const std::string ArgsParser::M_ARG_COMMON_WORD_FILE        { "--common=" };
    const std::string ArgsParser::M_ARG_COMMON_WORD_FILE_SHORT  { "-c" };
    const std::string ArgsParser::M_ARG_IGNORE_COMMON           { "--will-ignore-common=" };
    const std::string ArgsParser::M_ARG_IGNORE_COMMON_SHORT     { "-w" };
    const std::string ArgsParser::M_ARG_IGNORE_FILE             { "--ignore=" };
    const std::string ArgsParser::M_ARG_IGNORE_FILE_SHORT       { "-i" };
    const std::string ArgsParser::M_ARG_FONT_SIZE_MAX           { "--font-size-max=" };
    const std::string ArgsParser::M_ARG_FONT_SIZE_MAX_SHORT     { "-x" };
    const std::string ArgsParser::M_ARG_FONT_SIZE_MIN           { "--font-size-min=" };
    const std::string ArgsParser::M_ARG_FONT_SIZE_MIN_SHORT     { "-n" };
    const std::string ArgsParser::M_ARG_HELP                    { "--help" };
    const std::string ArgsParser::M_ARG_HELP_SHORT              { "-h" };
    const std::string ArgsParser::M_ARG_SKIP_DISPLAY            { "--skip-display=" };
    const std::string ArgsParser::M_ARG_SKIP_DISPLAY_SHORT      { "-s" };
    const std::string ArgsParser::M_ARG_VERBOSE                 { "--verbose=" };
    const std::string ArgsParser::M_ARG_VERBOSE_SHORT           { "-v" };
    const std::string ArgsParser::M_ARG_FLAGGED_WORDS_FILE      { "--flagged=" };
    const std::string ArgsParser::M_ARG_FLAGGED_WORDS_FILE_SHORT{ "-l"};
    const std::string ArgsParser::M_ARG_PARSE_HTML              { "--parse-html=" };
    const std::string ArgsParser::M_ARG_PARSE_HTML_SHORT        { "-m" };
    const int ArgsParser::M_FONT_SIZE_MAX                       { 999 };


    ArgsParser::ArgsParser(
        ReportMaker &      reportMaker,
        const std::size_t  ARGC,
        const char * const ARGV[])
    :
        m_parseType             (ParseType::Count),
        m_screenWidth           (-1.0f), //any negative will work here and below
        m_screenHeight          (-1.0f),
        m_bitsPerPixel          (0),
        m_paths                 (),
        m_fontPath              (""),
        m_commonWordsPath       (""),
        m_willIgnoreCommonWords (false),
        m_ignoreWordsPaths      (),
        m_isPathADireectory     (""),
        m_fontSizeMin           (30),
        m_fontSizeMax           (400),
        m_willSkipDisplay       (false),
        m_willVerbose           (false),
        m_flaggedWordsPaths     (),
        m_willParseHTML         (false)
    {
        ParseVideoMode(reportMaker);
        ParseCommandLineArguments(reportMaker, ARGC, ARGV);
        EnsureValidStartingValues(reportMaker);
    }


    void ArgsParser::ParseCommandLineArguments(
        ReportMaker &      reportMaker,
        const std::size_t  ARGC,
        const char * const ARGV[])
    {
        for (std::size_t i(1); i < ARGC; i++)
        {
            auto const ARG{ boost::trim_copy(std::string(ARGV[i])) };

            if (ARG.empty())
            {
                continue;
            }

            if (ARG == M_ARG_HELP_SHORT)
            {
                std::cout << "  Usage:  word_stacker [-wsvhm] "
                    << "[-t Text/Code] <-f font> [-c common words file] [-i ignored words file] "
                    << "[-n font size min] [-x font size max] <FILE or DIR to parse>\n"
                    << "Try 'word_stacker --help' for more information."
                    << std::endl;

                exit(EXIT_SUCCESS);
            }

            if (boost::starts_with(ARG, M_ARG_HELP))
            {
                std::cout
                    << "  Parses all .txt or all .hpp/.cpp files and then displays word frequency "
                    << "information in columns or in a jumble.\n"
                    << '\n'
                    << "  Usage:  word_stacker [-wsvhm] "
                    << "[-t Text/Code] <-f font> [-c common words file] [-i ignored words file] "
                    << "[-n font size min] [-x font size max] <FILE or DIR to parse ...>\n"
                    << '\n'
                    << "Options:\n"
                    << "  -w, --will-ignore-common=yes/no      prevents the display of words in the common words file\n"
                    << "  -s, --skip-display=yes/no            prevents the graphical display, implies -d\n"
                    << "  -v, --verbose=yes/no                 echos detailed parsing and display information\n"
                    << "  -t, --type=Text/Code                 type of parsing, 'Text' for all .txt, or 'Code' for all source files, defaults to 'Text'\n"
                    << "  -f, --font=FILE                      font to use\n"
                    << "  -n, --font-size-min=[1,999]          REQUIRED, smallest font size, must be <= max, defaults to 30\n"
                    << "  -x, --font-size-max=[1,999]          largest font size, must be >= min, defaults to 400\n"
                    << "  -c, --common=FILE                    file containing common words in order\n"
                    << "  -i, --ignore=FILE                    file containing words to be ignored\n"
                    << "  -m, --parse-html=yes/no              when parsing code also parse html, defaults to no\n"
                    << "  -h, --help                           echos this help message\n"
                    << '\n'
                    << "    Similar to a word cloud, the size and color of each word is proportional\n"
                    << "    to the frequency, or number of times the word was encountered.  The higher\n"
                    << "    the frequency, the larger the font size and the closer to white the word \n"
                    << "    will appear.\n"
                    << '\n'
                    << "    Specifying a common words list can help you understand your text, by using\n"
                    << "    color to indicate how common each word is.  If a common words list is\n"
                    << "    specified with '-c FILE' or '--common=FILE', then any word contained in\n"
                    << "    that list will be colored to indicate where in the file it occurred.\n"
                    << "    White indicates that the word appeared near the top of the list, followed\n"
                    << "    by yellow, then orange, then brown.  A list of the ten-thousand most common\n"
                    << "    English words is provided in 'media/data/text-commons-ordered.txt'.  You can\n"
                    << "    force all common words to be ignored with '-w' or '--will-ignore-common=yes'.\n"
                    << '\n'
                    << "    To specify a list of words that should be ignored, use '-i FILE' or\n"
                    << "    '--ignore=FILE'.  This can be repeated on the command line to specify\n"
                    << "    multiple lists of words to ignore.  A list of all C++ keywords is\n"
                    << "    provided in 'media/data/cpp-keywords.txt', and a list of commonly\n"
                    << "    occurring words in C++ is provided in 'media/data/cpp-commons.txt'.\n"
                    << '\n'
                    << "    A font must be specified on the command line with '-f FILE' or\n"
                    << "    '--font=FILE'."
                    << '\n'
                    << "    Press 'escape', 'q', 'e', or 'c' to exit.\n"
                    << "    Press 'ctrl - c' to cancel parsing while in progress.\n"
                    << "    Press 'return' to toggle between column and jumble view.\n"
                    << "    Press 'spacebar' to toggle between showing and not showing the frequency,"
                    << "    or count, next to each word.\n"
                    << "    Press 's' to save screenshots in PNG format.\n"
                    << "    Press 'r' to save a text report file.\n"
                    << "    Press 'l' to see the line length graph\n"
                    << std::endl;

                exit(EXIT_SUCCESS);
            }

            auto const IS_ARG_PARSE_AS_SHORT{ ARG == M_ARG_PARSE_AS_SHORT };
            if (IS_ARG_PARSE_AS_SHORT || boost::starts_with(ARG, M_ARG_PARSE_AS))
            {
                auto const NEXT_ARG{ ((i < ARGC) ? std::string(ARGV[++i]) : std::string("")) };

                auto const VALUE{ ((IS_ARG_PARSE_AS_SHORT) ?
                    boost::to_lower_copy(NEXT_ARG) :
                    boost::to_lower_copy(boost::replace_all_copy(ARG, M_ARG_PARSE_AS, ""))) };

                for (int p(0); p < ParseType::Count; ++p)
                {
                    auto const ENUM{ static_cast<ParseType::Enum>(p) };

                    auto const ENUM_STR_LOWER{
                        boost::algorithm::to_lower_copy(ParseType::toString(ENUM)) };

                    if (ENUM_STR_LOWER == VALUE)
                    {
                        m_parseType = ENUM;
                        break;
                    }
                }

                M_LOG_AND_ASSERT_OR_THROW((ParseType::Count != m_parseType), 
                    "Invalid argument: " << ARG << " of \"" << VALUE << "\""
                    << " (should be either 'Text' or 'Code')");

                continue;
            }

            auto const FONT_PATH{ parseCommandLineArgFile(
                ARGC,
                ARGV,
                i,
                M_ARG_FONT_FILE,
                M_ARG_FONT_FILE_SHORT) };

            if (FONT_PATH.empty() == false)
            {
                m_fontPath = FONT_PATH;

                if (m_willVerbose)
                {
                    reportMaker.argumentsStream() << "Using Font \"" << m_fontPath
                        << "\"";
                }

                continue;
            }

            auto const COMMON_WORDS_PATH{ parseCommandLineArgFile(
                ARGC,
                ARGV,
                i,
                M_ARG_COMMON_WORD_FILE,
                M_ARG_COMMON_WORD_FILE_SHORT) };

            if (COMMON_WORDS_PATH.empty() == false)
            {
                m_commonWordsPath = COMMON_WORDS_PATH;

                if (m_willVerbose)
                {
                    reportMaker.argumentsStream() << "Using Common Words File \""
                        << m_commonWordsPath << "\"";
                }

                continue;
            }

            auto const IGNORED_WORDS_PATH{ parseCommandLineArgFile(
                ARGC,
                ARGV,
                i,
                M_ARG_IGNORE_FILE,
                M_ARG_IGNORE_FILE_SHORT) };

            if (IGNORED_WORDS_PATH.empty() == false)
            {
                m_ignoreWordsPaths.push_back(IGNORED_WORDS_PATH);

                if (m_willVerbose)
                {
                    reportMaker.argumentsStream() << "Adding Ignored Words File \""
                        << IGNORED_WORDS_PATH << "\"";
                }

                continue;
            }

            auto const FLAGGED_WORDS_PATH{ parseCommandLineArgFile(
                ARGC,
                ARGV,
                i,
                M_ARG_FLAGGED_WORDS_FILE,
                M_ARG_FLAGGED_WORDS_FILE_SHORT) };

            if (FLAGGED_WORDS_PATH.empty() == false)
            {
                m_flaggedWordsPaths.push_back(FLAGGED_WORDS_PATH);

                if (m_willVerbose)
                {
                    reportMaker.argumentsStream() << "Adding Flagged Words File \""
                        << FLAGGED_WORDS_PATH << "\"";
                }

                continue;
            }

            auto const IS_ARG_FONT_SIZE_MAX_SHORT{ ARG == M_ARG_FONT_SIZE_MAX_SHORT };
            if (IS_ARG_FONT_SIZE_MAX_SHORT || boost::starts_with(ARG, M_ARG_FONT_SIZE_MAX))
            {
                auto const NEXT_ARG{ ((i < (ARGC - 1)) ? std::string(ARGV[++i]) : std::string("")) };

                auto const VALUE{ ((IS_ARG_FONT_SIZE_MAX_SHORT) ?
                    NEXT_ARG :
                    boost::replace_all_copy(ARG, M_ARG_FONT_SIZE_MAX, "")) };

                m_fontSizeMax = convertToUnsigned(VALUE);

                if (m_willVerbose)
                {
                    reportMaker.argumentsStream() << "Setting Font Size Max to "
                        << m_fontSizeMax;
                }

                continue;
            }

            auto const IS_ARG_FONT_SIZE_MIN_SHORT{ ARG == M_ARG_FONT_SIZE_MIN_SHORT };
            if (IS_ARG_FONT_SIZE_MIN_SHORT || boost::starts_with(ARG, M_ARG_FONT_SIZE_MIN))
            {
                auto const NEXT_ARG{ ((i < (ARGC - 1)) ? std::string(ARGV[++i]) : std::string("")) };

                auto const VALUE{ ((IS_ARG_FONT_SIZE_MIN_SHORT) ?
                    NEXT_ARG :
                    boost::replace_all_copy(ARG, M_ARG_FONT_SIZE_MAX, "")) };

                m_fontSizeMin = convertToUnsigned(VALUE);

                if (m_willVerbose)
                {
                    reportMaker.argumentsStream() << "Setting Font Size Min to "
                        << m_fontSizeMin;
                }

                continue;
            }
            
            if (parseCommandLineArgFlag(
                    ARG,
                    m_willIgnoreCommonWords,
                    M_ARG_IGNORE_COMMON,
                    M_ARG_IGNORE_COMMON_SHORT))
            {
                if (m_willVerbose)
                {
                    reportMaker.argumentsStream() << "Setting will ignore common words to "
                        << std::boolalpha << m_willIgnoreCommonWords;
                }

                continue;
            }

            if (parseCommandLineArgFlag(
                    ARG,
                    m_willSkipDisplay,
                    M_ARG_SKIP_DISPLAY,
                    M_ARG_SKIP_DISPLAY_SHORT))
            {
                if (m_willVerbose)
                {
                    reportMaker.argumentsStream() << "Setting will skip display to "
                        << std::boolalpha << m_willSkipDisplay;
                }

                continue;
            }

            if (parseCommandLineArgFlag(
                ARG,
                m_willParseHTML,
                M_ARG_PARSE_HTML,
                M_ARG_PARSE_HTML_SHORT))
            {
                if (m_willVerbose)
                {
                    reportMaker.argumentsStream() << "Setting will parse html to "
                        << std::boolalpha << m_willParseHTML;
                }

                continue;
            }

            if (parseCommandLineArgFlag(
                    ARG,
                    m_willVerbose,
                    M_ARG_VERBOSE,
                    M_ARG_VERBOSE_SHORT))
            {
                continue;
            }

            //try and interpret the ARG as a list of single character flags
            if ((ARG.size() > 1) && (ARG[0] == '-'))
            {
                for (std::size_t a(1); a < ARG.size(); ++a)
                {
                    switch (ARG[a])
                    {
                        case 'w':
                        {
                            m_willIgnoreCommonWords = true;

                            reportMaker.argumentsStream() << "Setting will ignore common words to "
                                << std::boolalpha << m_willIgnoreCommonWords;

                            break;
                        }
                        case 's':
                        {
                            m_willSkipDisplay = true;

                            reportMaker.argumentsStream() << "Setting will skip display to "
                                << std::boolalpha << m_willSkipDisplay;

                            break;
                        }
                        case 'v':
                        {
                            m_willVerbose = true;
                            break;
                        }
                        case 'm':
                        {
                            m_willParseHTML = true;
                            break;
                        }
                        default:
                        {
                            reportMaker.errorsStream() << "Invalid argument: '" << ARG[a]
                                << "' ignored.";

                            break;
                        }
                    }
                }

                continue;
            }

            //try and interpret the ARG as a path to be parsed
            {
                namespace bfs = boost::filesystem;

                const bfs::path PATH{ bfs::canonical(bfs::system_complete(ARG)) };

                m_isPathADireectory = bfs::is_directory(PATH);

                M_LOG_AND_ASSERT_OR_THROW((bfs::exists(PATH)),
                    "Invalid argument:  \"" << ARG
                    << "\" could not be interpreted as a path that exists.");

                auto const IS_REGULAR_FILE{ bfs::is_regular_file(PATH) };

                M_LOG_AND_ASSERT_OR_THROW((m_isPathADireectory != IS_REGULAR_FILE),
                    "Invalid argument:  The path '" << PATH.string() << "' "
                    << ((m_isPathADireectory) ? "is" : "is not") << " a directory but it "
                    << ((IS_REGULAR_FILE) ? "is" : "is not") << " a regular file.");

                m_paths.push_back(PATH.string());

                if (m_willVerbose)
                {
                    reportMaker.argumentsStream() << "Will Parse the "
                        << ((IS_REGULAR_FILE) ? "File" : "Directory") << " \""
                        << PATH.string() << "\"";
                }
            }
        }
    }


    void ArgsParser::EnsureValidStartingValues(ReportMaker & reportMaker)
    {
        if (ParseType::Count == m_parseType)
        {
            m_parseType = ParseType::Text;

            if (m_willVerbose)
            {
                reportMaker.argumentsStream() << "Argument '" << M_ARG_PARSE_AS
                    << " missing'.  Defaulting to " << ParseType::toString(m_parseType);
            }
        }

        namespace bfs = boost::filesystem;

        if (m_paths.empty())
        {
            auto const CURRENT_DIR{ bfs::current_path().string() };

            m_paths.push_back(CURRENT_DIR);

            if (m_willVerbose)
            {
                reportMaker.argumentsStream()
                    << "No parse path was specified.  Defaulting to current directory '"
                    << CURRENT_DIR << "'";
            }
        }
        else
        {
            std::sort(m_paths.begin(), m_paths.end());
            m_paths.erase(std::unique(m_paths.begin(), m_paths.end()), m_paths.end());
        }

        if (m_commonWordsPath.empty() && (m_parseType == ParseType::Text))
        {
            auto COMMON_FILE_DEFAULT_PATH{ bfs::system_complete("./common-words-ordered.txt") };
            if ((bfs::exists(COMMON_FILE_DEFAULT_PATH)) &&
                (bfs::is_regular_file(COMMON_FILE_DEFAULT_PATH)))
            {
                m_commonWordsPath = COMMON_FILE_DEFAULT_PATH.string();
            }
        }

        if (m_fontPath.empty())
        {
            m_fontPath = attemptToFindFont();
        }

        M_LOG_AND_ASSERT_OR_THROW((m_fontPath.empty() == false),
            "Missing required argument: A font must be specified with '" << M_ARG_FONT_FILE_SHORT
            << "' or '" << M_ARG_FONT_FILE << "FILE'.");

        const bfs::path FONT_PATH{ bfs::system_complete( bfs::path(m_fontPath) ) };

        M_LOG_AND_ASSERT_OR_THROW((bfs::exists(FONT_PATH)),
            "Invalid argument:  Font " << M_ARG_FONT_FILE << " does not exist'.");

        M_LOG_AND_ASSERT_OR_THROW((bfs::is_regular_file(FONT_PATH)),
            "Invalid argument:  Font at " << FONT_PATH.string() << " is not a regular file.");

        if (m_willIgnoreCommonWords)
        {
            const bfs::path COMMON_WORDS_PATH{ bfs::system_complete(bfs::path(m_commonWordsPath)) };

            M_LOG_AND_ASSERT_OR_THROW((bfs::exists(COMMON_WORDS_PATH)),
                "Ignore Common words is specified, but the " << M_ARG_COMMON_WORD_FILE
                << " file at '" << COMMON_WORDS_PATH.string() << "' does not exist.");

            M_LOG_AND_ASSERT_OR_THROW((bfs::is_regular_file(COMMON_WORDS_PATH)),
                "Ignore Common words is specified, but the " << M_ARG_COMMON_WORD_FILE
                << " file at '" << COMMON_WORDS_PATH.string() << "' is not a regular file.");
        }

        M_LOG_AND_ASSERT_OR_THROW((m_fontSizeMin <= m_fontSizeMax),
            "Invalid argument:  Font size min " << m_fontSizeMin << " is > than font size max "
            << m_fontSizeMax << ".");
    }


    void ArgsParser::ParseVideoMode(ReportMaker & reportMaker)
    {
        auto const VIDEO_MODE{ sf::VideoMode::getDesktopMode() };
        m_screenWidth = static_cast<float>(VIDEO_MODE.width);
        m_screenHeight = static_cast<float>(VIDEO_MODE.height);
        m_bitsPerPixel = VIDEO_MODE.bitsPerPixel;

        if (m_willVerbose)
        {
            reportMaker.miscStream() << "Using detected video mode "
                << VIDEO_MODE.width << "x" << VIDEO_MODE.height << " " << VIDEO_MODE.bitsPerPixel
                << "bpp.";
        }
    }


    unsigned ArgsParser::convertToUnsigned(const std::string & S) const
    {
        int x{ 0 };

        try
        {
            x = boost::lexical_cast<int>(S);
        }
        catch (...)
        {
            x = - 1;
        }

        M_LOG_AND_ASSERT_OR_THROW((x > 0),
            "Invalid Argument:  Font size '" << S << "' is not a valid positive number [1, "
            << M_FONT_SIZE_MAX << "].");

        M_LOG_AND_ASSERT_OR_THROW((x <= M_FONT_SIZE_MAX),
            "Invalid Argument:  Font size '" << S << "' is larger than the max of "
            << M_FONT_SIZE_MAX << ".");

        return static_cast<unsigned>(x);
    }


    bool ArgsParser::parseCommandLineArgFlag(
        const std::string & ARG,
        bool &              memberVar,
        const std::string & CMD_FULL,
        const std::string & CMD_FLAG) const
    {
        auto const IS_ARG_FLAG_VERSION{ ARG == CMD_FLAG };
        if (IS_ARG_FLAG_VERSION || boost::starts_with(ARG, CMD_FULL))
        {
            if (IS_ARG_FLAG_VERSION)
            {
                memberVar = true;
                return true;
            }

            auto value{ boost::replace_all_copy(ARG, CMD_FULL, "") };

            boost::trim(value);
            boost::to_lower(value);

            if (("yes" == value) || ("on" == value) || ("1" == value))
            {
                memberVar = true;
                return true;
            }
            else if (("no" == value) || ("off" == value) || ("0" == value))
            {
                memberVar = false;
                return true;
            }
        }

        return false;
    }


    const std::string ArgsParser::parseCommandLineArgFile(
        const std::size_t   ARGC,
        const char * const  ARGV[],
        std::size_t &       i,
        const std::string & CMD_FULL,
        const std::string & CMD_FLAG) const
    {
        const std::string ARG{ ARGV[i] };

        auto const IS_ARG_FLAG_VERSION{ ARG == CMD_FLAG };
        if (IS_ARG_FLAG_VERSION || boost::starts_with(ARG, CMD_FULL))
        {
            auto const NEXT_ARG{ ((i < (ARGC - 1)) ? std::string(ARGV[++i]) : std::string("")) };

            auto const VALUE{ ((IS_ARG_FLAG_VERSION) ?
                NEXT_ARG :
                boost::replace_all_copy(ARG, CMD_FULL, "")) };

            namespace bfs = boost::filesystem;

            const bfs::path PATH{ bfs::canonical(bfs::system_complete(VALUE)) };

            M_LOG_AND_ASSERT_OR_THROW((bfs::exists(PATH)),
                "Invalid argument:  " << ARG << " of \"" << VALUE << "\" does not exist.");

            M_LOG_AND_ASSERT_OR_THROW((bfs::is_regular_file(PATH)),
                "Invalid argument:  " << ARG << " of \"" << VALUE << "\" is not a regular file.");

            return PATH.string();
        }

        return "";
    }


    const std::string ArgsParser::attemptToFindFont() const
    {
        auto const LOCAL_FONT_PATH{ findLocalFont() };
        if (LOCAL_FONT_PATH.empty() == false)
        {
            return LOCAL_FONT_PATH;
        }

        namespace bfs = boost::filesystem;

        auto WINDOWS_ARIAL_PATH_LOWERCASE{ bfs::system_complete("C:\\Windows\\Fonts\\arial.ttf") };
        if (bfs::exists(WINDOWS_ARIAL_PATH_LOWERCASE))
        {
            return WINDOWS_ARIAL_PATH_LOWERCASE.string();
        }

        auto WINDOWS_ARIAL_PATH_UPPERCASE{ bfs::system_complete("C:\\Windows\\Fonts\\ARIAL.TTF") };
        if (bfs::exists(WINDOWS_ARIAL_PATH_UPPERCASE))
        {
            return WINDOWS_ARIAL_PATH_UPPERCASE.string();
        }

        auto const HOME_PATH{ bfs::system_complete( std::getenv("HOME") ) };
        if (bfs::exists(HOME_PATH) && bfs::is_directory(HOME_PATH))
        {
            auto MAC_ARIAL_PATH_LOWERCASE{ HOME_PATH / bfs::path("Library/Fonts/arial.ttf") };
            if (bfs::exists(MAC_ARIAL_PATH_LOWERCASE))
            {
                return MAC_ARIAL_PATH_LOWERCASE.string();
            }

            auto MAC_ARIAL_PATH_UPPERCASE{ HOME_PATH / bfs::path("Library/Fonts/ARIAL.TTF") };
            if (bfs::exists(MAC_ARIAL_PATH_UPPERCASE))
            {
                return MAC_ARIAL_PATH_UPPERCASE.string();
            }
        }

        return "";
    }


    const std::string ArgsParser::findLocalFont() const
    {
        namespace bfs = boost::filesystem;

        bfs::directory_iterator end_iter;
        for (bfs::directory_iterator iter(bfs::system_complete(".")); iter != end_iter; ++iter)
        {
            auto const PATH_STR{ iter->path().string() };

            if (boost::ends_with(PATH_STR, ".ttf") ||
                boost::ends_with(PATH_STR, ".otf"))
            {
                return PATH_STR;
            }
        }

        return "";
    }

}
