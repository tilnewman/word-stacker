// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "args-parser.hpp"
#include "assert-or-throw.hpp"
#include "file-parser.hpp"
#include "parse-displayer.hpp"
#include "report-maker.hpp"
#include "word-list.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <filesystem>

#include <cstddef> //for std::size_t
#include <cstdlib>

using namespace word_stacker;

void saveScreenshot(sf::RenderWindow &);

int main(int argc, char * argv[])
{

    sf::RenderWindow renderWin(
        sf::VideoMode::getDesktopMode(), "Word-Stacker", sf::Style::Fullscreen);

    const sf::VideoMode VIDEO_MODE(
        renderWin.getSize().x, renderWin.getSize().y, renderWin.getSettings().depthBits);

    ReportMaker reportMaker;

    ArgsParser args(VIDEO_MODE, reportMaker, static_cast<std::size_t>(argc), argv);

    WordList commonWords({ args.commonWordsPath() });

    if (commonWords.count() != 0)
    {
        reportMaker.miscStream() << "Loaded " << commonWords.count() << " common words";
    }

    WordList ignoredWords{ args.ignoredWordsPaths() };

    if (ignoredWords.count() != 0)
    {
        reportMaker.miscStream() << "Loaded " << ignoredWords.count() << " ignored words from "
                                 << args.ignoredWordsPaths().size() << " files";
    }

    WordList flaggedWords{ args.flaggedWordsPaths() };

    if (flaggedWords.count() != 0)
    {
        reportMaker.miscStream() << "Loaded " << flaggedWords.count() << " falgged words from "
                                 << args.flaggedWordsPaths().size() << " files";
    }

    FileParser parser(reportMaker, args, commonWords, ignoredWords, flaggedWords);

    ParseDisplayer displayer(args);

    auto willDisplayCounts{ true };
    auto willDisplayAsColumns{ true };
    auto willDisplayLineLengthGraph{ false };

    if (args.willSkipDisplay() == false)
    {
        displayer.setup(
            reportMaker,
            args,
            parser,
            commonWords,
            flaggedWords,
            willDisplayCounts,
            willDisplayAsColumns);
    }

    reportMaker.echoErrors();

    if (args.willVerbose())
    {
        reportMaker.echo();
    }

    if (args.willSkipDisplay())
    {
        return EXIT_SUCCESS;
    }

    renderWin.setFramerateLimit(30);

    while (renderWin.isOpen())
    {
        sf::Event event;
        while (renderWin.pollEvent(event))
        {
            if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Space))
            {
                willDisplayCounts = !willDisplayCounts;

                displayer.setup(
                    reportMaker,
                    args,
                    parser,
                    commonWords,
                    flaggedWords,
                    willDisplayCounts,
                    willDisplayAsColumns);
            }

            if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::R))
            {
                reportMaker.make();
            }

            if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Return))
            {
                willDisplayAsColumns = !willDisplayAsColumns;

                displayer.setup(
                    reportMaker,
                    args,
                    parser,
                    commonWords,
                    flaggedWords,
                    willDisplayCounts,
                    willDisplayAsColumns);
            }

            if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::L))
            {
                willDisplayLineLengthGraph = !willDisplayLineLengthGraph;

                displayer.setup(
                    reportMaker,
                    args,
                    parser,
                    commonWords,
                    flaggedWords,
                    willDisplayCounts,
                    willDisplayAsColumns,
                    willDisplayLineLengthGraph);
            }

            auto const IS_QUIT_KEY_PRESSED{ sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) ||
                                            sf::Keyboard::isKeyPressed(sf::Keyboard::F1) ||
                                            sf::Keyboard::isKeyPressed(sf::Keyboard::Delete) ||
                                            sf::Keyboard::isKeyPressed(sf::Keyboard::Q) ||
                                            sf::Keyboard::isKeyPressed(sf::Keyboard::E) ||
                                            sf::Keyboard::isKeyPressed(sf::Keyboard::C) };

            if (IS_QUIT_KEY_PRESSED || (event.type == sf::Event::Closed))
            {
                renderWin.close();
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            {
                saveScreenshot(renderWin);
            }
        }

        renderWin.clear(sf::Color::Black);

        sf::RenderStates states;
        displayer.draw(args, parser, renderWin, states);

        renderWin.display();
    }

    return EXIT_SUCCESS;
}

void saveScreenshot(sf::RenderWindow & renderWin)
{
    const sf::Vector2u WINDOW_SIZE{ renderWin.getSize() };

    sf::Texture texture;

    M_LOG_AND_ASSERT_OR_THROW(
        (texture.create(WINDOW_SIZE.x, WINDOW_SIZE.y)),
        "Unable to take screenshot.  sf::Texture::create(width=" << WINDOW_SIZE.x << ", height="
                                                                 << WINDOW_SIZE.y << ") failed.");

    texture.update(renderWin);

    auto const SCREENSHOT_IMAGE{ texture.copyToImage() };

    auto const FILENAME_BASE{ "screenshot" };
    auto const FILENAME_EXT{ ".png" };

    std::ostringstream ss;
    ss << FILENAME_BASE << FILENAME_EXT;

    namespace fs = std::filesystem;

    auto path{ fs::absolute(fs::current_path() / fs::path(ss.str())) };

    unsigned long long filenameNumber{ 0 };
    while (fs::exists(path))
    {
        ss.str("");
        ss << FILENAME_BASE << "-" << ++filenameNumber << FILENAME_EXT;
        path = fs::absolute(fs::current_path() / fs::path(ss.str()));
    }

    M_LOG_AND_ASSERT_OR_THROW(
        (SCREENSHOT_IMAGE.saveToFile(path.string())),
        "Unable to take screenshot.  sf::Image::saveToFile(\"" << path.string() << "\") failed.");
}
