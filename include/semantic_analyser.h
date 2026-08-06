#ifndef SEMANTIC_ANALYSER_H
#define SEMANTIC_ANALYSER_H

#include <stdexcept>
#include "scope.h"
#include "ast.h"
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
    Program& program;

    bool is_builtin_type(const Type *t);
    bool is_integral_type(const BuiltinType *t);
    bool is_floating_type(const BuiltinType *t);
    bool is_numeric_type(const BuiltinType *t);
    bool is_string_type(const BuiltinType *t);
    bool is_bool_type(const BuiltinType *t);

    std::string type_to_string(const Type *t);
    std::string builtintype_to_string(const BuiltinType *t);
    std::string arraytype_to_string(const ArrayType *t);
    std::string autotype_to_string(const AutoType *t);
//    std::string functiontype_to_string(const FunctionType *t);

    BuiltinType *promote(const BuiltinType *left, const BuiltinType *right);
    std::string type_mismatch(const Type *lt, Token& op, const Type *rt);
    Type *resolve_type_name(Token& t);
    Type *resolve(TypeSpecifier& t);


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

#endif // SEMANTIC_ANALYSER_H
