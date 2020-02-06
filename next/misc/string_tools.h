#ifndef SRC_STRING_TOOLS_H_
#define SRC_STRING_TOOLS_H_

#include <string>
#include <cstdio>
#include <cstdlib>

namespace toggl {

std::string trim_whitespace(const std::string &str)
{
    const std::string & whitespace = " \t";
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
        return ""; // no content

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

std::string to_string(const char *s) {
    if (!s) {
        return std::string("");
    }
#if defined(_WIN32) || defined(WIN32)
    std::wstring ws(s);
    std::string res("");
    Poco::UnicodeConverter::toUTF8(ws, res);
    return res;
#else
    return std::string(s);
#endif
}

const char *to_char_t(const std::string &s) {
#if defined(_WIN32) || defined(WIN32)
    static thread_local std::wstring ws;
    Poco::UnicodeConverter::toUTF16(s, ws);
    return ws.c_str();
#else
    return s.c_str();
#endif
}

char *copy_string(const std::string &s) {
#if defined(_WIN32) || defined(WIN32)
    std::wstring ws;
    Poco::UnicodeConverter::toUTF16(s, ws);
    return wcsdup(ws.c_str());
#else
    return strdup(s.c_str());
#endif
}

int compare_string(const char *s1, const char *s2) {
#if defined(_WIN32) || defined(WIN32)
    return wcscmp(s1, s2);
#else
    return strcmp(s1, s2);
#endif
}

} // namespace toggl

#endif SRC_STRING_TOOLS_H_
