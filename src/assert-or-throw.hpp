#ifndef WORDSTACKER_ASSERTORTHROW_HPP_INCLUDED
#define WORDSTACKER_ASSERTORTHROW_HPP_INCLUDED
//
// assert-or-throw.hpp
//
#include <cassert>
#include <exception>
#include <iostream>
#include <sstream>

namespace word_stacker
{

#if defined(NDEBUG)

#define M_LOG_AND_ASSERT_OR_THROW(exp, str_stream)                                         \
    {                                                                                      \
        {                                                                                  \
            if (false == (exp))                                                            \
            {                                                                              \
                std::ostringstream _m_oss_logandthrow_temp;                                \
                _m_oss_logandthrow_temp << "Assert Failed " << __FILE__ << ":" << __LINE__ \
                                        << " \"" << str_stream << "\"";                    \
                std::cerr << _m_oss_logandthrow_temp.str() << std::endl;                   \
                throw std::runtime_error(_m_oss_logandthrow_temp.str());                   \
            }                                                                              \
        }                                                                                  \
    }

#else

#define M_LOG_AND_ASSERT_OR_THROW(exp, str_stream)                                         \
    {                                                                                      \
        {                                                                                  \
            if (false == (exp))                                                            \
            {                                                                              \
                std::ostringstream _m_oss_logandthrow_temp;                                \
                _m_oss_logandthrow_temp << "Assert Failed " << __FILE__ << ":" << __LINE__ \
                                        << " \"" << str_stream << "\"";                    \
                std::cerr << _m_oss_logandthrow_temp.str() << std::endl;                   \
                assert((exp));                                                             \
            }                                                                              \
        }                                                                                  \
    }

#endif

} // namespace word_stacker

#endif // WORDSTACKER_ASSERTORTHROW_HPP_INCLUDED
