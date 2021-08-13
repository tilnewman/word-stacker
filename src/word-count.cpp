// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// word-count.cpp
//
#include "word-count.hpp"

namespace word_stacker
{

    WordCount::WordCount(std::string word, const std::size_t COUNT)
        : m_count(COUNT)
        , m_word(std::move(word))
    {}

} // namespace word_stacker
