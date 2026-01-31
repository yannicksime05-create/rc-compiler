#ifndef PRINTER_H
#define PRINTER_H

#include <iostream>
#include "ast.h"

using ANT = ASTNodeType;

class Printer : public Visitor {
    int nspace = 0;
    short tab_length = 4;

    void indent() {
        for(int i = 0; i < nspace; ++i) std::cout << " ";
    }

//    void printExpression(const Expr *expr);
//    void print_literal_exprs(const Expr *expr);
//    void print_two_operands_exprs(const Expr *expr);


//    void printDeclaration(const Decl *decl);
//
//
//    void printStatement(const Stmt *stmt);


    void exprs_printer_helper(Expr *e);
    void stmts_printer_helper(Stmt *s);

public:
    void print(const std::vector<Stmt *>& statements) {
        std::cout << "AST:" << std::endl;
        indent(); std::cout << "Program: {\n";
        nspace += tab_length;
        indent(); std::cout << "body: [\n";
        for(Stmt *s : statements) {
            stmts_printer_helper(s);
        }
        indent(); std::cout << "]\n";
        nspace -= tab_length;
        indent(); std::cout << "}\n";
    }

    void print(Program& p) {
        visit(p);
    }

    void visit(Program& p) override;

    void visit(IntNumberExpr& e) override;
    void visit(DecimalNumberExpr& e) override;
    void visit(StringExpr& e) override;
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

    void visit(CompoundStmt& c) override;
    void visit(ExpressionStmt& e) override;
    void visit(DeclarationStmt& d) override;
    void visit(IfStmt& i) override;
    void visit(SwitchStmt& s) override;
    void visit(WhileStmt& w) override;
    void visit(DoWhileStmt& dw) override;
    void visit(ForStmt& f) override;
    void visit(ReturnStmt& r) override;

};

#endif // PRINTERS_H
