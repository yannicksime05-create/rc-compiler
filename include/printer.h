#ifndef PRINTER_H
#define PRINTER_H

#include <iostream>
#include "ast.h"

using ANT = ASTNodeType;

class Printer {
    int nspace = 0;
    short tab_length = 4;

    void indent() {
        for(int i = 0; i < nspace; ++i) std::cout << " ";
    }

    void printExpression(const Expr *e);
    void print_literal_exprs(const Expr *e);
    void print_two_operands_exprs(const Expr *e);


    void printDeclaration(const Decl *d);


    void printStatement(const Stmt *s);


    void exprs_printer_helper(Expr *e);
    void stmts_printer_helper(Stmt *s);

public:
    void print(const Stmt *s) {
        indent(); std::cout << "Program: {\n";
        nspace += tab_length;
        indent(); std::cout << "body: [\n";
        nspace += tab_length;
        printStatement(s);
        nspace -= tab_length;
        indent(); std::cout << "]\n";
        nspace -= tab_length;
        indent(); std::cout << "}\n";
    }
};

#endif // PRINTERS_H
