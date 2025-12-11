#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum class TokenType {
    // ===== Special tokens =====
    END_OF_FILE,
    UNKNOWN,
    USELESS,            //comments

    // ===== Identifiers & keywords =====
    IDENTIFIER,

    // -- Keywords
    KW_ANY, KW_AUTO, KW_BOOL, KW_BREAK, KW_CASE, KW_CHAR, KW_CONST, KW_DEFAULT,
    KW_DO, KW_DOUBLE, KW_ELSE, KW_FLOAT, KW_FOR, KW_IF, KW_INT, KW_RETURN,
    KW_STRING, KW_SWITCH, KW_WHILE,

    // ===== Literals =====
    INTEGER,                                                             // 42, 0xFF, 010, 0b1010, etc.
    FLOAT,                                                              // 3.14, 1e-9
    //CHAR,                                                             // 'a'
    STRING,                                                             // "hello"

    // ===== Operators =====
    PLUS, MINUS, STAR, SLASH, MOD,                                      // + - * / %
    PLUS_PLUS, MINUS_MINUS,                                             // ++ --
    ASSIGN, EQUAL,                                                      // = ==
    PLUS_EQUAL, MINUS_EQUAL, STAR_EQUAL, SLASH_EQUAL, MOD_EQUAL,        //+= -= *= /= %=
    NOT, NOT_EQUAL,                                                     // ! !=
    GREATER, GREATER_EQUAL, LESS, LESS_EQUAL,                           // > >= < <=
    BIT_AND, AND, BIT_OR, OR,                                           // & && | ||
    CARET, TILDE,                                                       // ^ ~
    SHIFT_LEFT, SHIFT_RIGHT, SHIFT_LEFT_EQUAL, SHIFT_RIGHT_EQUAL,       // << >> <<= >>=
    QUESTION, COLON,                                                    // ? :
    DOT,
    //ARROW, DOT, DOT_STAR, ELLIPSIS,                                   // -> . .* ...

    // ===== Punctuation =====
    LPAREN, RPAREN,                                                     // ( )
    LBRACE, RBRACE,                                                     // { }
    LBRACKET, RBRACKET,                                                 // [ ]
    COMMA, SEMICOLON,                                                   // , ;
    HASH                                                                // #
};

struct Location {
    int col;
    int line;
};

struct Token {
    TokenType type;
    std::string value;
    Location loc;

    bool is(TokenType t) {
        return type == t;
    }

//    bool isLiteral() {
//        return type == TokenType::INTEGER || type == TokenType::FLOAT || type == TokenType::STRING;
//    }

    bool isArithmeticOperator() {
        return value == "+" || value == "-" || value == "*" || value == "/";
    }
};

#endif // TOKEN_H
