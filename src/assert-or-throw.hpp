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
#ifndef WORDSTACKER_ASSERTORTHROW_HPP_INCLUDED
#define WORDSTACKER_ASSERTORTHROW_HPP_INCLUDED
//
// assert-or-throw.hpp
//
#include <cassert>
#include <iostream>
#include <sstream>
#include <exception>


namespace word_stacker
{

#if defined(NDEBUG)

#define M_LOG_AND_ASSERT_OR_THROW(exp, str_stream)                                        \
{                                                                                         \
    {                                                                                     \
        if (false == (exp))                                                               \
        {                                                                                 \
            std::ostringstream _m_oss_logandthrow_temp;                                   \
            _m_oss_logandthrow_temp << "Assert Failed "                                   \
                << __FILE__ << ":" << __LINE__                                            \
                << " \"" << str_stream << "\"";                                           \
            std::cerr << _m_oss_logandthrow_temp.str() << std::endl;                      \
            throw std::runtime_error( _m_oss_logandthrow_temp.str() );                    \
        }                                                                                 \
    }                                                                                     \
}                                                                                         \

#else

#define M_LOG_AND_ASSERT_OR_THROW(exp, str_stream)                                        \
{                                                                                         \
    {                                                                                     \
        if (false == (exp))                                                               \
        {                                                                                 \
            std::ostringstream _m_oss_logandthrow_temp;                                   \
            _m_oss_logandthrow_temp << "Assert Failed "                                   \
                << __FILE__ << ":" << __LINE__                                            \
                << " \"" << str_stream << "\"";                                           \
            std::cerr << _m_oss_logandthrow_temp.str() << std::endl;                      \
            assert( (exp) );                                                              \
        }                                                                                 \
    }                                                                                     \
}                                                                                         \

#endif

}

#endif //WORDSTACKER_ASSERTORTHROW_HPP_INCLUDED
