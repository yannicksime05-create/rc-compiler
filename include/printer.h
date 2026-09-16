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


    void exprs_printer_helper(Expr *e) {
        nspace += tab_length;
        e->accept(*this);
        nspace -= tab_length;
    }

    void stmts_printer_helper(Stmt *s) {
        nspace += tab_length;
        s->accept(*this);
        nspace -= tab_length;
    }


    void print_type_specifier(const TypeSpecifier& t, const char *msg) {
        indent(); std::cout << msg << t.type_name.value;
        for(int d : t.dimension) std::cout << "[" << d << "]";
        std::cout << ",\n";
    }

    void print_function_prototype(const FunctionPrototype *proto) {
        indent(); std::cout << "name: " << proto->function_name.value << ",\n";
        indent(); std::cout << "qualifiers: ";
        if(proto->return_type.qualifiers.empty()) std::cout << "none,\n";
        else {
            for(const std::string& q : proto->return_type.qualifiers) std::cout << q << ",";
            std::cout << "\n";
        }
        print_type_specifier(proto->return_type, "return type: ");

        indent(); std::cout << "parameters: ";
        if(proto->parameters.empty()) std::cout << "[],\n";
        else {
            std::cout << "[\n";
            nspace += tab_length;
            for(const Parameter *p : proto->parameters) {
                indent(); std::cout << "identifier: {\n";
                nspace += tab_length;
                indent(); std::cout << "name: " << p->parameter_name.value<< ",\n";
                indent(); std::cout << "type: " << p->type_name.type_name.value << ",\n";
                indent(); std::cout << "const: ";
    //            p->type_name.is_constant ? (std::cout << "true,\n") : (std::cout << "false,\n");
    //            indent(); std::cout << "default value: ";
                if(p->default_value) {
                    std::cout << "{\n";
                    exprs_printer_helper(p->default_value);
                    indent(); std::cout << "}\n";
                }
                else std::cout << "null\n";
                nspace -= tab_length;
                indent(); std::cout << "},\n";
            }
            nspace -= tab_length;
            indent(); std::cout << "],\n";
        }
    }

public:
    void print(Program& p) {
        visit(p);
    }

    void visit(Program& p) override;

    void visit(BoolExpr& e) override;
    void visit(IntNumberExpr& e) override;
    void visit(DecimalNumberExpr& e) override;
    void visit(CharExpr& e) override;
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
    void visit(PrintStmt& s) override;
    void visit(BreakStmt& s) override;
    void visit(ContinueStmt& s) override;

};

#endif // PRINTER_H
