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
#ifndef WORDSTACKER_REPORTMAKER_HPP_INCLUDED
#define WORDSTACKER_REPORTMAKER_HPP_INCLUDED
//
// report-maker.hpp
//
#include <string>
#include <sstream>


namespace word_stacker
{

    //Responsible for collecting status strings and then making a report file.
    class ReportMaker
    {
    public:
        ReportMaker();

        void make() const;
        void echo() const;
        void echoErrors() const;

        inline std::stringstream & argumentsStream()    { return prepareStreamForAppend(m_argsSS); }
        inline std::stringstream & errorsStream()       { return prepareStreamForAppend(m_errorsSS); }
        inline std::stringstream & miscStream()         { return prepareStreamForAppend(m_miscSS); }
        inline std::stringstream & fileStatsStream()    { return prepareStreamForAppend(m_fileStatsSS); }
        inline std::stringstream & displayStatsStream() { return prepareStreamForAppend(m_displayStatsSS); }
    
        inline void displayStatsStreamClear()           { m_displayStatsSS.str(""); }

        inline std::size_t frequencyListLength()        { return 10; }

    private:
        std::stringstream & prepareStreamForAppend(std::stringstream &) const;

        static const std::string M_INDENT;

        std::stringstream m_argsSS;
        std::stringstream m_errorsSS;
        std::stringstream m_miscSS;
        std::stringstream m_fileStatsSS;
        std::stringstream m_displayStatsSS;
    };
}

#endif //WORDSTACKER_REPORTMAKER_HPP_INCLUDED
