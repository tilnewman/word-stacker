// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// file-parser.cpp
//
#include "assert-or-throw.hpp"
#include "file-parser.hpp"
#include "strings.hpp"
#include "word-count-stats.hpp"
#include "word-list.hpp"

#include <boost/algorithm/algorithm.hpp>
#include <boost/algorithm/string.hpp>

#include <algorithm>
#include <bitset>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <map>
#include <sstream>
#include <vector>

namespace word_stacker
{

    FileParser::FileParser(
        ReportMaker & reportMaker,
        const ArgsParser & ARGS,
        const WordList & COMMON_WORDS,
        const WordList & IGNORED_WORDS,
        const WordList & FLAGGED_WORDS)
        : m_wordCounts()
        , m_lineCount(0)
        , m_unCLineCount(0)
        , m_singleCount(0)
        , m_ignoredCount(0)
        , m_fileCount(0)
        , m_dirCount(0)
        , m_flaggedCount(0)
        , m_lengthCountMap()
    {
        M_LOG_AND_ASSERT_OR_THROW(
            ((ARGS.parseAs() == ParseType::Text) || (ARGS.parseAs() == ParseType::Code)),
            "word_stacker::FileParser::Constructor(parse_type="
                << ARGS.parseAs() << ") that parse type is not yet supported.");

        ParseSupplies parseSupplies(reportMaker, ARGS, COMMON_WORDS, IGNORED_WORDS, FLAGGED_WORDS);

        auto const & PATHS{ ARGS.parsePaths() };
        for (auto const & PATH : PATHS)
        {
            parseDirectoryOrFile(parseSupplies, PATH);
        }

        if (parseSupplies.m_wordCountMap.empty())
        {
            std::cerr << "  Failed to parse any words from the following paths:\n";

            for (auto const & PATH : PATHS)
            {
                std::cerr << "\t'" << PATH << "'\n";
            }

            std::cerr << std::endl;

            exit(EXIT_FAILURE);
        }

        m_wordCounts.reserve(parseSupplies.m_wordCountMap.size());
        for (auto const & PAIR : parseSupplies.m_wordCountMap)
        {
            m_wordCounts.emplace_back(PAIR.first, PAIR.second);
        }

        parseSupplies.m_wordCountMap.clear();

        auto const STATS{ Statistics::calculate(
            m_wordCounts, "Frequency List\t", parseSupplies.m_reportMaker.frequencyListLength()) };

        logStatistics(parseSupplies.m_reportMaker, STATS);
    }

    const WordCount & FileParser::wordCountObj(const std::size_t INDEX) const
    {
        M_LOG_AND_ASSERT_OR_THROW(
            (INDEX < m_wordCounts.size()),
            "FileParser::wordCountObj(index=" << INDEX << ") index out of range.  (max="
                                              << m_wordCounts.size() << ")");

        return m_wordCounts[INDEX];
    }

    void FileParser::parseDirectoryOrFile(ParseSupplies & supplies, const std::string & PATH_STR)
    {
        namespace fs = std::filesystem;

        auto const PATH{ fs::canonical(fs::path(PATH_STR)) };

        if (fs::is_directory(PATH))
        {
            ++m_dirCount;
            fs::directory_iterator end_iter;
            for (fs::directory_iterator iter(PATH); iter != end_iter; ++iter)
            {
                parseDirectoryOrFile(supplies, iter->path().string());
            }
        }
        else if (fs::is_regular_file(PATH))
        {
            if (doesFilenameMatchParseType(supplies, PATH.string()))
            {
                openFileAndParse(supplies, PATH.string());
            }
        }
    }

    bool FileParser::doesFilenameMatchParseType(
        ParseSupplies & supplies, const std::string & FILENAME) const
    {
        StrVec_t fileExtensions;

        switch (supplies.M_ARGS.parseAs())
        {
            case ParseType::Text:
            {
                const StrVec_t TEXT_FILE_EXTENSIONS = { ".txt", ".rtf" };

                fileExtensions = TEXT_FILE_EXTENSIONS;
                break;
            }
            case ParseType::Code:
            {
                const StrVec_t CODE_FILE_EXTENSIONS = {
                    ".hpp",  ".cpp",  ".h",    ".c",      ".cs",  ".class", ".java", ".rb",
                    ".rake", ".php",  ".php3", ".php4",   ".js",  ".m",     ".mm",   ".cmd",
                    ".bat",  ".asm",  ".s",    ".sh",     ".bat", ".hxx",   ".cxx",  ".jsp",
                    ".ll",   ".pl",   ".y",    ".yxx",    ".asp", ".aspx",  ".inc",  ".jsp",
                    ".jspx", ".scpt", ".do",   ".action", ".wss", ".pl"
                };

                fileExtensions = CODE_FILE_EXTENSIONS;

                if (supplies.M_ARGS.willParseHTML())
                {
                    const StrVec_t HTML_FILE_EXTENSIONS = { ".xslt", ".css",   ".xsl",   ".htm",
                                                            ".html", ".xhtml", ".jhtml", ".phtml",
                                                            ".rss",  ".xml" };

                    std::copy(
                        HTML_FILE_EXTENSIONS.begin(),
                        HTML_FILE_EXTENSIONS.end(),
                        std::back_inserter(fileExtensions));
                }

                break;
            }
            case ParseType::Count:
            default:
            {
                break;
            }
        }

        for (auto const & EXTENSION : fileExtensions)
        {
            if (utilz::endsWith(FILENAME, EXTENSION))
            {
                return true;
            }
        }

        return false;
    }

    void FileParser::openFileAndParse(ParseSupplies & supplies, const std::string & FILE_PATH)
    {
        std::ifstream file;
        file.open(FILE_PATH);

        M_LOG_AND_ASSERT_OR_THROW(
            (file.is_open()),
            "word_stacker::FileParser::ParseAsText(file_path=\""
                << FILE_PATH << "\") failed to open that file.");

        try
        {
            switch (supplies.M_ARGS.parseAs())
            {
                case ParseType::Text:
                {
                    parseFileContentsText(supplies, file);
                    break;
                }
                case ParseType::Code:
                {
                    parseFileContentsCode(supplies, file);
                    break;
                }
                case ParseType::Count:
                default:
                {
                    break;
                }
            }
        }
        catch (...)
        {
            file.close();
            throw;
        }

        file.close();
    }

    void FileParser::parseFileContentsText(ParseSupplies & supplies, std::ifstream & file)
    {
        ++m_fileCount;

        const std::string CHARS_TO_KEEP{
            "qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDSFGHJKLZXCVBNM-'"
        };

        std::string line;
        while (std::getline(file, line))
        {
            ++m_lineCount;
            ++m_unCLineCount;

            utilz::replaceAll(line, "\r", "");
            utilz::replaceAll(line, "\n", "");

            m_lengthCountMap[line.length()]++;

            std::istringstream iss(line);
            std::string word;
            while (iss >> word)
            {
                boost::to_lower(word);
                utilz::trimWhitespace(word);

                changeInvalidCharactersToSpaces(word, CHARS_TO_KEEP);
                utilz::trimWhitespace(word);

                // Note that these two replacements, when in this order,
                // handles the case of four spaces appearing together.
                utilz::replaceAll(word, "   ", " ");
                utilz::replaceAll(word, "  ", " ");

                // handle words that had unicode apostrophes or ellipsis
                if (word.find(' ') != std::string::npos)
                {
                    // handle apostrophes
                    const StrVec_t APOSTR_WORD_ENDINGS = { " t ",  " s ",  " d ", " m ",
                                                           " ll ", " ve ", " re " };

                    for (auto const & ENDING : APOSTR_WORD_ENDINGS)
                    {
                        const std::string REPLACEMENT = [&]() {
                            std::string rep = ENDING;
                            rep[0] = '\'';
                            return rep;
                        }();

                        utilz::replaceAll(word, ENDING, REPLACEMENT);

                        const std::string ENDING_RIGHT_TRIM = ENDING.substr(0, (ENDING.size() - 2));

                        if (utilz::endsWith(word, ENDING_RIGHT_TRIM))
                        {
                            utilz::replaceAll(
                                word,
                                ENDING_RIGHT_TRIM,
                                utilz::replaceAllCopy(ENDING_RIGHT_TRIM, " ", "'"));
                        }
                    }

                    // handle ellipsis
                    if (word.find(' ') != std::string::npos)
                    {
                        std::istringstream issSubWords(word);
                        std::string subWord{ "" };
                        while (issSubWords >> subWord)
                        {
                            if (("-" != subWord) && ("'" != subWord))
                            {
                                parseWord(supplies, subWord);
                            }
                        }

                        continue;
                    }
                }

                if (("-" != word) && ("'" != word))
                {
                    parseWord(supplies, word);
                }
            }
        }
    }

    void FileParser::parseFileContentsCode(ParseSupplies & supplies, std::ifstream & file)
    {
        ++m_fileCount;

        const std::string CHARS_TO_KEEP{
            "qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDSFGHJKLZXCVBNM1234567890_"
        };

        std::string line;
        while (std::getline(file, line))
        {
            ++m_lineCount;
            ++m_unCLineCount;

            utilz::replaceAll(line, "\r", "");
            utilz::replaceAll(line, "\n", "");

            auto const ORIG_LINE_LENGTH{ line.length() };

            utilz::trimWhitespace(line);

            if (line.empty())
            {
                continue;
            }

            if (utilz::startsWith(line, "//"))
            {
                --m_unCLineCount;
                continue;
            }
            else
            {
                m_lengthCountMap[ORIG_LINE_LENGTH]++;
            }

            // remove in-line comments
            auto const COMMENT_POS{ line.find("//") };
            if (COMMENT_POS != std::string::npos)
            {
                line = line.substr(0, COMMENT_POS);
            }

            utilz::replaceAll(line, "\\\"", "");

            // remove text in double quotes
            if (line.find('\"') != std::string::npos)
            {
                auto isDeletingString{ false };

                for (auto iter{ line.begin() }; iter != line.end(); ++iter)
                {
                    if ('\"' == *iter)
                    {
                        *iter = ' ';
                        isDeletingString = !isDeletingString;
                    }

                    if (isDeletingString)
                    {
                        *iter = ' ';
                    }
                }
            }

            changeInvalidCharactersToSpaces(line, CHARS_TO_KEEP);
            utilz::trimWhitespace(line);

            std::istringstream lineSS(line);
            std::string word;
            while (lineSS >> word)
            {
                utilz::trimWhitespace(word);
                parseWord(supplies, word);
            }
        }
    }

    void FileParser::parseWord(ParseSupplies & supplies, const std::string & WORD)
    {
        auto const LENGTH{ WORD.size() };

        if (0 == LENGTH)
        {
            return;
        }

        if (LENGTH == 1)
        {
            ++m_singleCount;
        }

        if (supplies.M_FLAGGED_WORDS.contains(WORD))
        {
            ++m_flaggedCount;
        }

        auto const WILL_SKIP_IGNORED_COMMON{ (
            supplies.M_ARGS.willIgnoreCommonWords() && supplies.M_COMMON_WORDS.contains(WORD)) };

        auto const WILL_SKIP_IGNORED{ supplies.M_IGNORED_WORDS.contains(WORD) };

        if (WILL_SKIP_IGNORED_COMMON || WILL_SKIP_IGNORED)
        {
            ++m_ignoredCount;
        }
        else
        {
            supplies.m_wordCountMap[WORD]++;
        }
    }

    void FileParser::logStatistics(ReportMaker & reportMaker, const FreqStats & STATS)
    {
        reportMaker.fileStatsStream()
            << m_fileCount << " File" << ((1 == m_fileCount) ? "" : "s") << " Parsed";

        reportMaker.fileStatsStream()
            << m_dirCount << " Director" << ((1 == m_dirCount) ? "y" : "ies") << " Parsed";

        reportMaker.fileStatsStream() << '-';
        reportMaker.fileStatsStream() << "Total Line Count\t=" << m_lineCount;

        if (m_unCLineCount > 0)
        {
            reportMaker.fileStatsStream() << "Line Count Excluding Comments\t=" << m_unCLineCount;
        }

        reportMaker.fileStatsStream() << '-';
        reportMaker.fileStatsStream() << "Single Letter or Number Word Count\t=" << m_singleCount;
        reportMaker.fileStatsStream() << "Ignored Word Count\t=" << m_ignoredCount;
        reportMaker.fileStatsStream() << "Unique Word Count\t=" << STATS.unique;
        reportMaker.fileStatsStream() << "Flagged Word Count\t=" << m_flaggedCount;
        reportMaker.fileStatsStream() << "Total Word Count\t=" << STATS.sum;
        reportMaker.fileStatsStream() << '-';
        reportMaker.fileStatsStream() << "Frequency Minimum\t=" << STATS.min;
        reportMaker.fileStatsStream() << "Frequency Average\t=" << STATS.average;
        reportMaker.fileStatsStream() << "Frequency Median\t=" << STATS.median;
        reportMaker.fileStatsStream() << "Frequency Maximum\t=" << STATS.max;
        reportMaker.fileStatsStream() << "Frequency StdDev\t=" << STATS.stddev;
        reportMaker.fileStatsStream() << '-';

        for (auto const & FREQ_STR : STATS.freqs)
        {
            reportMaker.fileStatsStream() << FREQ_STR;
        }
    }

    void FileParser::changeInvalidCharactersToSpaces(
        std::string & s, const std::string & CHARS_TO_KEEP) const
    {
        for (auto iter{ s.begin() }; iter != s.end(); ++iter)
        {
            if (CHARS_TO_KEEP.find(*iter) == std::string::npos)
            {
                *iter = ' ';
            }
        }
    }

} // namespace word_stacker
