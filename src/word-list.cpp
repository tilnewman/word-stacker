// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// word-list.cpp
//
#include "assert-or-throw.hpp"
#include "strings.hpp"
#include "word-list.hpp"

#include <boost/algorithm/algorithm.hpp>
#include <boost/algorithm/string.hpp>

#include <fstream>

namespace word_stacker
{

    WordList::WordList(const StrVec_t & FILE_PATHS)
        : m_count(0)
        , m_wordOrderMap()
    {
        for (auto const & FILE_PATH : FILE_PATHS)
        {
            if (FILE_PATH.empty())
            {
                continue;
            }

            std::ifstream file;
            file.open(FILE_PATH);

            M_LOG_AND_ASSERT_OR_THROW(
                (file.is_open()),
                "word_stacker::FileParser::ParseAsText(file_path=\""
                    << FILE_PATH << "\") failed to open that file.");

            try
            {
                std::size_t orderNum{ 0 };
                std::string word;
                while (file >> word)
                {
                    utilz::trimWhitespace(word);
                    utilz::replaceAll(word, ",", "");
                    utilz::replaceAll(word, "\n", "");
                    utilz::replaceAll(word, "\r", "");
                    utilz::replaceAll(word, "\t", "");
                    boost::to_lower(word);

                    if (word.empty() == false)
                    {
                        m_wordOrderMap[word] = ++orderNum;
                        ++m_count;
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
    }

    std::size_t WordList::order(const std::string & WORD) const
    {
        if (0 == m_count)
        {
            return 0;
        }
        else
        {
            auto const FOUND_ITER{ m_wordOrderMap.find(WORD) };

            if (FOUND_ITER == m_wordOrderMap.end())
            {
                return 0;
            }
            else
            {
                return FOUND_ITER->second;
            }
        }
    }

} // namespace word_stacker
