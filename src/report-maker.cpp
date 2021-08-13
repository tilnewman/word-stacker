// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
        : m_argsSS()
        , m_errorsSS()
        , m_miscSS()
        , m_fileStatsSS()
        , m_displayStatsSS()
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
        std::cout << m_argsSS.str() << '\n'
                  << m_miscSS.str() << '\n'
                  << m_fileStatsSS.str() << '\n'
                  << m_displayStatsSS.str() << std::endl;
    }

    void ReportMaker::echoErrors() const { std::cout << m_errorsSS.str() << std::endl; }

    std::stringstream & ReportMaker::prepareStreamForAppend(std::stringstream & ss) const
    {
        if (ss.str().empty() == false)
        {
            ss << "\n";
        }

        ss << M_INDENT;
        return ss;
    }

} // namespace word_stacker
