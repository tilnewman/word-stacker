// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// args-parser.cpp
//
#include "args-parser.hpp"
#include "assert-or-throw.hpp"
#include "report-maker.hpp"
#include "strings.hpp"

#include <SFML/Window/VideoMode.hpp>

#include <cstdlib>
#include <filesystem>
#include <iostream>

namespace word_stacker
{

    // all argument strings must be lowercase
    const std::string ArgsParser::M_ARG_PARSE_AS{ "--type=" };
    const std::string ArgsParser::M_ARG_PARSE_AS_SHORT{ "-t" };
    const std::string ArgsParser::M_ARG_FONT_FILE{ "--font=" };
    const std::string ArgsParser::M_ARG_FONT_FILE_SHORT{ "-f" };
    const std::string ArgsParser::M_ARG_COMMON_WORD_FILE{ "--common=" };
    const std::string ArgsParser::M_ARG_COMMON_WORD_FILE_SHORT{ "-c" };
    const std::string ArgsParser::M_ARG_IGNORE_COMMON{ "--will-ignore-common=" };
    const std::string ArgsParser::M_ARG_IGNORE_COMMON_SHORT{ "-w" };
    const std::string ArgsParser::M_ARG_IGNORE_FILE{ "--ignore=" };
    const std::string ArgsParser::M_ARG_IGNORE_FILE_SHORT{ "-i" };
    const std::string ArgsParser::M_ARG_HELP{ "--help" };
    const std::string ArgsParser::M_ARG_HELP_SHORT{ "-h" };
    const std::string ArgsParser::M_ARG_SKIP_DISPLAY{ "--skip-display=" };
    const std::string ArgsParser::M_ARG_SKIP_DISPLAY_SHORT{ "-s" };
    const std::string ArgsParser::M_ARG_VERBOSE{ "--verbose=" };
    const std::string ArgsParser::M_ARG_VERBOSE_SHORT{ "-v" };
    const std::string ArgsParser::M_ARG_FLAGGED_WORDS_FILE{ "--flagged=" };
    const std::string ArgsParser::M_ARG_FLAGGED_WORDS_FILE_SHORT{ "-l" };
    const std::string ArgsParser::M_ARG_PARSE_HTML{ "--parse-html=" };
    const std::string ArgsParser::M_ARG_PARSE_HTML_SHORT{ "-m" };
    const int ArgsParser::M_FONT_SIZE_MAX{ 999 };

    ArgsParser::ArgsParser(
        ReportMaker & reportMaker, const std::size_t ARGC, const char * const ARGV[])
        : m_parseType(ParseType::Count)
        , m_screenWidth(-1.0f)
        , // any negative will work here and below
        m_screenHeight(-1.0f)
        , m_bitsPerPixel(0)
        , m_paths()
        , m_fontPath("")
        , m_commonWordsPath("")
        , m_willIgnoreCommonWords(false)
        , m_ignoreWordsPaths()
        , m_isPathADireectory("")
        , m_fontSizeMin(30)
        , m_fontSizeMax(400)
        , m_willSkipDisplay(false)
        , m_willVerbose(false)
        , m_flaggedWordsPaths()
        , m_willParseHTML(false)
    {
        ParseVideoMode(reportMaker);
        ParseCommandLineArguments(reportMaker, ARGC, ARGV);
        EnsureValidStartingValues(reportMaker);
    }

    void ArgsParser::ParseCommandLineArguments(
        ReportMaker & reportMaker, const std::size_t ARGC, const char * const ARGV[])
    {
        for (std::size_t i(1); i < ARGC; i++)
        {
            auto const ARG{ utilz::trimWhitespaceCopy(std::string(ARGV[i])) };

            if (ARG.empty())
            {
                continue;
            }

            if (ARG == M_ARG_HELP_SHORT)
            {
                std::cout
                    << "  Usage:  word_stacker [-wsvhm] "
                    << "[-t Text/Code] <-f font> [-c common words file] [-i ignored words file] "
                    << "[-n font size min] [-x font size max] <FILE or DIR to parse>\n"
                    << "Try 'word_stacker --help' for more information." << std::endl;

                exit(EXIT_SUCCESS);
            }

            if (utilz::startsWith(ARG, M_ARG_HELP))
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
                    << "  -w, --will-ignore-common=yes/no      prevents the display of words in "
                       "the common words file\n"
                    << "  -s, --skip-display=yes/no            prevents the graphical display, "
                       "implies -d\n"
                    << "  -v, --verbose=yes/no                 echos detailed parsing and display "
                       "information\n"
                    << "  -t, --type=Text/Code                 type of parsing, 'Text' for all "
                       ".txt, or 'Code' for all source files, defaults to 'Text'\n"
                    << "  -f, --font=FILE                      font to use\n"
                    << "  -n, --font-size-min=[1,999]          REQUIRED, smallest font size, must "
                       "be <= max, defaults to 30\n"
                    << "  -x, --font-size-max=[1,999]          largest font size, must be >= min, "
                       "defaults to 400\n"
                    << "  -c, --common=FILE                    file containing common words in "
                       "order\n"
                    << "  -i, --ignore=FILE                    file containing words to be "
                       "ignored\n"
                    << "  -m, --parse-html=yes/no              when parsing code also parse html, "
                       "defaults to no\n"
                    << "  -h, --help                           echos this help message\n"
                    << '\n'
                    << "    Similar to a word cloud, the size and color of each word is "
                       "proportional\n"
                    << "    to the frequency, or number of times the word was encountered.  The "
                       "higher\n"
                    << "    the frequency, the larger the font size and the closer to white the "
                       "word \n"
                    << "    will appear.\n"
                    << '\n'
                    << "    Specifying a common words list can help you understand your text, by "
                       "using\n"
                    << "    color to indicate how common each word is.  If a common words list is\n"
                    << "    specified with '-c FILE' or '--common=FILE', then any word contained "
                       "in\n"
                    << "    that list will be colored to indicate where in the file it occurred.\n"
                    << "    White indicates that the word appeared near the top of the list, "
                       "followed\n"
                    << "    by yellow, then orange, then brown.  A list of the ten-thousand most "
                       "common\n"
                    << "    English words is provided in 'media/data/text-commons-ordered.txt'.  "
                       "You can\n"
                    << "    force all common words to be ignored with '-w' or "
                       "'--will-ignore-common=yes'.\n"
                    << '\n'
                    << "    To specify a list of words that should be ignored, use '-i FILE' or\n"
                    << "    '--ignore=FILE'.  This can be repeated on the command line to specify\n"
                    << "    multiple lists of words to ignore.  A list of all C++ keywords is\n"
                    << "    provided in 'media/data/cpp-keywords.txt', and a list of commonly\n"
                    << "    occurring words in C++ is provided in 'media/data/cpp-commons.txt'.\n"
                    << '\n'
                    << "    A font must be specified on the command line with '-f FILE' or\n"
                    << "    '--font=FILE'." << '\n'
                    << "    Press 'escape', 'q', 'e', or 'c' to exit.\n"
                    << "    Press 'ctrl - c' to cancel parsing while in progress.\n"
                    << "    Press 'return' to toggle between column and jumble view.\n"
                    << "    Press 'spacebar' to toggle between showing and not showing the "
                       "frequency,"
                    << "    or count, next to each word.\n"
                    << "    Press 's' to save screenshots in PNG format.\n"
                    << "    Press 'r' to save a text report file.\n"
                    << "    Press 'l' to see the line length graph\n"
                    << std::endl;

                exit(EXIT_SUCCESS);
            }

            auto const IS_ARG_PARSE_AS_SHORT{ ARG == M_ARG_PARSE_AS_SHORT };
            if (IS_ARG_PARSE_AS_SHORT || utilz::startsWith(ARG, M_ARG_PARSE_AS))
            {
                auto const NEXT_ARG{ ((i < ARGC) ? std::string(ARGV[++i]) : std::string("")) };

                auto const VALUE{ (
                    (IS_ARG_PARSE_AS_SHORT)
                        ? utilz::toLowerCopy(NEXT_ARG)
                        : utilz::toLowerCopy(utilz::replaceAllCopy(ARG, M_ARG_PARSE_AS, ""))) };

                for (int p(0); p < ParseType::Count; ++p)
                {
                    auto const ENUM{ static_cast<ParseType::Enum>(p) };

                    auto const ENUM_STR_LOWER{ utilz::toLowerCopy(ParseType::toString(ENUM)) };

                    if (ENUM_STR_LOWER == VALUE)
                    {
                        m_parseType = ENUM;
                        break;
                    }
                }

                M_LOG_AND_ASSERT_OR_THROW(
                    (ParseType::Count != m_parseType),
                    "Invalid argument: " << ARG << " of \"" << VALUE << "\""
                                         << " (should be either 'Text' or 'Code')");

                continue;
            }

            auto const FONT_PATH{ parseCommandLineArgFile(
                ARGC, ARGV, i, M_ARG_FONT_FILE, M_ARG_FONT_FILE_SHORT) };

            if (FONT_PATH.empty() == false)
            {
                m_fontPath = FONT_PATH;

                if (m_willVerbose)
                {
                    reportMaker.argumentsStream() << "Using Font \"" << m_fontPath << "\"";
                }

                continue;
            }

            auto const COMMON_WORDS_PATH{ parseCommandLineArgFile(
                ARGC, ARGV, i, M_ARG_COMMON_WORD_FILE, M_ARG_COMMON_WORD_FILE_SHORT) };

            if (COMMON_WORDS_PATH.empty() == false)
            {
                m_commonWordsPath = COMMON_WORDS_PATH;

                if (m_willVerbose)
                {
                    reportMaker.argumentsStream()
                        << "Using Common Words File \"" << m_commonWordsPath << "\"";
                }

                continue;
            }

            auto const IGNORED_WORDS_PATH{ parseCommandLineArgFile(
                ARGC, ARGV, i, M_ARG_IGNORE_FILE, M_ARG_IGNORE_FILE_SHORT) };

            if (IGNORED_WORDS_PATH.empty() == false)
            {
                m_ignoreWordsPaths.push_back(IGNORED_WORDS_PATH);

                if (m_willVerbose)
                {
                    reportMaker.argumentsStream()
                        << "Adding Ignored Words File \"" << IGNORED_WORDS_PATH << "\"";
                }

                continue;
            }

            auto const FLAGGED_WORDS_PATH{ parseCommandLineArgFile(
                ARGC, ARGV, i, M_ARG_FLAGGED_WORDS_FILE, M_ARG_FLAGGED_WORDS_FILE_SHORT) };

            if (FLAGGED_WORDS_PATH.empty() == false)
            {
                m_flaggedWordsPaths.push_back(FLAGGED_WORDS_PATH);

                if (m_willVerbose)
                {
                    reportMaker.argumentsStream()
                        << "Adding Flagged Words File \"" << FLAGGED_WORDS_PATH << "\"";
                }

                continue;
            }

            if (parseCommandLineArgFlag(
                    ARG, m_willIgnoreCommonWords, M_ARG_IGNORE_COMMON, M_ARG_IGNORE_COMMON_SHORT))
            {
                if (m_willVerbose)
                {
                    reportMaker.argumentsStream() << "Setting will ignore common words to "
                                                  << std::boolalpha << m_willIgnoreCommonWords;
                }

                continue;
            }

            if (parseCommandLineArgFlag(
                    ARG, m_willSkipDisplay, M_ARG_SKIP_DISPLAY, M_ARG_SKIP_DISPLAY_SHORT))
            {
                if (m_willVerbose)
                {
                    reportMaker.argumentsStream()
                        << "Setting will skip display to " << std::boolalpha << m_willSkipDisplay;
                }

                continue;
            }

            if (parseCommandLineArgFlag(
                    ARG, m_willParseHTML, M_ARG_PARSE_HTML, M_ARG_PARSE_HTML_SHORT))
            {
                if (m_willVerbose)
                {
                    reportMaker.argumentsStream()
                        << "Setting will parse html to " << std::boolalpha << m_willParseHTML;
                }

                continue;
            }

            if (parseCommandLineArgFlag(ARG, m_willVerbose, M_ARG_VERBOSE, M_ARG_VERBOSE_SHORT))
            {
                continue;
            }

            // try and interpret the ARG as a list of single character flags
            if ((ARG.size() > 1) && (ARG[0] == '-'))
            {
                for (std::size_t a(1); a < ARG.size(); ++a)
                {
                    switch (ARG[a])
                    {
                        case 'w':
                        {
                            m_willIgnoreCommonWords = true;

                            reportMaker.argumentsStream()
                                << "Setting will ignore common words to " << std::boolalpha
                                << m_willIgnoreCommonWords;

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
                            reportMaker.errorsStream()
                                << "Invalid argument: '" << ARG[a] << "' ignored.";

                            break;
                        }
                    }
                }

                continue;
            }

            // try and interpret the ARG as a path to be parsed
            {
                const std::filesystem::path PATH{ std::filesystem::canonical(ARG) };

                m_isPathADireectory = std::filesystem::is_directory(PATH);

                M_LOG_AND_ASSERT_OR_THROW(
                    (std::filesystem::exists(PATH)),
                    "Invalid argument:  \""
                        << ARG << "\" could not be interpreted as a path that exists.");

                auto const IS_REGULAR_FILE{ std::filesystem::is_regular_file(PATH) };

                M_LOG_AND_ASSERT_OR_THROW(
                    (m_isPathADireectory != IS_REGULAR_FILE),
                    "Invalid argument:  The path '"
                        << PATH.string() << "' " << ((m_isPathADireectory) ? "is" : "is not")
                        << " a directory but it " << ((IS_REGULAR_FILE) ? "is" : "is not")
                        << " a regular file.");

                m_paths.push_back(PATH.string());

                if (m_willVerbose)
                {
                    reportMaker.argumentsStream()
                        << "Will Parse the " << ((IS_REGULAR_FILE) ? "File" : "Directory") << " \""
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
                reportMaker.argumentsStream()
                    << "Argument '" << M_ARG_PARSE_AS << " missing'.  Defaulting to "
                    << ParseType::toString(m_parseType);
            }
        }

        namespace fs = std::filesystem;

        if (m_paths.empty())
        {
            auto const CURRENT_DIR{ fs::current_path().string() };

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
            auto COMMON_FILE_DEFAULT_PATH{ fs::canonical("./common-words-ordered.txt") };
            if ((fs::exists(COMMON_FILE_DEFAULT_PATH)) &&
                (fs::is_regular_file(COMMON_FILE_DEFAULT_PATH)))
            {
                m_commonWordsPath = COMMON_FILE_DEFAULT_PATH.string();
            }
        }

        if (m_fontPath.empty())
        {
            m_fontPath = attemptToFindFont();
        }

        M_LOG_AND_ASSERT_OR_THROW(
            (m_fontPath.empty() == false),
            "Missing required argument: A font must be specified with '"
                << M_ARG_FONT_FILE_SHORT << "' or '" << M_ARG_FONT_FILE << "FILE'.");

        const fs::path FONT_PATH{ fs::canonical(fs::path(m_fontPath)) };

        M_LOG_AND_ASSERT_OR_THROW(
            (fs::exists(FONT_PATH)),
            "Invalid argument:  Font " << M_ARG_FONT_FILE << " does not exist'.");

        M_LOG_AND_ASSERT_OR_THROW(
            (fs::is_regular_file(FONT_PATH)),
            "Invalid argument:  Font at " << FONT_PATH.string() << " is not a regular file.");

        if (m_willIgnoreCommonWords)
        {
            const fs::path COMMON_WORDS_PATH{ fs::canonical(fs::path(m_commonWordsPath)) };

            M_LOG_AND_ASSERT_OR_THROW(
                (fs::exists(COMMON_WORDS_PATH)),
                "Ignore Common words is specified, but the "
                    << M_ARG_COMMON_WORD_FILE << " file at '" << COMMON_WORDS_PATH.string()
                    << "' does not exist.");

            M_LOG_AND_ASSERT_OR_THROW(
                (fs::is_regular_file(COMMON_WORDS_PATH)),
                "Ignore Common words is specified, but the "
                    << M_ARG_COMMON_WORD_FILE << " file at '" << COMMON_WORDS_PATH.string()
                    << "' is not a regular file.");
        }

        M_LOG_AND_ASSERT_OR_THROW(
            (m_fontSizeMin <= m_fontSizeMax),
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
            reportMaker.miscStream()
                << "Using detected video mode " << VIDEO_MODE.width << "x" << VIDEO_MODE.height
                << " " << VIDEO_MODE.bitsPerPixel << "bpp.";
        }
    }

    bool ArgsParser::parseCommandLineArgFlag(
        const std::string & ARG,
        bool & memberVar,
        const std::string & CMD_FULL,
        const std::string & CMD_FLAG) const
    {
        auto const IS_ARG_FLAG_VERSION{ ARG == CMD_FLAG };
        if (IS_ARG_FLAG_VERSION || utilz::startsWith(ARG, CMD_FULL))
        {
            if (IS_ARG_FLAG_VERSION)
            {
                memberVar = true;
                return true;
            }

            auto value{ utilz::replaceAllCopy(ARG, CMD_FULL, "") };

            utilz::trimWhitespace(value);
            utilz::toLower(value);

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
        const std::size_t ARGC,
        const char * const ARGV[],
        std::size_t & i,
        const std::string & CMD_FULL,
        const std::string & CMD_FLAG) const
    {
        const std::string ARG{ ARGV[i] };

        auto const IS_ARG_FLAG_VERSION{ ARG == CMD_FLAG };
        if (IS_ARG_FLAG_VERSION || utilz::startsWith(ARG, CMD_FULL))
        {
            auto const NEXT_ARG{ ((i < (ARGC - 1)) ? std::string(ARGV[++i]) : std::string("")) };

            auto const VALUE{ (
                (IS_ARG_FLAG_VERSION) ? NEXT_ARG : utilz::replaceAllCopy(ARG, CMD_FULL, "")) };

            namespace fs = std::filesystem;

            const fs::path PATH{ fs::canonical(VALUE) };

            M_LOG_AND_ASSERT_OR_THROW(
                (fs::exists(PATH)),
                "Invalid argument:  " << ARG << " of \"" << VALUE << "\" does not exist.");

            M_LOG_AND_ASSERT_OR_THROW(
                (fs::is_regular_file(PATH)),
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

        namespace fs = std::filesystem;

        auto WINDOWS_ARIAL_PATH_LOWERCASE{ fs::canonical("C:\\Windows\\Fonts\\arial.ttf") };
        if (fs::exists(WINDOWS_ARIAL_PATH_LOWERCASE))
        {
            return WINDOWS_ARIAL_PATH_LOWERCASE.string();
        }

        auto WINDOWS_ARIAL_PATH_UPPERCASE{ fs::canonical("C:\\Windows\\Fonts\\ARIAL.TTF") };
        if (fs::exists(WINDOWS_ARIAL_PATH_UPPERCASE))
        {
            return WINDOWS_ARIAL_PATH_UPPERCASE.string();
        }

        auto const HOME_PATH{ fs::canonical(std::getenv("HOME")) };
        if (fs::exists(HOME_PATH) && fs::is_directory(HOME_PATH))
        {
            auto MAC_ARIAL_PATH_LOWERCASE{ HOME_PATH / fs::path("Library/Fonts/arial.ttf") };
            if (fs::exists(MAC_ARIAL_PATH_LOWERCASE))
            {
                return MAC_ARIAL_PATH_LOWERCASE.string();
            }

            auto MAC_ARIAL_PATH_UPPERCASE{ HOME_PATH / fs::path("Library/Fonts/ARIAL.TTF") };
            if (fs::exists(MAC_ARIAL_PATH_UPPERCASE))
            {
                return MAC_ARIAL_PATH_UPPERCASE.string();
            }
        }

        return "";
    }

    const std::string ArgsParser::findLocalFont() const
    {
        namespace fs = std::filesystem;

        fs::directory_iterator end_iter;
        for (fs::directory_iterator iter(fs::canonical(".")); iter != end_iter; ++iter)
        {
            auto const PATH_STR{ iter->path().string() };

            if (utilz::endsWith(PATH_STR, ".ttf") || utilz::endsWith(PATH_STR, ".otf"))
            {
                return PATH_STR;
            }
        }

        return "";
    }

} // namespace word_stacker
