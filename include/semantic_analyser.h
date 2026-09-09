#ifndef SEMANTIC_ANALYSER_H
#define SEMANTIC_ANALYSER_H

#include <stdexcept>
#include "scope.h"
#include "ast.h"
#include "type_checker.h"
//#include "symbol.h"           //symbol.h already comes in with scope.h and ast.h


class SemanticError : public std::logic_error {

public:
    SemanticError(const std::string& error_msg) : std::logic_error(error_msg) {}

    const char *what() {
        return std::logic_error::what();
    }

};


class SemanticAnalyser : public Visitor {
    ScopeManager manager;
    TypeChecker checker;
    Program& program;

    /**
    */
    bool is_function_scope = false;
    Symbol *current_function_symbol = nullptr;
    std::vector<ReturnStmt*> current_function_return_stmts;
    void check_fn_return_types(Type *t, const Token& fn_name);

    int loop_depth = 0, switch_depth = 0;

    void warning(const std::string& msg) {
        std::cout << msg;
    }

    void error(const std::string& msg, bool _throw = true) {
        if(_throw) throw SemanticError(msg);

        std::cerr << msg;
    }

    void check_stmts_condition(Expr *condition, const Token& where);


public:
    SemanticAnalyser(Program& p) : program(p) {}

    /** Default destructor */
    ~SemanticAnalyser() = default;

    void analyze() {
        visit(program);
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

};

#endif // SEMANTIC_ANALYSER_H
