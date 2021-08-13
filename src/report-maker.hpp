#ifndef WORDSTACKER_REPORTMAKER_HPP_INCLUDED
#define WORDSTACKER_REPORTMAKER_HPP_INCLUDED
//
// report-maker.hpp
//
#include <sstream>
#include <string>

namespace word_stacker
{

    // Responsible for collecting status strings and then making a report file.
    class ReportMaker
    {
      public:
        ReportMaker();

        void make() const;
        void echo() const;
        void echoErrors() const;

        inline std::stringstream & argumentsStream() { return prepareStreamForAppend(m_argsSS); }
        inline std::stringstream & errorsStream() { return prepareStreamForAppend(m_errorsSS); }
        inline std::stringstream & miscStream() { return prepareStreamForAppend(m_miscSS); }
        inline std::stringstream & fileStatsStream()
        {
            return prepareStreamForAppend(m_fileStatsSS);
        }
        inline std::stringstream & displayStatsStream()
        {
            return prepareStreamForAppend(m_displayStatsSS);
        }

        inline void displayStatsStreamClear() { m_displayStatsSS.str(""); }

        inline std::size_t frequencyListLength() { return 10; }

      private:
        std::stringstream & prepareStreamForAppend(std::stringstream &) const;

        static const std::string M_INDENT;

        std::stringstream m_argsSS;
        std::stringstream m_errorsSS;
        std::stringstream m_miscSS;
        std::stringstream m_fileStatsSS;
        std::stringstream m_displayStatsSS;
    };
} // namespace word_stacker

#endif // WORDSTACKER_REPORTMAKER_HPP_INCLUDED
