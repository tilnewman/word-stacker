#ifndef WORDSTACKER_PARSETYPEENUM_HPP_INCLUDED
#define WORDSTACKER_PARSETYPEENUM_HPP_INCLUDED
//
// parse-type-enum.hpp
//
#include <string>

namespace word_stacker
{

    struct ParseType
    {
        enum Enum
        {
            Text = 0,
            Code,
            Count
        };

        static const std::string toString(const Enum);
    };

} // namespace word_stacker

#endif // WORDSTACKER_PARSETYPEENUM_HPP_INCLUDED
