#ifndef UTILZ_STRINGS_HPP_INCLUDED
#define UTILZ_STRINGS_HPP_INCLUDED
//
// strings.hpp
//
#include <algorithm>
#include <string>

namespace utilz
{

    static constexpr bool isUpper(const char CH) noexcept
    {
        return ((CH >= 'A') && (CH <= 'Z') && (CH != '\127'));
    }

    static constexpr bool isLower(const char CH) noexcept
    {
        return ((CH >= 'a') && (CH <= 'z') && (CH != '\127'));
    }

    static void toUpper(char & ch) noexcept
    {
        if (isLower(ch))
        {
            ch -= 32;
        }
    }

    [[nodiscard]] static char toUpperCopy(const char CH) noexcept
    {
        auto copy{ CH };
        toUpper(copy);
        return copy;
    }

    static void toUpper(std::string & str)
    {
        for (char & ch : str)
        {
            toUpper(ch);
        }
    }

    [[nodiscard]] static const std::string toUpperCopy(const std::string & STR)
    {
        auto copy{ STR };
        toUpper(copy);
        return copy;
    }

    static void toLower(char & ch) noexcept
    {
        if (isUpper(ch))
        {
            ch += 32;
        }
    }

    [[nodiscard]] static char toLowerCopy(const char CH) noexcept
    {
        auto copy{ CH };
        toLower(copy);
        return copy;
    }

    static void toLower(std::string & str)
    {
        for (char & ch : str)
        {
            toLower(ch);
        }
    }

    [[nodiscard]] static const std::string toLowerCopy(const std::string & STR)
    {
        auto copy{ STR };
        toLower(copy);
        return copy;
    }

    static constexpr bool isAlpha(const char CH) noexcept { return (isUpper(CH) || isLower(CH)); }

    static constexpr bool isDigit(const char CH) noexcept { return ((CH >= '0') && (CH <= '9')); }

    // technically ascii printable set excluding delete
    static constexpr bool isPrintable(const char CH) noexcept
    {
        return ((CH >= 32) && (CH <= 126) && (CH != '\127'));
    }

    static constexpr bool isWhitespace(const char CH) noexcept
    {
        return ((CH == ' ') || (CH == '\t') || (CH == '\r') || (CH == '\n'));
    }

    // includes typical whitespace and the printable ascii set excluding delete
    static constexpr bool isTypical(const char CH) noexcept
    {
        return (isWhitespace(CH) || isPrintable(CH));
    }

    static constexpr bool isWhitespaceOrNonTypical(const char CH) noexcept
    {
        return (isWhitespace(CH) || !isTypical(CH));
    }

    // trims any char(s) for which the lambda returns false
    template <typename Lambda_t>
    void trimIfNot(std::string & str, Lambda_t lambda)
    {
        str.erase(std::begin(str), std::find_if(std::begin(str), std::end(str), lambda));
        str.erase(std::find_if(std::rbegin(str), std::rend(str), lambda).base(), std::end(str));
    }

    static void trimWhitespace(std::string & str)
    {
        trimIfNot(str, [](const char CH) { return !isWhitespace(CH); });
    }

    [[nodiscard]] static const std::string trimWhitespaceCopy(const std::string & STR_ORIG)
    {
        std::string newStr{ STR_ORIG };
        trimWhitespace(newStr);
        return newStr;
    }

    static void trimNonTypical(std::string & str)
    {
        trimIfNot(str, [](const char CH) { return isTypical(CH); });
    }

    [[nodiscard]] static const std::string trimNonTypicalCopy(const std::string & STR_ORIG)
    {
        std::string newStr{ STR_ORIG };
        trimNonTypical(newStr);
        return newStr;
    }

    static void trimWhitespaceAndNonTypical(std::string & str)
    {
        trimIfNot(str, [](const char CH) { return !isWhitespaceOrNonTypical(CH); });
    }

    [[nodiscard]] static std::string trimWhitespaceAndNonTypicalCopy(const std::string & STR_ORIG)
    {
        std::string newStr{ STR_ORIG };
        trimWhitespaceAndNonTypical(newStr);
        return newStr;
    }

    static bool startsWith(const std::string & str, const std::string & with)
    {
        if (str.empty() || with.empty() || (with.size() > str.size()))
        {
            return false;
        }

        return (str.find(with) == 0);
    }

    static bool endsWith(const std::string & str, const std::string & with)
    {
        if (str.empty() || with.empty() || (with.size() > str.size()))
        {
            return false;
        }

        return (str.rfind(with) == (str.size() - with.size()));
    }

    static std::size_t replaceAll(std::string & inout, std::string_view what, std::string_view with)
    {
        if (inout.empty() || what.empty() || (what.size() > inout.size()))
        {
            return 0;
        }

        std::size_t count = 0;

        for (std::string::size_type pos{};
             inout.npos != (pos = inout.find(what.data(), pos, what.length()));
             pos += with.length(), ++count)
        {
            inout.replace(pos, what.length(), with.data(), with.length());
        }

        return count;
    }

    [[nodiscard]] static std::string
        replaceAllCopy(std::string_view str, std::string_view what, std::string_view with)
    {
        std::string copy{ str };
        replaceAll(copy, what, with);
        return copy;
    }

    static std::size_t removeAll(std::string & inout, std::string_view what)
    {
        return replaceAll(inout, what, "");
    }

    [[nodiscard]] static std::string removeAllCopy(std::string_view str, std::string_view what)
    {
        std::string copy{ str };
        removeAll(copy, what);
        return copy;
    }

} // namespace utilz

#endif // UTILZ_STRINGS_HPP_INCLUDED
