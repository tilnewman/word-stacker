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
#include "args-parser.hpp"
#include "file-parser.hpp"
#include "parse-displayer.hpp"
#include "word-list.hpp"
#include "report-maker.hpp"
#include "assert-or-throw.hpp"

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <boost/filesystem.hpp>

#include <cstdlib>
#include <cstddef> //for std::size_t


using namespace word_stacker;


void saveScreenshot(sf::RenderWindow &);


int main(int argc, char * argv[])
{
    ReportMaker reportMaker;

    ArgsParser args(reportMaker, static_cast<std::size_t>(argc), argv);
    
    WordList commonWords( { args.commonWordsPath() } );

    if (commonWords.count() != 0)
    {
        reportMaker.miscStream() << "Loaded " << commonWords.count() << " common words";
    }

    WordList ignoredWords{ args.ignoredWordsPaths() };

    if (ignoredWords.count() != 0)
    {
        reportMaker.miscStream() << "Loaded " << ignoredWords.count()
            << " ignored words from " << args.ignoredWordsPaths().size() << " files";
    }

    WordList flaggedWords{ args.flaggedWordsPaths() };

    if (flaggedWords.count() != 0)
    {
        reportMaker.miscStream() << "Loaded " << flaggedWords.count()
            << " falgged words from " << args.flaggedWordsPaths().size() << " files";
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

    sf::RenderWindow renderWin(
        sf::VideoMode(args.screenWidthU(), args.screenHeightU(), args.bitsPerPixel()),
        "Word-Stacker",
        sf::Style::Fullscreen);

    renderWin.setFramerateLimit(30);

    while (renderWin.isOpen())
    {
        sf::Event event;
        while (renderWin.pollEvent(event))
        {
            if ((event.type == sf::Event::KeyReleased) &&
                (event.key.code == sf::Keyboard::Space))
            {
                willDisplayCounts = ! willDisplayCounts;

                displayer.setup(
                    reportMaker,
                    args,
                    parser,
                    commonWords,
                    flaggedWords,
                    willDisplayCounts,
                    willDisplayAsColumns);
            }

            if ((event.type == sf::Event::KeyReleased) &&
                (event.key.code == sf::Keyboard::R))
            {
                reportMaker.make();
            }

            if ((event.type == sf::Event::KeyReleased) &&
                (event.key.code == sf::Keyboard::Return))
            {
                willDisplayAsColumns = ! willDisplayAsColumns;

                displayer.setup(
                    reportMaker,
                    args,
                    parser,
                    commonWords,
                    flaggedWords,
                    willDisplayCounts,
                    willDisplayAsColumns);
            }

            if ((event.type == sf::Event::KeyReleased) &&
                (event.key.code == sf::Keyboard::L))
            {
                willDisplayLineLengthGraph = ! willDisplayLineLengthGraph;

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

            auto const IS_QUIT_KEY_PRESSED{
                sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) ||
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
                
    M_LOG_AND_ASSERT_OR_THROW((texture.create(WINDOW_SIZE.x, WINDOW_SIZE.y)),
        "Unable to take screenshot.  sf::Texture::create(width=" << WINDOW_SIZE.x
        << ", height=" << WINDOW_SIZE.y << ") failed.");

    texture.update(renderWin);

    auto const SCREENSHOT_IMAGE{ texture.copyToImage() };

    auto const FILENAME_BASE{ "screenshot" };
    auto const FILENAME_EXT{ ".png" };
    
    std::ostringstream ss;
    ss << FILENAME_BASE << FILENAME_EXT;

    namespace bfs = boost::filesystem;

    auto path{ bfs::system_complete(bfs::current_path() / bfs::path(ss.str())) };

    unsigned long long filenameNumber{ 0 };
    while (bfs::exists(path))
    {
        ss.str("");
        ss << FILENAME_BASE << "-" << ++filenameNumber << FILENAME_EXT;
        path = bfs::system_complete(bfs::current_path() / bfs::path(ss.str()));
    }

    M_LOG_AND_ASSERT_OR_THROW((SCREENSHOT_IMAGE.saveToFile(path.string())),
        "Unable to take screenshot.  sf::Image::saveToFile(\""
        << path.string() << "\") failed.");
}
