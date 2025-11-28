#ifndef PRINTER_H
#define PRINTER_H

#include <iostream>
#include "ast.h"

using ANT = ASTNodeType;

class Printer {
    int nspace = 2;
    short tab_length = 2;

    void indent() {
        for(int i = 0; i < nspace; ++i) std::cout << " ";
    }

    void indent(short n) {
        for(int i = 0; i < n; ++i) std::cout << " ";
    }

    void printExpression(const Expr *e);
    void print_literal_exprs(const Expr *e);
    void print_two_operands_exprs(const Expr *e);
    void exprs_printer_helper(Expr *e);


//    void printStatement(const Stmt *s);


//    void printDeclaration(const Delc *d);

public:
    void print() {
    }

    void printStatement(const Stmt *s);
};

#endif // PRINTERS_H
