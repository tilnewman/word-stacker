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
// report-maker.cpp
//
#include "report-maker.hpp"

#include <boost/filesystem.hpp>

#include <fstream>
#include <iostream>


namespace word_stacker
{

    const std::string ReportMaker::M_INDENT{ "  " };


    ReportMaker::ReportMaker()
    :
        m_argsSS(),
        m_errorsSS(),
        m_miscSS(),
        m_fileStatsSS(),
        m_displayStatsSS()
    {}


    void ReportMaker::make() const
    {
        auto const FILENAME_BASE{ "report" };
        auto const FILENAME_EXT{ ".txt" };

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

        std::ofstream file;
        file.open(path.string());

        try
        {
            file << m_argsSS.str() << '\n'
                 << m_errorsSS.str() << '\n'
                 << m_miscSS.str() << '\n'
                 << m_fileStatsSS.str() << '\n'
                 << m_displayStatsSS.str() << std::endl;
        }
        catch (...)
        {
            file.close();
            throw;
        }

        file.close();
    }


    void ReportMaker::echo() const
    {
        std::cout
            << m_argsSS.str() << '\n'
            << m_miscSS.str() << '\n'
            << m_fileStatsSS.str() << '\n'
            << m_displayStatsSS.str() << std::endl;
    }


    void ReportMaker::echoErrors() const
    {
        std::cout << m_errorsSS.str() << std::endl;
    }


    std::stringstream & ReportMaker::prepareStreamForAppend(std::stringstream & ss) const
    {
        if (ss.str().empty() == false)
        {
            ss << "\n";
        }

        ss << M_INDENT;
        return ss;
    }

}
