#ifndef STRING_OVERLOADS
#define STRING_OVERLOADS

#include <vector>
#include <string>

//The trim method
inline std::string operator-(std::string& s, const std::string& trim) {
    if(s.empty()) return "";

    if(trim.empty()) return s;

    size_t start = s.find_first_not_of(trim);
    size_t end = s.find_last_not_of(trim);

    return s.substr(start, end);
}

inline std::string operator*(const std::string& s, const int n) {
    if(s.empty()) return "";

    if(n <= 0) return s;

    std::string result;
    result.reserve(s.size() * static_cast<size_t>(n));
    for(int i = 1; i <= n; ++i) result += s;

    return result;
}

//The split method
inline std::vector<std::string> operator/(const std::string& s, const std::string& delim) {
    std::vector<std::string> tokens;

    if(s.empty()) return tokens;

    if(delim.empty()) {
        tokens.push_back(s);
        return tokens;
    }

    size_t start = 0, pos = s.find(delim);
    while(pos != s.npos) {
        tokens.push_back(s.substr(start, pos - start));
        start = pos + delim.length();
        pos = s.find(delim, start);
    }

    tokens.push_back(s.substr(start));
    return tokens;
}

#endif // STRING_OVERLOADS
