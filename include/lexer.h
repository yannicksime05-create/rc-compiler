#ifndef LEXER_H
#define LEXER_H

#include <iostream>
#include <vector>
#include <map>
#include "diagnostic.h"
#include "token.h"

using TT = TokenType;

const std::map<std::string, TokenType> keywords = {
    {"any",         TT::KW_ANY},
    {"auto",        TT::KW_AUTO},
    {"bool",        TT::KW_BOOL},
    {"break",       TT::KW_BREAK},
    {"byte",        TT::KW_BYTE},
    {"case",        TT::KW_CASE},
    {"char",        TT::KW_CHAR},
    {"const",       TT::KW_CONST},
    {"continue",    TT::KW_CONTINUE},
    {"default",     TT::KW_DEFAULT},
    {"do",          TT::KW_DO},
    {"double",      TT::KW_DOUBLE},
    {"else",        TT::KW_ELSE},
    {"false",       TT::KW_FALSE},
    {"float",       TT::KW_FLOAT},
    {"for",         TT::KW_FOR},
    {"if",          TT::KW_IF},
    {"int",         TT::KW_INT},
    {"long",        TT::KW_LONG},
    {"print",       TT::KW_PRINT},
//    {"quad",        TT::KW_QUAD},
    {"return",      TT::KW_RETURN},
    {"short",       TT::KW_SHORT},
    {"string",      TT::KW_STRING},
    {"switch",      TT::KW_SWITCH},
    {"true",        TT::KW_TRUE},
    {"ubyte",       TT::KW_UBYTE},
    {"uint",        TT::KW_UINT},
    {"ulong",       TT::KW_ULONG},
    {"ushort",      TT::KW_USHORT},
    {"void",        TT::KW_VOID},
    {"while",       TT::KW_WHILE}
};



class Lexer {
    std::vector<Token> tokens;
    const std::string& input;
    DiagnosticEngine& diagnostic;
    size_t pos = 0;
    bool hit_eof = false;

    //current character in the file.
    char c;

    TokenType get_word_type(const std::string& word) {
        auto it = keywords.find(word);
        return it != keywords.end() ? it->second : TokenType::IDENTIFIER;
    }

    char get() {
        if(pos >= input.size()) {
            hit_eof = true;
            return static_cast<char>(EOF);
        }

        return input[pos++];
    }

    /**
    *   @return the character at pos without incrementing it.
    */
    int peek() {
        if(pos >= input.size()) {
            hit_eof = true;
            return EOF;
        }

        return static_cast<unsigned char>(input[pos]);
    }

    bool eof() { return hit_eof; }

    void error(Token& t, const std::string& s, const std::string& msg) {
        t.type = TT::UNKNOWN;
        t.value = s;
        t.range.end = pos;

        diagnostic.error(t, msg);
    }

    bool scan_binary_numbers(Token& t, std::string& s);
    bool scan_octal_numbers(Token& t, std::string& s);
    bool scan_hex_numbers(Token& t, std::string& s);
    void scan_integral_part(Token& t, std::string& s);
    void scan_floating_point_part(Token& t, std::string& s);
    bool scan_integer_suffix(Token& t, std::string& s);
    bool scan_float_suffix(Token& t, std::string& s);

    Token comments_or_div_operator();
    Token strings();
    Token chars();
    Token keywords_and_identifiers();
    Token numbers();
    Token next_token();


public:
    Lexer(const std::string& source, DiagnosticEngine& d) : input(source), diagnostic(d) {}

    std::vector<Token>& lex() {
        Token t;
        do{
            t = next_token();
            if(t.type == TT::UNKNOWN || t.type == TT::USELESS) continue;
            tokens.push_back(t);
        }while( t.type != TT::END_OF_FILE );

        return tokens;
    }

    void print(Token& t) {
//        std::cout << "token value = " << t.value << ", offset begin: " << t.range.begin << ", offset end: " << t.range.end << " start at line: " << loc.line << ", col: " << loc.col << std::endl;
        std::cout << "token value = " << t.value << ", offset begin: " << t.range.begin << ", offset end: " << t.range.end << std::endl;
    }

};


#endif //LEXER_H
