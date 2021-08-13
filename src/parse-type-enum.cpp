// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// parse-type-enum.cpp
//
#include "parse-type-enum.hpp"

#include <sstream>

namespace word_stacker
{

    const std::string ParseType ::toString(const ParseType::Enum E)
    {
        switch (E)
        {
            case Text:
            {
                return "Text";
            }
            case Code:
            {
                return "Code";
            }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "(invalid ParseType::Enum=" << E << ")";
                return ss.str();
            }
        }
    }

} // namespace word_stacker
