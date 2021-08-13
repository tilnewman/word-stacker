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
#ifndef WORDSTACKER_WORDCOUNT_HPP_INCLUDED
#define WORDSTACKER_WORDCOUNT_HPP_INCLUDED
//
// word-count.hpp
//
#include <string>
#include <vector>
#include <cstddef> //for std::size_t


namespace word_stacker
{

    //Responsible for wrapping a word and its frequency.
    class WordCount
    {
    public:
        explicit WordCount(std::string word = "", const std::size_t COUNT = 0);

        inline const std::string word() const       { return m_word; }
        inline std::size_t count() const            { return m_count; }
        inline std::size_t add(const std::size_t X) { return m_count + X; }

    private:
        std::size_t m_count;
        std::string m_word;
    };

    using WordCountVec_t = std::vector<WordCount>;

}

#endif //WORDSTACKER_WORDCOUNT_HPP_INCLUDED
