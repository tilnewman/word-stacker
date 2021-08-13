#ifndef WORDSTACKER_WORDLIST_HPP_INCLUDED
#define WORDSTACKER_WORDLIST_HPP_INCLUDED
//
// word-list.hpp
//
#include <cstddef> //for std::size_t
#include <map>
#include <string>
#include <vector>

namespace word_stacker
{

    using StrVec_t = std::vector<std::string>;

    // Responsible for parsing a word list text file and presenting an interface
    // for querying which words are contained and what is their order of appearance.
    class WordList
    {
      public:
        explicit WordList(const StrVec_t & FILE_PATHS);

        // Valid order numbers start at 1, so this returns zero if S was not found.
        std::size_t order(const std::string & S) const;

        inline bool contains(const std::string & S) const { return (order(S) != 0); }

        inline std::size_t count() const { return m_count; }

      private:
        std::size_t m_count;
        std::map<std::string, std::size_t> m_wordOrderMap;
    };

} // namespace word_stacker

#endif // WORDSTACKER_WORDLIST_HPP_INCLUDED
