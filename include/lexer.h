#ifndef LEXER_H
#define LEXER_H

#include <iostream>
#include <vector>
#include <istream>
#include <map>
#include "token.h"

using TT = TokenType;

const std::map<std::string, TokenType> keywords = {
    {"any", TT::KW_ANY},
    {"auto", TT::KW_AUTO},
    {"bool", TT::KW_BOOL},
    {"break", TT::KW_BREAK},
    {"case", TT::KW_CASE},
    {"char", TT::KW_CHAR},
    {"const", TT::KW_CONST},
    {"default", TT::KW_DEFAULT},
    {"do", TT::KW_DO},
    {"double", TT::KW_DOUBLE},
    {"else", TT::KW_ELSE},
    {"float", TT::KW_FLOAT},
    {"for", TT::KW_FOR},
    {"if", TT::KW_IF},
    {"int", TT::KW_INT},
    {"return", TT::KW_RETURN},
    {"string", TT::KW_STRING},
    {"switch", TT::KW_SWITCH},
    {"while", TT::KW_WHILE}
};

//const std::pair<std::string, TokenType> keywords[] = {
//    {"bool", TokenType::KW_BOOL},
//    {"char", TokenType::KW_CHAR},
//    {"const", TokenType::KW_CONST},
//    {"do", TokenType::KW_DO},
//    {"double", TokenType::KW_DOUBLE},
//    {"else", TokenType::KW_ELSE},
//    {"float", TokenType::KW_FLOAT},
//    {"for", TokenType::KW_FOR},
//    {"if", TokenType::KW_IF},
//    {"int", TokenType::KW_INT}
//};




class Lexer {
    std::vector<Token> tokens;
    std::istream& input;
    int line, column;

    //Le caractère actuel dans le fichier.
    char c;

    void advance() {
        if(c == '\n') {
            ++line; column = 1;
        }
        else    ++column;
    }

    TokenType getWordType(const std::string& word) {
        auto it = keywords.find(word);
        return it != keywords.end() ? it->second : TokenType::IDENTIFIER;
    }

    Token handleCommentsOrSlash();
    Token handleStringLiterals();
    Token handleKeywordsAndIdentifiers();
    Token handleNumberLiterals();
    const Token getNextToken();


public:
    Lexer(std::istream& input) : input(input), line(1), column(1) {}

    std::vector<Token>& lex() {
        Token t;
        do{
            t = getNextToken();
            printToken(t);
            if(t.type != TT::USELESS) tokens.push_back(t);
        }while( t.type != TT::END_OF_FILE );

        return tokens;
    }

    void printToken(Token& t) {
        std::cout << "token value = " << t.value << std::endl << std::endl;
    }

};


#endif //LEXER_H
