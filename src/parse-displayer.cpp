// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// parse-displayer.cpp
//
#include "assert-or-throw.hpp"
#include "parse-displayer.hpp"
#include "word-count-stats.hpp"

#include <filesystem>

#include <algorithm>
#include <sstream>

namespace word_stacker
{

    const std::size_t ParseDisplayer::M_GRADIENT_DEPTH{ 4096 };

    ParseDisplayer::ParseDisplayer(const ArgsParser & ARGS)
        : m_uniqueColors(
              sf::Color(sf::Uint8(100), sf::Uint8(100), sf::Uint8(255)),
              sf::Color(sf::Uint8(192), sf::Uint8(192), sf::Uint8(255)),
              M_GRADIENT_DEPTH)
        , m_commonColors()
        , m_font()
        , m_texts()
        , m_willShowLineLengthGraph(false)
    {
        // create a brown-orange-yellow-white color gradient
        ColorRangeVec_t colorRanges = {
            ColorRange(
                sf::Color(160, 82, 45), // brown
                sf::Color(255, 165, 0), // orange
                M_GRADIENT_DEPTH * 4),
            ColorRange(
                sf::Color(255, 165, 0), // orange
                sf::Color::Yellow,
                M_GRADIENT_DEPTH * 2),
            ColorRange(sf::Color::Yellow, sf::Color::White, M_GRADIENT_DEPTH / 32)
        };

        m_commonColors = ColorRange(colorRanges);

        if (ARGS.willSkipDisplay() == false)
        {
            loadFont(ARGS);
        }
    }

    void ParseDisplayer::draw(
        const ArgsParser & ARGS,
        const FileParser & PARSED_FILE,
        sf::RenderTarget & target,
        sf::RenderStates states) const
    {
        if (m_willShowLineLengthGraph)
        {
            drawLineLengthGraph(ARGS, PARSED_FILE, target, states);
        }
        else
        {
            for (auto const & TEXT : m_texts)
            {
                target.draw(TEXT, states);
            }
        }
    }

    void ParseDisplayer::setup(
        ReportMaker & reportMaker,
        const ArgsParser & ARGS,
        const FileParser & PARSED_FILE,
        const WordList & COMMON_WORDS,
        const WordList & FLAGGED_WORDS,
        const bool WILL_SHOW_COUNTS,
        const bool WILL_SHOW_COLUMS,
        const bool WILL_SHOW_LINELEN_GRAPH)
    {
        m_willShowLineLengthGraph = WILL_SHOW_LINELEN_GRAPH;

        if (m_willShowLineLengthGraph)
        {
            return;
        }

        const std::size_t MAX_WORDS_DISPLAY_COUNT{ 500 };

        m_texts.clear();
        m_texts.reserve(MAX_WORDS_DISPLAY_COUNT);

        WordCountVec_t wordCounts;
        {
            auto const PARSED_WORDS{ PARSED_FILE.words() };

            M_LOG_AND_ASSERT_OR_THROW(
                (PARSED_WORDS.empty() == false),
                "ParseDisplayer::setupDisplay() the FileParser contained no parsed words.");

            auto iter{ PARSED_WORDS.begin() };
            auto const ADVANCE_COUNT{ std::min(MAX_WORDS_DISPLAY_COUNT, PARSED_WORDS.size()) };
            std::advance(iter, static_cast<int>(ADVANCE_COUNT));
            std::copy(PARSED_WORDS.begin(), iter, std::back_inserter(wordCounts));
        }

        std::size_t displayedCount{ 0 };
        if (WILL_SHOW_COLUMS)
        {
            displayedCount =
                setupAsColumns(ARGS, COMMON_WORDS, FLAGGED_WORDS, WILL_SHOW_COUNTS, wordCounts);
        }
        else
        {
            displayedCount =
                setupAsJumble(ARGS, COMMON_WORDS, FLAGGED_WORDS, WILL_SHOW_COUNTS, wordCounts);
        }

        reportMaker.displayStatsStreamClear();

        reportMaker.displayStatsStream() << "Displayed " << displayedCount << " words in "
                                         << ((WILL_SHOW_COLUMS) ? "columns" : "jumble") << " mode"
                                         << ((WILL_SHOW_COUNTS) ? " with counts showing" : "");

        wordCounts.resize(displayedCount);

        auto const STATS{ Statistics::calculate(
            wordCounts, "Displayed Frequency List:\t", reportMaker.frequencyListLength()) };

        logStatistics(reportMaker, STATS);
    }

    std::size_t ParseDisplayer::setupAsColumns(
        const ArgsParser & ARGS,
        const WordList & COMMON_WORDS,
        const WordList & FLAGGED_WORDS,
        const bool WILL_SHOW_COUNTS,
        WordCountVec_t & wordCounts)
    {
        unsigned fontSizeMax{ ARGS.fontSizeMax() };

        auto displayCount{ setupAsColumnsAtFontSize(
            ARGS, COMMON_WORDS, FLAGGED_WORDS, WILL_SHOW_COUNTS, fontSizeMax, wordCounts) };

        while (0 == displayCount)
        {
            fontSizeMax /= 2;
            if (fontSizeMax < 2)
            {
                std::cerr << "Most frequent word \"" << wordCounts[0].word()
                          << "\" will not fit on screen." << std::endl;
                break;
            }

            displayCount = setupAsColumnsAtFontSize(
                ARGS, COMMON_WORDS, FLAGGED_WORDS, WILL_SHOW_COUNTS, fontSizeMax, wordCounts);
        }

        return displayCount;
    }

    std::size_t ParseDisplayer::setupAsColumnsAtFontSize(
        const ArgsParser & ARGS,
        const WordList & COMMON_WORDS,
        const WordList & FLAGGED_WORDS,
        const bool WILL_SHOW_COUNTS,
        const unsigned FONT_SIZE_MAX,
        WordCountVec_t & wordCounts)
    {
        auto const COLUMN_TO_COLUMN_PAD{ 50.0f };

        float posLeft{ 0.0f };
        std::size_t wordCountsIndex{ 0 };

        while (wordCountsIndex < wordCounts.size())
        {
            setupColumn(
                ARGS,
                posLeft,
                wordCounts,
                wordCountsIndex,
                COMMON_WORDS,
                FLAGGED_WORDS,
                WILL_SHOW_COUNTS,
                FONT_SIZE_MAX);

            posLeft += COLUMN_TO_COLUMN_PAD;

            if (posLeft > ARGS.screenWidthF())
            {
                break;
            }
        }

        return wordCountsIndex;
    }

    std::size_t ParseDisplayer::setupAsJumble(
        const ArgsParser & ARGS,
        const WordList & COMMON_WORDS,
        const WordList & FLAGGED_WORDS,
        const bool WILL_SHOW_COUNTS,
        WordCountVec_t & wordCounts)
    {
        unsigned fontSizeMax{ ARGS.fontSizeMax() };

        auto displayCount{ setupAsJumbleAtFontSize(
            ARGS, COMMON_WORDS, FLAGGED_WORDS, WILL_SHOW_COUNTS, fontSizeMax, wordCounts) };

        while (0 == displayCount)
        {
            fontSizeMax /= 2;
            if (fontSizeMax < 2)
            {
                std::cerr << "Most frequent word \"" << wordCounts[0].word()
                          << "\" will not fit on screen." << std::endl;
                break;
            }

            displayCount = setupAsJumbleAtFontSize(
                ARGS, COMMON_WORDS, FLAGGED_WORDS, WILL_SHOW_COUNTS, fontSizeMax, wordCounts);
        }

        return displayCount;
    }

    std::size_t ParseDisplayer::setupAsJumbleAtFontSize(
        const ArgsParser & ARGS,
        const WordList & COMMON_WORDS,
        const WordList & FLAGGED_WORDS,
        const bool WILL_SHOW_COUNTS,
        const unsigned FONT_SIZE_MAX,
        WordCountVec_t & wordCounts)
    {
        RectVec_t rects;
        std::size_t wordCountsIndex{ 0 };
        while (wordCountsIndex < wordCounts.size())
        {
            if (placeNextJumbledWord(
                    ARGS,
                    COMMON_WORDS,
                    FLAGGED_WORDS,
                    WILL_SHOW_COUNTS,
                    rects,
                    wordCounts,
                    wordCountsIndex,
                    FONT_SIZE_MAX) == false)
            {
                break;
            }
            else
            {
                ++wordCountsIndex;
            }
        }

        return wordCountsIndex;
    }

    bool ParseDisplayer::placeNextJumbledWord(
        const ArgsParser & ARGS,
        const WordList & COMMON_WORDS,
        const WordList & FLAGGED_WORDS,
        const bool WILL_SHOW_COUNTS,
        RectVec_t & rects,
        const WordCountVec_t & WORDS,
        const std::size_t WORDS_INDEX,
        const unsigned FONT_SIZE_MAX)
    {
        auto const FREQ_MIN{ WORDS[WORDS.size() - 1].count() };
        auto const HIGHEST_WORD_FREQ_F{ static_cast<float>(WORDS[0].count()) };
        auto const FREQ{ WORDS[WORDS_INDEX].count() };

        auto const FREQ_RATIO{ (
            ((WORDS.size() == 1) || ((FREQ - FREQ_MIN) == 0))
                ? 1.0f
                : (static_cast<float>(FREQ - FREQ_MIN) /
                   static_cast<float>(HIGHEST_WORD_FREQ_F - static_cast<float>(FREQ_MIN)))) };

        auto const WORD{ WORDS[WORDS_INDEX].word() };
        auto const COMMON_ORDER{ COMMON_WORDS.order(WORD) };
        auto const COMMON_WORDS_COUNT_F{ static_cast<float>(COMMON_WORDS.count()) };

        auto const COMMON_RATIO{ (COMMON_WORDS_COUNT_F - static_cast<float>(COMMON_ORDER)) /
                                 COMMON_WORDS_COUNT_F };

        const unsigned FONT_SIZE_COUNT{ 30 };
        const unsigned FONT_SIZE_MIN{ std::min(ARGS.fontSizeMin(), (FONT_SIZE_MAX - 1)) };

        // make an sf::Text object for the word
        auto const FONT_SIZE{ FONT_SIZE_MIN +
                              static_cast<unsigned>(
                                  static_cast<float>(FONT_SIZE_MAX - FONT_SIZE_MIN) * FREQ_RATIO) };

        sf::Text wordText(WORD, m_font, FONT_SIZE);

        wordText.setColor(
            ((COMMON_ORDER == 0) ? m_uniqueColors.colorAtRatio(FREQ_RATIO)
                                 : m_commonColors.colorAtRatio(COMMON_RATIO)));

        if (FLAGGED_WORDS.contains(WORD))
        {
            wordText.setColor(sf::Color::Red);
        }

        wordText.setPosition(0.0f, 0.0f);

        // make an sf::Text object for the count
        std::ostringstream ss;

        if (WILL_SHOW_COUNTS)
        {
            ss << FREQ;
        }

        sf::Text countText(ss.str(), m_font, FONT_SIZE_COUNT);
        countText.setColor(sf::Color::White);

        // extra vertical space to compensate for sfml's inability to know actual font height
        auto const EXTRA_HEIGHT{ static_cast<float>(FONT_SIZE) / 2.0f };

        // move the word over so it doesn't overlap with the count
        if (WILL_SHOW_COUNTS)
        {
            // center the count text to the left of the wordText
            auto const COUNT_TEXT_VERT_POS{ ((wordText.getGlobalBounds().height * 0.5f) -
                                             (countText.getGlobalBounds().height * 0.5f)) +
                                            EXTRA_HEIGHT };

            countText.setPosition(0.0f, COUNT_TEXT_VERT_POS);

            auto const COUNT_TO_WORD_PAD{ 20.0f };
            wordText.move(countText.getGlobalBounds().width + COUNT_TO_WORD_PAD, EXTRA_HEIGHT);
        }

        const sf::Vector2f SIZE{
            wordText.getGlobalBounds().left + wordText.getGlobalBounds().width + EXTRA_HEIGHT,
            std::max(wordText.getGlobalBounds().height, countText.getGlobalBounds().height) +
                (1.5f * EXTRA_HEIGHT)
        };

        auto const POS{ findNextJumblePosition(ARGS, SIZE, rects) };

        if ((POS.x < 0.0f) || (POS.y < 0.0f))
        {
            return false;
        }
        else
        {
            if (WILL_SHOW_COUNTS)
            {
                countText.move(POS);
                m_texts.push_back(countText);
            }

            wordText.move(POS);
            m_texts.push_back(wordText);

            rects.push_back(sf::FloatRect(POS, SIZE));
            return true;
        }
    }

    const sf::Vector2f ParseDisplayer::findNextJumblePosition(
        const ArgsParser & ARGS, const sf::Vector2f & SIZE, const RectVec_t & INVALID_RECTS) const
    {
        auto const INTERVAL{ 20.0f };

        auto posLeft{ 0.0f };
        while ((posLeft + SIZE.x) < ARGS.screenWidthF())
        {
            auto posTop{ 0.0f };
            while ((posTop + SIZE.y) < (ARGS.screenHeightF() - 100.0f))
            {
                auto const SLIDE_DOWN_AMOUNT{ findNextLowestValidPosition(
                    sf::FloatRect(posLeft, posTop, SIZE.x, SIZE.y), INVALID_RECTS) };

                if (SLIDE_DOWN_AMOUNT < 1.0f)
                {
                    return sf::Vector2f(posLeft, posTop);
                }
                else
                {
                    posTop += SLIDE_DOWN_AMOUNT;
                }
            }
            posLeft += INTERVAL;
        }

        return sf::Vector2f(-1.0f, -1.0f);
    }

    float ParseDisplayer::findNextLowestValidPosition(
        const sf::FloatRect & TEST_RECT, const RectVec_t & INVALID_RECTS) const
    {
        auto const PAD{ 20.0f };

        auto const TEST_RECT_RIGHT{ TEST_RECT.left + TEST_RECT.width };
        auto const TEST_RECT_BOTTOM{ TEST_RECT.top + TEST_RECT.height };

        for (auto const & INVALID_RECT : INVALID_RECTS)
        {
            auto const INVALID_RECT_BOTTOM{ INVALID_RECT.top + INVALID_RECT.height };
            auto const INVALID_RECT_RIGHT{ INVALID_RECT.left + INVALID_RECT.width };

            if ((std::max(TEST_RECT.left, INVALID_RECT.left) <
                 std::min(TEST_RECT_RIGHT, INVALID_RECT_RIGHT)) &&
                std::max(TEST_RECT.top, INVALID_RECT.top) <
                    std::min(TEST_RECT_BOTTOM, INVALID_RECT_BOTTOM))
            {
                return PAD;
            }
        }

        return 0.0f;
    }

    void ParseDisplayer::loadFont(const ArgsParser & ARGS)
    {
        namespace fs = std::filesystem;

        auto const PATH{ fs::canonical(ARGS.fontPath()) };
        auto const PATH_STR{ PATH.string() };

        M_LOG_AND_ASSERT_OR_THROW(
            (fs::exists(PATH)),
            "ParseDisplayer::LoadFont(\"" << PATH_STR
                                          << "\") failed because that file does not exist.");

        M_LOG_AND_ASSERT_OR_THROW(
            (fs::is_regular_file(PATH)),
            "ParseDisplayer::LoadFont(\"" << PATH_STR
                                          << "\") failed because that is not a regular file.");

        M_LOG_AND_ASSERT_OR_THROW(
            (m_font.loadFromFile(PATH_STR.c_str())),
            "During ParseDisplayer::LoadFont() sf::Font::loadFromFile(\""
                << PATH_STR << "\") failed!  Check console output for information.");
    }

    void ParseDisplayer::setupColumn(
        const ArgsParser & ARGS,
        float & posLeft,
        const WordCountVec_t & WORDS,
        std::size_t & wordsIndex,
        const WordList & COMMON_WORDS,
        const WordList & FLAGGED_WORDS,
        const bool WILL_SHOW_COUNTS,
        const unsigned FONT_SIZE_MAX)
    {
        const unsigned FONT_SIZE_COUNT{ 30 };
        const unsigned FONT_SIZE_MIN{ std::min(ARGS.fontSizeMin(), (FONT_SIZE_MAX - 1)) };

        auto const WIDTH{ ARGS.screenWidthF() };
        auto const HEIGHT{ ARGS.screenHeightF() };
        auto const COUNT_TO_WORD_PAD{ 20.0f };
        auto const FREQ_MIN{ WORDS[WORDS.size() - 1].count() };
        auto const COMMON_WORDS_COUNT_F{ static_cast<float>(COMMON_WORDS.count()) };
        auto const HIGHEST_WORD_FREQ_F{ static_cast<float>(WORDS[0].count()) };

        auto maxHorizExtent{ 0.0f };
        auto vertPos{ 0.0f };

        while (wordsIndex < WORDS.size())
        {
            auto const FREQ{ WORDS[wordsIndex].count() };

            auto const FREQ_RATIO{ (
                ((WORDS.size() == 1) || ((WORDS[0].count() - FREQ_MIN) == 0))
                    ? 1.0f
                    : (static_cast<float>(FREQ - FREQ_MIN) /
                       static_cast<float>(HIGHEST_WORD_FREQ_F - static_cast<float>(FREQ_MIN)))) };

            auto const WORD{ WORDS[wordsIndex].word() };

            auto const COMMON_ORDER{ COMMON_WORDS.order(WORD) };

            auto const COMMON_RATIO{ (COMMON_WORDS_COUNT_F - static_cast<float>(COMMON_ORDER)) /
                                     COMMON_WORDS_COUNT_F };

            // make an sf::Text object for the word
            auto const FONT_SIZE{
                FONT_SIZE_MIN + static_cast<unsigned>(
                                    static_cast<float>(FONT_SIZE_MAX - FONT_SIZE_MIN) * FREQ_RATIO)
            };

            sf::Text wordText(WORD, m_font, FONT_SIZE);

            const sf::Vector2f LOCAL_POS{ wordText.getLocalBounds().left,
                                          wordText.getLocalBounds().top };

            wordText.setOrigin(LOCAL_POS);

            wordText.setColor(
                ((COMMON_ORDER == 0) ? m_uniqueColors.colorAtRatio(FREQ_RATIO)
                                     : m_commonColors.colorAtRatio(COMMON_RATIO)));

            if (FLAGGED_WORDS.contains(WORD))
            {
                wordText.setColor(sf::Color::Red);
            }

            wordText.setPosition(posLeft, vertPos);

            // Adjust the vertPos down to compensate for SFML's inability
            // to actually know the height of text.
            auto const VERT_SHIFT{ static_cast<float>(FONT_SIZE) / 3.0f };
            vertPos += VERT_SHIFT;

            // make an sf::Text object for the count
            std::ostringstream ss;

            if (WILL_SHOW_COUNTS)
            {
                ss << FREQ;
            }

            sf::Text countText(ss.str(), m_font, FONT_SIZE_COUNT);

            const sf::Vector2f COUNT_LOCAL_POS{ countText.getLocalBounds().left,
                                                countText.getLocalBounds().top };

            countText.setOrigin(COUNT_LOCAL_POS);

            countText.setColor(sf::Color::White);

            // center the count text to the left of the wordText
            auto const COUNT_TEXT_VERT_POS{ vertPos - VERT_SHIFT };

            countText.setPosition(posLeft, COUNT_TEXT_VERT_POS);

            // move the word over so it doesn't overlap with the count
            wordText.move(countText.getGlobalBounds().width + COUNT_TO_WORD_PAD, 0.0f);

            auto const HORIZ_EXTENT{ wordText.getGlobalBounds().left +
                                     wordText.getGlobalBounds().width };

            if (maxHorizExtent < HORIZ_EXTENT)
            {
                maxHorizExtent = HORIZ_EXTENT;
            }

            if (maxHorizExtent > WIDTH)
            {
                break;
            }

            auto const VERT_SPACE_CONSUMED{ std::max(
                countText.getGlobalBounds().height, wordText.getGlobalBounds().height) };

            vertPos += VERT_SPACE_CONSUMED;
            if (vertPos > HEIGHT)
            {
                break;
            }

            if (WILL_SHOW_COUNTS)
            {
                m_texts.push_back(countText);
            }

            m_texts.push_back(wordText);

            ++wordsIndex;
        }

        posLeft = maxHorizExtent;
    }

    void ParseDisplayer::logStatistics(ReportMaker & reportMaker, const FreqStats & STATS)
    {
        reportMaker.displayStatsStream() << '-';
        reportMaker.displayStatsStream() << "Displayed Unique Word Count\t=" << STATS.unique;
        reportMaker.displayStatsStream() << "Displayed Total Word Count\t=" << STATS.sum;
        reportMaker.displayStatsStream() << '-';
        reportMaker.displayStatsStream() << "Displayed Frequency Minimum\t=" << STATS.min;
        reportMaker.displayStatsStream() << "Displayed Frequency Average\t=" << STATS.average;
        reportMaker.displayStatsStream() << "Displayed Frequency Median\t=" << STATS.median;
        reportMaker.displayStatsStream() << "Displayed Frequency Maximum\t=" << STATS.max;
        reportMaker.displayStatsStream() << "Displayed Frequency StdDev\t=" << STATS.stddev;
        reportMaker.displayStatsStream() << '-';

        for (auto const & FREQ_STR : STATS.freqs)
        {
            reportMaker.displayStatsStream() << FREQ_STR;
        }
    }

    void ParseDisplayer::drawLineLengthGraph(
        const ArgsParser & ARGS,
        const FileParser & PARSED_FILE,
        sf::RenderTarget & target,
        sf::RenderStates states) const
    {
        auto const & LINELEN_MAP{ PARSED_FILE.lengthCountMap() };

        if (LINELEN_MAP.size() < 2)
        {
            return;
        }

        // determine line length min/max and line length count min/max/sum
        std::size_t lineLengthMin{ LINELEN_MAP.begin()->first };
        std::size_t lineLengthMax{ lineLengthMin };
        std::size_t lineCountSum{ 0 };
        std::size_t lineCountMin{ LINELEN_MAP.begin()->second };
        std::size_t lineCountMax{ lineCountMin };
        for (auto const & LENGTH_COUNT_PAIR : LINELEN_MAP)
        {
            lineCountSum += LENGTH_COUNT_PAIR.second;

            if (LENGTH_COUNT_PAIR.first < lineLengthMin)
            {
                lineLengthMin = LENGTH_COUNT_PAIR.first;
            }

            if (LENGTH_COUNT_PAIR.first > lineLengthMax)
            {
                lineLengthMax = LENGTH_COUNT_PAIR.first;
            }

            if (LENGTH_COUNT_PAIR.second < lineCountMin)
            {
                lineCountMin = LENGTH_COUNT_PAIR.second;
            }

            if (LENGTH_COUNT_PAIR.second > lineCountMax)
            {
                lineCountMax = LENGTH_COUNT_PAIR.second;
            }
        }

        std::ostringstream lineLengthMaxSS;
        lineLengthMaxSS << lineLengthMax;

        std::ostringstream lineLengthMinSS;
        lineLengthMinSS << lineLengthMin;

        std::ostringstream lineCountMaxSS;
        lineCountMaxSS << lineCountMax;

        std::ostringstream lineCountMinSS;
        lineCountMinSS << lineCountMin;

        auto const SPACER_RATIO{ 0.1f };

        auto const VERTICAL_SPACER{ ARGS.screenHeightF() * SPACER_RATIO };
        auto const GRAPH_HEIGHT{ ARGS.screenHeightF() - (VERTICAL_SPACER * 2.0f) };

        auto const HORIZONTAL_SPACER{ ARGS.screenWidthF() * SPACER_RATIO };
        auto const GRAPH_WIDTH{ ARGS.screenWidthF() - (HORIZONTAL_SPACER * 2.0f) };

        const unsigned LABEL_FONT_SIZE{ 30 };

        // top label
        sf::Text topLabelText(lineCountMaxSS.str(), m_font, LABEL_FONT_SIZE);
        topLabelText.setColor(m_uniqueColors.colorAtRatio(1.0f));
        topLabelText.setPosition(
            HORIZONTAL_SPACER, VERTICAL_SPACER - topLabelText.getGlobalBounds().height);

        target.draw(topLabelText, states);

        auto const PAD{ 10.0f };

        auto const GRAPH_LEFT{ HORIZONTAL_SPACER + topLabelText.getGlobalBounds().width + PAD };
        auto const GRAPH_TOP{ VERTICAL_SPACER };
        auto const GRAPH_BOTTOM{ GRAPH_TOP + GRAPH_HEIGHT };
        auto const GRAPH_RIGHT{ GRAPH_LEFT + GRAPH_WIDTH };

        // bottom label
        sf::Text bottomLabelText(lineCountMinSS.str(), m_font, LABEL_FONT_SIZE);
        bottomLabelText.setColor(m_uniqueColors.colorAtRatio(1.0f));
        bottomLabelText.setPosition(
            (GRAPH_LEFT - bottomLabelText.getGlobalBounds().width) - PAD,
            (GRAPH_BOTTOM - bottomLabelText.getGlobalBounds().height) - (PAD * 2.0f));

        target.draw(bottomLabelText, states);

        // left label
        sf::Text leftLabelText(lineLengthMinSS.str(), m_font, LABEL_FONT_SIZE);
        leftLabelText.setColor(m_uniqueColors.colorAtRatio(1.0f));
        leftLabelText.setPosition(GRAPH_LEFT, GRAPH_BOTTOM);
        target.draw(leftLabelText, states);

        // label for column 80
        sf::Text label80Text("80", m_font, LABEL_FONT_SIZE);
        label80Text.setColor(m_uniqueColors.colorAtRatio(1.0f));

        auto const GRAPH_DISTANCE_TO_80{
            (80.0f / static_cast<float>(lineLengthMax - lineLengthMin)) * GRAPH_WIDTH
        };

        label80Text.setPosition(
            ((GRAPH_LEFT + GRAPH_DISTANCE_TO_80) - label80Text.getGlobalBounds().width) + PAD,
            GRAPH_BOTTOM);

        target.draw(label80Text, states);

        // label for column 100
        sf::Text label100Text("100", m_font, LABEL_FONT_SIZE);
        label100Text.setColor(m_uniqueColors.colorAtRatio(1.0f));

        auto const GRAPH_DISTANCE_TO_100{
            (100.0f / static_cast<float>(lineLengthMax - lineLengthMin)) * GRAPH_WIDTH
        };

        label100Text.setPosition(
            ((GRAPH_LEFT + GRAPH_DISTANCE_TO_100) - label100Text.getGlobalBounds().width) + PAD,
            GRAPH_BOTTOM);

        target.draw(label100Text, states);

        // right label
        sf::Text rightLabelText(lineLengthMaxSS.str(), m_font, LABEL_FONT_SIZE);
        rightLabelText.setColor(m_uniqueColors.colorAtRatio(1.0f));
        rightLabelText.setPosition(
            GRAPH_RIGHT - rightLabelText.getGlobalBounds().width, GRAPH_BOTTOM);

        target.draw(rightLabelText, states);

        // esatblish graph bar widths
        auto const BETWEEN_RECT_PAD_RATIO{ 0.1f };

        auto const RAW_RECT_WIDTH{ (GRAPH_WIDTH / static_cast<float>(LINELEN_MAP.size())) *
                                   (1.0f - BETWEEN_RECT_PAD_RATIO) };

        auto const RECT_WIDTH{ ((RAW_RECT_WIDTH < 1.0f) ? 1.0f : RAW_RECT_WIDTH) };

        // garph limits lines
        std::vector<sf::Vertex> lineVerts;
        lineVerts.push_back(
            sf::Vertex(sf::Vector2f(GRAPH_LEFT, GRAPH_TOP), m_uniqueColors.colorAtRatio(1.0f)));

        lineVerts.push_back(
            sf::Vertex(sf::Vector2f(GRAPH_LEFT, GRAPH_BOTTOM), m_uniqueColors.colorAtRatio(0.5f)));

        lineVerts.push_back(
            sf::Vertex(sf::Vector2f(GRAPH_LEFT, GRAPH_BOTTOM), m_uniqueColors.colorAtRatio(1.0f)));

        lineVerts.push_back(sf::Vertex(
            sf::Vector2f(GRAPH_RIGHT + RECT_WIDTH, GRAPH_BOTTOM),
            m_uniqueColors.colorAtRatio(1.0f)));

        lineVerts.push_back(sf::Vertex(
            sf::Vector2f(GRAPH_LEFT + GRAPH_DISTANCE_TO_80, GRAPH_BOTTOM),
            m_uniqueColors.colorAtRatio(1.0f)));

        lineVerts.push_back(sf::Vertex(
            sf::Vector2f(GRAPH_LEFT + GRAPH_DISTANCE_TO_80, GRAPH_BOTTOM + PAD),
            m_uniqueColors.colorAtRatio(1.0f)));

        lineVerts.push_back(sf::Vertex(
            sf::Vector2f(GRAPH_LEFT + GRAPH_DISTANCE_TO_100, GRAPH_BOTTOM),
            m_uniqueColors.colorAtRatio(1.0f)));

        lineVerts.push_back(sf::Vertex(
            sf::Vector2f(GRAPH_LEFT + GRAPH_DISTANCE_TO_100, GRAPH_BOTTOM + PAD),
            m_uniqueColors.colorAtRatio(1.0f)));

        target.draw(&lineVerts[0], lineVerts.size(), sf::Lines, states);

        // graph rects
        std::vector<sf::Vertex> rectVerts;
        for (auto const & LENGTH_COUNT_PAIR : LINELEN_MAP)
        {
            auto const LINE_LENGTH_RATIO{ static_cast<float>(LENGTH_COUNT_PAIR.first) /
                                          static_cast<float>(lineLengthMax) };

            auto const LINE_COUNT_RATIO{ static_cast<float>(LENGTH_COUNT_PAIR.second) /
                                         static_cast<float>(lineCountMax) };

            auto const LEFT{ (GRAPH_LEFT + (GRAPH_WIDTH * LINE_LENGTH_RATIO)) };

            auto const RIGHT{ (LEFT + RECT_WIDTH) };
            auto const BOTTOM{ GRAPH_BOTTOM - 1.0f };
            auto const TOP{ (BOTTOM - (GRAPH_HEIGHT * LINE_COUNT_RATIO)) - 1.0f };

            auto const COLOR_TOP_RATIO{ LINE_COUNT_RATIO };
            auto const COLOR_TOP{ m_uniqueColors.colorAtRatio(COLOR_TOP_RATIO) };

            auto const COLOR_BOTTOM_RATIO{ 0.0f };
            auto const COLOR_BOTTOM{ m_uniqueColors.colorAtRatio(COLOR_BOTTOM_RATIO) };

            rectVerts.push_back(sf::Vertex(sf::Vector2f(LEFT, TOP), COLOR_TOP));
            rectVerts.push_back(sf::Vertex(sf::Vector2f(RIGHT, TOP), COLOR_TOP));
            rectVerts.push_back(sf::Vertex(sf::Vector2f(RIGHT, BOTTOM), COLOR_BOTTOM));
            rectVerts.push_back(sf::Vertex(sf::Vector2f(LEFT, BOTTOM), COLOR_BOTTOM));
        }

        target.draw(&rectVerts[0], rectVerts.size(), sf::Quads, states);
    }

} // namespace word_stacker
