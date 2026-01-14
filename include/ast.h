#ifndef AST_H
#define AST_H

#include <iostream>
#include <vector>
#include "token.h"

enum class ASTNodeType {
    PROGRAM,

    INT_LIT_NODE, DECIMAL_LIT_NODE, STRING_LIT_NODE,

    //Expressions Nodes
    IDENTIFIER_EXPR_NODE, BINARY_EXPR_NODE, UNARY_EXP_NODE, ASSIGNMENT_EXPR_NODE,
    CONDITIONAL_EXPR_NODE, CALL_EXPR_NODE, MEMBER_ACCESS_EXPR_NODE, SUBSCRIPT_EXPR_NODE,
    SEQUENCE_EXPR_NODE,

    //Statements Nodes
    COMP_STMT_NODE, EXPR_STMT_NODE, IF_STMT_NODE, SWITCH_STMT_NODE, WHILE_STMT_NODE,
    DO_WHILE_STMT_NODE, FOR_STMT_NODE, RETURN_STMT_NODE, DECL_STMT_NODE,

    //Declarations Nodes
    VAR_DECL_NODE, FUNC_DECL_NODE

};


// Base node type — every node inherits from this
struct ASTNode {
    ASTNodeType node_type;
    virtual ~ASTNode() = default;
};

/**
*   We need Stmt inside FunctionDecl, and Decl inside DeclarationStmt,
*   so we create all the roots here, so that we can nest them in any way we want.
*/

struct Expr : ASTNode {

    Expr(ASTNodeType t) { node_type = t; }

    virtual ~Expr() = default;
};

struct Decl : ASTNode {

    Decl(ASTNodeType t) { node_type = t; }

    virtual ~Decl() = default;
};

struct Stmt : ASTNode {

    Stmt(ASTNodeType t) { node_type = t; }

    virtual ~Stmt() = default;
};



struct Program : ASTNode {
    std::vector<Stmt *> statements;

    Program(const std::vector<Stmt *>& s) : statements(s) {
        node_type = ASTNodeType::PROGRAM;
    }

    ~Program() {
        for(const Stmt *s : statements) {
            delete s;
            s = nullptr;
        }

        std::cout << "Cleaned up Program node...\n";
    }
};



// --- Expressions ---
struct IntNumberExpr : Expr {
    int value;

    IntNumberExpr(int v) : Expr(ASTNodeType::INT_LIT_NODE), value(v) {}

    ~IntNumberExpr() {
        std::cout << "Cleaning up IntNumberExpr node...\n";
    }
};

struct DecimalNumberExpr : Expr {
    double value;

    DecimalNumberExpr(double v) : Expr(ASTNodeType::DECIMAL_LIT_NODE), value(v) {}

    ~DecimalNumberExpr() {
        std::cout << "Cleaning up DecimalNumberExpr node...\n";
    }
};

struct StringExpr : Expr {
    std::string value;

    StringExpr(std::string& v) : Expr(ASTNodeType::STRING_LIT_NODE), value(std::move(v)) {}

    ~StringExpr() {
        std::cout << "Cleaning up StringExpr node...\n";
    }
};

struct IdentifierExpr : Expr {
    Token name;

    IdentifierExpr(const Token& n) : Expr(ASTNodeType::IDENTIFIER_EXPR_NODE), name(n) {}

    ~IdentifierExpr() {
        std::cout << "Cleaning up IdentifierExpr node...\n";
    }
};

struct BinaryExpr : Expr {
    Expr* left = nullptr;
    std::string op;
    Expr* right = nullptr;

    BinaryExpr(Expr* l, const std::string& o, Expr* r)
        : Expr(ASTNodeType::BINARY_EXPR_NODE), left(l), op(std::move(o)), right(r) {}

    ~BinaryExpr() {
        delete left;
        left = nullptr;
        delete right;
        right = nullptr;
        std::cout << "Cleaned up BinaryExpr node...\n";
    }
};

struct UnaryExpr : Expr {
    bool is_prefix;            //true if ++a, false if a++
    std::string op;
    Expr *expr = nullptr;

    UnaryExpr(std::string& o, Expr *e, bool p = true) : Expr(ASTNodeType::UNARY_EXP_NODE), is_prefix(p), op(o), expr(e) {}

    ~UnaryExpr() {
        delete expr;
        expr = nullptr;
        std::cout << "Cleaned up UnaryExpr node...\n";
    }
};

struct AssignmentExpr : Expr {
    Expr* target = nullptr;        // usually a VariableExpr
    std::string op;                // "=" or "+=", "-=", etc.
    Expr* value = nullptr;         // the right-hand side expression

    AssignmentExpr(Expr* t, const std::string& o, Expr* v) : Expr(ASTNodeType::ASSIGNMENT_EXPR_NODE), target(t), op(o), value(v) {}

    ~AssignmentExpr() {
        delete target;
        target = nullptr;
        delete value;
        value = nullptr;
        std::cout << "Cleaned up AssignExpr node...\n";
    }
};

struct ConditionalExpr : Expr {
    Expr *condition = nullptr;
    Expr *if_true = nullptr;
    Expr *if_false = nullptr;

    ConditionalExpr(Expr *c, Expr *t, Expr *f)
        : Expr(ASTNodeType::CONDITIONAL_EXPR_NODE), condition(c), if_true(t), if_false(f) {}

    ~ConditionalExpr() {
        delete condition;
        condition = nullptr;
        delete if_true;
        if_true = nullptr;
        delete if_false;
        if_false = nullptr;

        std::cout << "Cleaned up ConditionalExpr node...\n";
    }
};

struct CallExpr : Expr {
    Expr *callee = nullptr;
    std::vector<Expr *> arguments;

    CallExpr(Expr *c, const std::vector<Expr *>& args = std::vector<Expr *>())
        : Expr(ASTNodeType::CALL_EXPR_NODE), callee(c), arguments(args) {}

    ~CallExpr() {
        delete callee;
        callee = nullptr;
        for(const Expr *e : arguments) {
            delete e;
            e = nullptr;
        }

        std::cout << "Cleaned up CallExpr node...\n";
    }
};

struct MemberAccessExpr : Expr {
    Expr *object = nullptr;
    std::string member;

    MemberAccessExpr(Expr *obj, const std::string& m) : Expr(ASTNodeType::MEMBER_ACCESS_EXPR_NODE), object(obj), member(m) {}

    ~MemberAccessExpr() {
        delete object;
        object = nullptr;

        std::cout << "Cleaned up MemberAccessExpr node...\n";
    }
};

struct SubscriptExpr : Expr {
    Expr *object = nullptr;
    Expr *index = nullptr;

    SubscriptExpr(Expr *o, Expr *i) : Expr(ASTNodeType::SUBSCRIPT_EXPR_NODE), object(o), index(i) {}

    ~SubscriptExpr() {
        delete object;
        object = nullptr;
        delete index;
        index = nullptr;

        std::cout << "Cleaned up SubscriptExpr node...\n";
    }
};

struct SequenceExpr : Expr {
    std::vector<Expr *> expressions;

    SequenceExpr(const std::vector<Expr *>& exprs) : Expr(ASTNodeType::SEQUENCE_EXPR_NODE), expressions(exprs) {}

    ~SequenceExpr() {
        for(const Expr *e : expressions) {
            delete e;
            e = nullptr;
        }

        std::cout << "Cleaned up SequenceExpr node...\n";
    }
};








// ---- Declarations -----
struct TypeSpecifier {
    std::string type_name;
    bool is_constant;

    TypeSpecifier() {}

    TypeSpecifier(const std::string& t, bool c) : type_name(t), is_constant(c) {}
};

struct VariableDeclarator {
    std::string variable_name;
    Expr *initializer = nullptr;

    VariableDeclarator(const std::string& n, Expr *i = nullptr) : variable_name(n), initializer(i) {}

    ~VariableDeclarator() {
        delete initializer;
        initializer = nullptr;

        std::cout << "Cleaned up VariableDeclarator...\n";
    }
};

struct VariableDecl : Decl {
    TypeSpecifier variable_type;
    std::vector<VariableDeclarator *> declarations;

    VariableDecl(const TypeSpecifier& t, const std::vector<VariableDeclarator *>& decls)
        : Decl(ASTNodeType::VAR_DECL_NODE), variable_type(t), declarations(decls) {}

    ~VariableDecl() {
        for(const VariableDeclarator *vd : declarations) {
            delete vd;
            vd = nullptr;
        }

        std::cout << "Cleaned up VariableDecl node...\n";
    }
};

struct Parameter {
    TypeSpecifier type_name;
    std::string parameter_name;
    Expr *default_value = nullptr;

    Parameter(const TypeSpecifier& t, const std::string& n, Expr *df = nullptr) : type_name(t), parameter_name(n), default_value(df) {}

    ~Parameter() {
        delete default_value;
        default_value = nullptr;

        std::cout << "Cleaning up Param...\n";
    }
};

// --- Function Declaration Node ---
struct FunctionDecl : Decl {
    TypeSpecifier return_type;
    std::string function_name;
    std::vector<Parameter *> parameters;
    Stmt *body = nullptr;

    FunctionDecl(const TypeSpecifier& rt, const std::string& n, Stmt *b, const std::vector<Parameter *>& p = std::vector<Parameter *>())
        : Decl(ASTNodeType::FUNC_DECL_NODE), return_type(rt), function_name(n), parameters(std::move(p)), body(std::move(b)) {}

    ~FunctionDecl() {
        delete body;
        body = nullptr;
        for(const Parameter *p : parameters) {
            delete p;
            p = nullptr;
        }

        std::cout << "Cleaned up FunctionDecl...\n";
    }
};







// --- Statements ---
struct CompoundStmt : Stmt {
    std::vector<Stmt *> statements;

    CompoundStmt(const std::vector<Stmt *>& s) : Stmt(ASTNodeType::COMP_STMT_NODE), statements(s) {}

    ~CompoundStmt() {
        for(const Stmt *s : statements) {
            delete s;
            s = nullptr;
        }

        std::cout << "Cleaned up CompoundStmt node...\n";
    }
};

struct ExpressionStmt : Stmt {
    Expr *expression = nullptr;

    ExpressionStmt(Expr *e) : Stmt(ASTNodeType::EXPR_STMT_NODE), expression(e) {}

    ~ExpressionStmt() {
        delete expression;
        expression = nullptr;

        std::cout << "Cleaned up ExpressionStmt node...\n";
    }
};

struct DeclarationStmt : Stmt {
    Decl *declaration = nullptr;

    DeclarationStmt(Decl *d) : Stmt(ASTNodeType::DECL_STMT_NODE), declaration(d) {}

    ~DeclarationStmt() {
        delete declaration;
        declaration = nullptr;

        std::cout << "Cleaned up DeclarationStmt node...\n";
    }
};

struct IfStmt : Stmt {
    Expr *condition = nullptr;
    Stmt *then_statement = nullptr;
    Stmt *else_statement = nullptr;

    IfStmt(Expr *c, Stmt *t = nullptr, Stmt *e = nullptr)
        : Stmt(ASTNodeType::IF_STMT_NODE), condition(c), then_statement(t), else_statement(e) {}

    ~IfStmt() {
        delete condition;
        condition = nullptr;
        delete then_statement;
        then_statement = nullptr;
        delete else_statement;
        else_statement = nullptr;

        std::cout << "Cleaned up IfStmt node...\n";
    }
};

struct CaseClause {
    Expr *expression = nullptr;
    CompoundStmt *body = nullptr;

    CaseClause(Expr *e, CompoundStmt *b) : expression(e), body(b) {}

    ~CaseClause() {
        delete expression;
        expression = nullptr;
        delete body;
        body = nullptr;

        std::cout << "Cleaned up CaseClause...\n";
    }
};

struct SwitchStmt : Stmt {
    Expr *pattern = nullptr;
    std::vector<CaseClause *> cases;

    SwitchStmt(Expr *p, const std::vector<CaseClause *>& c) : Stmt(ASTNodeType::SWITCH_STMT_NODE), pattern(p), cases(c) {}

    ~SwitchStmt() {
        delete pattern;
        pattern = nullptr;
        for(const CaseClause *c : cases) {
            delete c;
            c = nullptr;
        }

        std::cout << "Cleaned up SwitchStmt node...\n";
    }
};

struct WhileStmt : Stmt {
    Expr *condition = nullptr;
    Stmt *body = nullptr;

    WhileStmt(Expr *c, Stmt *b) : Stmt(ASTNodeType::WHILE_STMT_NODE), condition(c), body(b) {}

    ~WhileStmt() {
        delete condition;
        condition = nullptr;
        delete body;
        body = nullptr;

        std::cout << "Cleaned up WhileStmt node...\n";
    }
};

struct DoWhileStmt : Stmt {
    Stmt *body = nullptr;
    Expr *condition = nullptr;

    DoWhileStmt(Stmt *b, Expr *c = nullptr) : Stmt(ASTNodeType::DO_WHILE_STMT_NODE), body(b), condition(c) {}

    ~DoWhileStmt() {
        delete body;
        body = nullptr;
        delete condition;
        condition = nullptr;

        std::cout << "Cleaned up DoWhileStmt node...\n";
    }
};

struct ForStmt : Stmt {
    Stmt *initialization = nullptr;
    Expr *condition = nullptr;
    Expr *increment = nullptr;
    Stmt *body = nullptr;

    ForStmt(Stmt *init, Expr *c, Expr *incr, Stmt *b)
        : Stmt(ASTNodeType::FOR_STMT_NODE), initialization(init), condition(c), increment(incr), body(b) {}

    ~ForStmt() {
        delete initialization;
        initialization = nullptr;
        delete condition;
        condition = nullptr;
        delete increment;
        increment = nullptr;
        delete body;
        body = nullptr;

        std::cout << "Cleaned up ForStmt node...\n";
    }
};

struct ReturnStmt : Stmt {
    Expr *expression = nullptr;

    ReturnStmt(Expr *e = nullptr) : Stmt(ASTNodeType::RETURN_STMT_NODE), expression(e) {}

    ~ReturnStmt() {
        delete expression;
        expression = nullptr;

        std::cout << "Cleaned up ReturnStmt node...\n";
    }
};

#endif // AST_H
