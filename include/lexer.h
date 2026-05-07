#ifndef LEXER_H
#define LEXER_H

#include <iostream>
#include <vector>
#include <istream>
#include <map>
#include "token.h"

using TT = TokenType;

const std::map<std::string, TokenType> keywords = {
    {"any",         TT::KW_ANY},
    {"auto",        TT::KW_AUTO},
    {"bool",        TT::KW_BOOL},
    {"break",       TT::KW_BREAK},
    {"case",        TT::KW_CASE},
    {"char",        TT::KW_CHAR},
    {"const",       TT::KW_CONST},
    {"default",     TT::KW_DEFAULT},
    {"do",          TT::KW_DO},
    {"double",      TT::KW_DOUBLE},
    {"else",        TT::KW_ELSE},
    {"false",       TT::KW_FALSE},
    {"float",       TT::KW_FLOAT},
    {"for",         TT::KW_FOR},
    {"if",          TT::KW_IF},
    {"int",         TT::KW_INT},
    {"return",      TT::KW_RETURN},
    {"string",      TT::KW_STRING},
    {"switch",      TT::KW_SWITCH},
    {"true",        TT::KW_TRUE},
    {"while",       TT::KW_WHILE}
};



class Lexer {
    std::vector<Token> tokens;
    std::istream& input;
    int line, column;

    //current character in the file.
    char c;

    void advance() {
        if(c == '\n') {
            ++line; column = 1;
        }
        else    ++column;
    }

    TokenType get_word_type(const std::string& word) {
        auto it = keywords.find(word);
        return it != keywords.end() ? it->second : TokenType::IDENTIFIER;
    }

    /**
    *   Use this function when there's no doubt about character we're going to read
    *   (i.e we don't need to check it with advance()).
    */
    char get() {
        ++column;
        return input.get();
    }

    void error(Token& t, const std::string& s, const std::string& msg) {
        t.type = TT::UNKNOWN;
        t.value = s;
        std::cerr << msg << "\nToken: value = " << t.value
        << ", spans from: line " << t.start.line << ", col " << t.start.col
        << " to: line " << t.end.line << ", col " << t.end.col << ".\n" << std::endl;
    }

    void scan_binary_numbers(Token& t, std::string& s);
    void scan_octal_numbers(Token& t, std::string& s);
    void scan_hex_numbers(Token& t, std::string& s);
    void base10_or_float_numbers(Token& t, std::string& s);

    Token comments_or_div_operator();
    Token strings();
    Token keywords_and_identifiers();
    Token numbers();
    Token next_token();


public:
    Lexer(std::istream& input) : input(input), line(1), column(1) {}

    std::vector<Token>& lex() {
        Token t;
        do{
            t = next_token();
            print(t);
            if(t.type != TT::USELESS) tokens.push_back(t);
        }while( t.type != TT::END_OF_FILE );

        return tokens;
    }

    void print(Token& t) {
        std::cout << "token value = " << t.value << std::endl << std::endl;
    }

};


#endif //LEXER_H
