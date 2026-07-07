// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the OpenColorIO Project.

#ifndef INCLUDED_OCIO_STRINGUTILS_H
#define INCLUDED_OCIO_STRINGUTILS_H

#include <algorithm>
#include <cctype>
#include <string>
#include <vector>

namespace StringUtils
{
using StringVec = std::vector<std::string>;

inline std::string Lower(const std::string & s)
{
    std::string out(s);
    std::transform(out.begin(), out.end(), out.begin(), [](unsigned char c) { return char(std::tolower(c)); });
    return out;
}

inline std::string Upper(const std::string & s)
{
    std::string out(s);
    std::transform(out.begin(), out.end(), out.begin(), [](unsigned char c) { return char(std::toupper(c)); });
    return out;
}

inline std::string & ReplaceInPlace(std::string & s, const std::string & from, const std::string & to)
{
    if (from.empty()) return s;
    size_t pos = 0;
    while ((pos = s.find(from, pos)) != std::string::npos)
    {
        s.replace(pos, from.size(), to);
        pos += to.size();
    }
    return s;
}

inline std::string LeftTrim(const std::string & s)
{
    size_t pos = 0;
    while (pos < s.size() && std::isspace(static_cast<unsigned char>(s[pos]))) ++pos;
    return s.substr(pos);
}

inline std::string RightTrim(const std::string & s)
{
    size_t pos = s.size();
    while (pos > 0 && std::isspace(static_cast<unsigned char>(s[pos - 1]))) --pos;
    return s.substr(0, pos);
}

inline std::string Trim(const std::string & s)
{
    return RightTrim(LeftTrim(s));
}

inline bool StartsWith(const std::string & s, const std::string & prefix)
{
    return s.rfind(prefix, 0) == 0;
}

inline bool StartsWith(const std::string & s, char prefix)
{
    return !s.empty() && s.front() == prefix;
}

inline bool EndsWith(const std::string & s, const std::string & suffix)
{
    return s.size() >= suffix.size() && s.compare(s.size() - suffix.size(), suffix.size(), suffix) == 0;
}

inline bool Compare(const std::string & a, const std::string & b)
{
    return a == b;
}

inline StringVec SplitByWhiteSpaces(const std::string & s)
{
    StringVec out;
    size_t i = 0;
    while (i < s.size())
    {
        while (i < s.size() && std::isspace(static_cast<unsigned char>(s[i]))) ++i;
        size_t start = i;
        while (i < s.size() && !std::isspace(static_cast<unsigned char>(s[i]))) ++i;
        if (start < i) out.push_back(s.substr(start, i - start));
    }
    return out;
}
} // namespace StringUtils

#endif
