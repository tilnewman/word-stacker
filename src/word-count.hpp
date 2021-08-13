#ifndef WORDSTACKER_WORDCOUNT_HPP_INCLUDED
#define WORDSTACKER_WORDCOUNT_HPP_INCLUDED
//
// word-count.hpp
//
#include <cstddef> //for std::size_t
#include <string>
#include <vector>

namespace word_stacker
{

    // Responsible for wrapping a word and its frequency.
    class WordCount
    {
      public:
        explicit WordCount(std::string word = "", const std::size_t COUNT = 0);

        inline const std::string word() const { return m_word; }
        inline std::size_t count() const { return m_count; }
        inline std::size_t add(const std::size_t X) { return m_count + X; }

      private:
        std::size_t m_count;
        std::string m_word;
    };

    using WordCountVec_t = std::vector<WordCount>;

} // namespace word_stacker

#endif // WORDSTACKER_WORDCOUNT_HPP_INCLUDED
