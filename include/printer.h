#ifndef PRINTER_H
#define PRINTER_H

#include <iostream>
#include "ast.h"
#include "scope.h"

using ANT = ASTNodeType;

class Printer : public Visitor {
    int nspace = 0;
    short tab_length = 4;

    void indent() {
        for(int i = 0; i < nspace; ++i) std::cout << " ";
    }


    void exprs_printer_helper(Expr *e);
    void stmts_printer_helper(Stmt *s);

    void print_type_specifier(TypeSpecifier& t, const char *msg) {
        indent(); std::cout << msg << t.type_name.value;
        for(int d : t.dimension) std::cout << "[" << d << "]";
        std::cout << ",\n";
    }

public:
    void print(Program& p) {
        visit(p);
    }

    void visit(Program& p) override;

    void visit(BoolExpr& e) override;
    void visit(IntNumberExpr& e) override;
    void visit(DecimalNumberExpr& e) override;
    void visit(StringExpr& e) override;
    void visit(ArrayLiteralExpr& e) override;
    void visit(IdentifierExpr& e) override;
    void visit(BinaryExpr& e) override;
    void visit(UnaryExpr& e) override;
    void visit(AssignmentExpr& e) override;
    void visit(ConditionalExpr& e) override;
    void visit(CallExpr& e) override;
    void visit(MemberAccessExpr& e) override;
    void visit(SubscriptExpr& e) override;
    void visit(SequenceExpr& e) override;

    void visit(VariableDecl& d) override;
    void visit(FunctionDecl& d) override;

    void visit(CompoundStmt& s) override;
    void visit(ExpressionStmt& s) override;
    void visit(DeclarationStmt& s) override;
    void visit(IfStmt& s) override;
    void visit(SwitchStmt& s) override;
    void visit(WhileStmt& s) override;
    void visit(DoWhileStmt& s) override;
    void visit(ForStmt& s) override;
    void visit(RangeForStmt& s) override;
    void visit(ReturnStmt& s) override;

};

//class ScopePrinter {
//
//public:
//    ScopePrinter() {
//
//    }
//};

#endif // PRINTER_H
