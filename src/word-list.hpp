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
#ifndef WORDSTACKER_WORDLIST_HPP_INCLUDED
#define WORDSTACKER_WORDLIST_HPP_INCLUDED
//
// word-list.hpp
//
#include <map>
#include <string>
#include <vector>
#include <cstddef> //for std::size_t


namespace word_stacker
{

    using StrVec_t = std::vector<std::string>;


    //Responsible for parsing a word list text file and presenting an interface
    //for querying which words are contained and what is their order of appearance.
    class WordList
    {
    public:
        explicit WordList(const StrVec_t & FILE_PATHS);

        //Valid order numbers start at 1, so this returns zero if S was not found.
        std::size_t order(const std::string & S) const;

        inline bool contains(const std::string & S) const { return (order(S) != 0); }

        inline std::size_t count() const { return m_count; }

    private:
        std::size_t m_count;
        std::map<std::string, std::size_t> m_wordOrderMap;
    };

}

#endif //WORDSTACKER_WORDLIST_HPP_INCLUDED
