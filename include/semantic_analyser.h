#ifndef SEMANTIC_ANALYSER_H
#define SEMANTIC_ANALYSER_H

#include "scope.h"
#include "ast.h"
#include "diagnostic.h"
#include "type_checker.h"
//#include "symbol.h"           //symbol.h already comes in with scope.h and ast.h

/**
*   Thrown to abandon the statement being analysed (see analyze_statement()). Use it when the node has no
*   meaningful result after the error (undefined identifier, operator on incompatible types, ...).
*   When the analysis can simply go on (redefinition, wrong argument type, ...) call error() instead.
*/
class SemanticError : public CompileError {
public:
    SemanticError(const SourceRange& r, const std::string& msg) : CompileError(r, msg) {}
};


class SemanticAnalyser : public Visitor {
    ScopeManager manager;
    TypeChecker checker;
    Program& program;
    DiagnosticEngine& diagnostic;

    /** What the function being analysed needs to know about its return statements. */
    bool is_function_scope = false;
    Symbol *current_function_symbol = nullptr;
    std::vector<ReturnStmt*> current_function_return_stmts;
    void check_fn_return_types(Type *t, const Token& fn_name);

    /** */
    bool unaryexpr_tiny_constant_folder(Expr *e, long long& v);

    void error(const SourceRange& r, const std::string& msg) {
        diagnostic.error(r, msg);
    }

    void warning(const SourceRange& r, const std::string& msg) {
        diagnostic.warning(r, msg);
    }

    void note(const SourceRange& r, const std::string& msg) {
        diagnostic.note(r, msg);
    }

    /** Analyses a statement; an error that abandons it is reported here and the next statement is analysed normally. */
    void analyze_statement(Stmt *s);

    /**
    *   Same as analyze_statement, but sets e->resolved_type to unknown with the reason contained in the thrown error.
    *   Used in nodes containing a list of expressions, and in check_stmts_conditions (so that a wrong condition doesn't throw out the whole statement)
    */
    void analyze_expression(Expr *e);

    /**
    *   Checks that 'value' (of type 'from') can be converted to 'to'.
    *   Throws if it can't, warns if the value may change (narrowing), unless it's a literal that fits.
    */
    void check_conversion(const Type *from, const Type *to, Expr *value);
    bool literal_fits(Expr *value, const Type *in);

    void check_stmts_condition(Expr *condition);


public:
    SemanticAnalyser(Program& p, DiagnosticEngine& d) : program(p), diagnostic(d) {}

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
    void visit(ContinueStmt& s) override;

};

#endif // SEMANTIC_ANALYSER_H
