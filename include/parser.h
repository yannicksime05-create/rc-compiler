#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include "token.h"
#include "printers.h"

using TT = TokenType;

class Parser {
    std::vector<Token>& tokens;
    size_t pos;


    Token& get() {
        if(pos >= tokens.size()) return tokens.back();

        return tokens.at(pos++);
    }

    Token& current() { return tokens.at(pos); }

    Token& peek() {
        if(pos >= tokens.size()) return  tokens.back();

        return tokens[pos + 1];
    }

    Expr *literal_expression();

    Expr *ParseExpression();
    Expr *binary_expression();
        Expr *factor_expression();
        Expr *term_expression();
//    Stmt *ParseStatement();




//    Decl *ParseDeclaration();
    VarDecl *var_declaration();



public:
    Parser(std::vector<Token>& tokens) : tokens(tokens), pos(0) {}

    void parse() {
        UnaryExpr *e = nullptr;
        e = static_cast<UnaryExpr *>(factor_expression());
//        BinaryExpr *e = nullptr;
//        e = static_cast<BinaryExpr *>(binary_expression());
        if(!e) {
            std::cout << "e is null" << std::endl;
            return;
        }
        ExprPrinter::print(e);
        delete e;
        e = nullptr;

//        VarDecl *vd = var_declaration();
//
//        if(!vd) {
//            std::cout << "e is null" << std::endl;
//            return;
//        }
//        DeclPrinter::print(vd);
//        std::cout << "\n\nResult = " << interpretBinaryExpr(e) << std::endl;
//        delete vd;
//        vd = nullptr;
    }

    double interpretBinaryExpr(BinaryExpr *e);
//    double evalBinary(BinaryExpr *be);

};

#endif // PARSER_H
