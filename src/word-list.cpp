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
// word-list.cpp
//
#include "word-list.hpp"
#include "assert-or-throw.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/algorithm.hpp>

#include <fstream>


namespace word_stacker
{

    WordList::WordList(const StrVec_t & FILE_PATHS)
    :
        m_count(0),
        m_wordOrderMap()
    {
        for (auto const & FILE_PATH : FILE_PATHS)
        {
            if (FILE_PATH.empty())
            {
                continue;
            }

            std::ifstream file;
            file.open(FILE_PATH);

            M_LOG_AND_ASSERT_OR_THROW((file.is_open()),
                "word_stacker::FileParser::ParseAsText(file_path=\"" << FILE_PATH
                << "\") failed to open that file.");

            try
            {
                std::size_t orderNum{ 0 };
                std::string word;
                while (file >> word)
                {
                    boost::trim(word);
                    boost::replace_all(word, ",", "");
                    boost::replace_all(word, "\n", "");
                    boost::replace_all(word, "\r", "");
                    boost::replace_all(word, "\t", "");
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

}
