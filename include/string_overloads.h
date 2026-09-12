#ifndef STRING_OVERLOADS
#define STRING_OVERLOADS

#include <vector>
#include <string>

//Overload for numeric values (it takes a double because this way I don't have to write an overload for each and every c++ numeric type).
inline std::string operator+(const std::string& s, double n) {
    return s + std::to_string(n);
}

//The trim method
inline std::string operator-(const std::string& s, const std::string& trim) {
    if(s.empty()) return "";

    if(trim.empty()) return s;

    size_t start = s.find_first_not_of(trim), end = s.find_last_not_of(trim);

    return s.substr(start, end - start + 1);
}

inline std::string operator*(const std::string& s, const int n) {
    if(s.empty() || n <= 0) return "";

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

inline std::vector<std::string> operator/(const std::string& s, const size_t chunk_length) {
    std::vector<std::string> tokens;

    if(s.empty() || chunk_length <= 0) return tokens;

    if(chunk_length >= s.size()) {
        tokens.push_back(s);
        return tokens;
    }

    size_t start = 0;
    while(start + chunk_length < s.size()) {
        tokens.push_back(s.substr(start, chunk_length));
        start += chunk_length;
    }

    tokens.push_back(s.substr(start));
    return tokens;
}

#endif // STRING_OVERLOADS
