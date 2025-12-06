#ifndef AST_H
#define AST_H

#include <iostream>
#include <vector>
#include "token.h"

enum class ASTNodeType {

    INT_LIT_NODE, DECIMAL_LIT_NODE, STRING_LIT_NODE,

    //Expressions Nodes
    IDENTIFIER_EXPR_NODE, BINARY_EXPR_NODE, UNARY_EXP_NODE, ASSIGNMENT_EXPR_NODE,

    //Statements Nodes
    COMP_STMT_NODE, EXPR_STMT_NODE, IF_STMT_NODE, SWITCH_STMT_NODE, WHILE_STMT_NODE,
    DO_WHILE_STMT_NODE, FOR_STMT_NODE, RETURN_STMT_NODE, DECL_STMT_NODE,

    //Declarations Nodes
    VAR_DECL_NODE, FUNC_DECL_NODE

};


// Base node type — every node inherits from this
struct ASTNode {
    virtual void for_dynamic_polymorphism_purpose() = 0;
    virtual ~ASTNode() = default;
};

/**
*   We need Stmt inside FunctionDecl, and Decl inside DeclarationStmt,
*   so we create all the roots here, so that we can nest them in any way we want.
*/

struct Expr : ASTNode {
    ASTNodeType node_type;

    Expr(ASTNodeType t) : node_type(t) {}
};

struct Decl : ASTNode {
    ASTNodeType node_type;

    Decl(ASTNodeType t) : node_type(t) {}
};

struct Stmt : ASTNode {
    ASTNodeType node_type;

    Stmt(ASTNodeType t) : node_type(t) {}
};



// --- Expressions ---
struct IntNumberExpr : Expr {
    int value;

    IntNumberExpr(int v) : Expr(ASTNodeType::INT_LIT_NODE), value(v) {}

    //DO NOT REMOVE!!
    void for_dynamic_polymorphism_purpose() override {}

    ~IntNumberExpr() {
        std::cout << "Cleaning up IntNumberExpr node...\n";
    }
};

struct DecimalNumberExpr : Expr {
    double value;

    DecimalNumberExpr(double v) : Expr(ASTNodeType::DECIMAL_LIT_NODE), value(v) {}

    //DO NOT REMOVE!!
    void for_dynamic_polymorphism_purpose() override {}

    ~DecimalNumberExpr() {
        std::cout << "Cleaning up DecimalNumberExpr node...\n";
    }
};

struct StringExpr : Expr {
    std::string value;

    StringExpr(std::string& v) : Expr(ASTNodeType::STRING_LIT_NODE), value(std::move(v)) {}

    //DO NOT REMOVE!!
    void for_dynamic_polymorphism_purpose() override {}

    ~StringExpr() {
        std::cout << "Cleaning up StringExpr node...\n";
    }
};

struct IdentifierExpr : Expr {
    Token name;

    IdentifierExpr(const Token& n) : Expr(ASTNodeType::IDENTIFIER_EXPR_NODE), name(n) {}

    //DO NOT REMOVE!!
    void for_dynamic_polymorphism_purpose() override {}

    ~IdentifierExpr() {
        std::cout << "Cleaning up IdentifierExpr node...\n";
    }
};

struct UnaryExpr : Expr {
    bool prefix;            //true if ++a, false if a++
    std::string op;
    Expr *expr = nullptr;

    UnaryExpr(Expr *e, std::string& o, bool p = true) : Expr(ASTNodeType::UNARY_EXP_NODE), prefix(p), op(o), expr(e) {}

    //DO NOT REMOVE!!
    void for_dynamic_polymorphism_purpose() override {}

    ~UnaryExpr() {
        delete expr;
        expr = nullptr;
        std::cout << "Cleaned up UnaryExpr node...\n";
    }
};

struct BinaryExpr : Expr {
    Expr* left = nullptr;
    std::string op;
    Expr* right = nullptr;

    BinaryExpr(Expr* l, const std::string& o, Expr* r)
        : Expr(ASTNodeType::BINARY_EXPR_NODE), left(l), op(std::move(o)), right(r) {}


    //DO NOT REMOVE!!
    void for_dynamic_polymorphism_purpose() override {}


    ~BinaryExpr() {
        delete left;
        left = nullptr;
        delete right;
        right = nullptr;
        std::cout << "Cleaned up BinaryExpr node...\n";
    }
};

struct AssignmentExpr : Expr {
    Expr* target = nullptr;        // usually a VariableExpr
    std::string op;                // "=" or "+=", "-=", etc.
    Expr* value = nullptr;         // the right-hand side expression

    AssignmentExpr(Expr* t, const std::string& o, Expr* v) : Expr(ASTNodeType::ASSIGNMENT_EXPR_NODE), target(t), op(o), value(v) {}

    //DO NOT REMOVE!!
    void for_dynamic_polymorphism_purpose() override {}

    ~AssignmentExpr() {
        delete target;
        target = nullptr;
        delete value;
        value = nullptr;
        std::cout << "Cleaned up AssignExpr node...\n";
    }
};








// ---- Declarations -----
struct TypeSpecifier {
    std::string type_name;
    bool has_const;

    TypeSpecifier() {}

    TypeSpecifier(const std::string& t, bool c) : type_name(t), has_const(c) {}
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

    //DO NOT REMOVE!!
    void for_dynamic_polymorphism_purpose() override {}

    ~VariableDecl() {
        for(VariableDeclarator *vd : declarations) {
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

    //DO NOT REMOVE!!
    void for_dynamic_polymorphism_purpose() override {}

    ~FunctionDecl() {
        delete body;
        body = nullptr;

        std::cout << "Cleaned up FunctionDecl...\n";
    }
};







// --- Statements ---
struct CompoundStmt : Stmt {
    std::vector<Stmt *> statements;

    CompoundStmt(const std::vector<Stmt *>& s) : Stmt(ASTNodeType::COMP_STMT_NODE), statements(s) {}

    //DO NOT REMOVE!!
    void for_dynamic_polymorphism_purpose() override {}

    ~CompoundStmt() {
        for(Stmt *s : statements) {
            delete s;
            s = nullptr;
        }

        std::cout << "Cleaned up CompoundStmt node...\n";
    }
};

struct ExpressionStmt : Stmt {
    Expr *expression = nullptr;

    ExpressionStmt(Expr *e) : Stmt(ASTNodeType::EXPR_STMT_NODE), expression(e) {}

    //DO NOT REMOVE!!
    void for_dynamic_polymorphism_purpose() override {}

    ~ExpressionStmt() {
        delete expression;
        expression = nullptr;
        std::cout << "Cleaned up ExprStmt node...\n";
    }
};

struct DeclarationStmt : Stmt {
    Decl *declaration = nullptr;

    DeclarationStmt(Decl *d) : Stmt(ASTNodeType::DECL_STMT_NODE), declaration(d) {}

    //DO NOT REMOVE!!
    void for_dynamic_polymorphism_purpose() override {}

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

    IfStmt(Expr *c, Stmt *t, Stmt *e = nullptr)
        : Stmt(ASTNodeType::IF_STMT_NODE), condition(c), then_statement(t), else_statement(e) {}

    //DO NOT REMOVE!!
    void for_dynamic_polymorphism_purpose() override {}

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

    //DO NOT REMOVE!!
    void for_dynamic_polymorphism_purpose() override {}

    ~SwitchStmt() {
        delete pattern;
        pattern = nullptr;
        for(CaseClause *c : cases) {
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

    //DO NOT REMOVE!!
    void for_dynamic_polymorphism_purpose() override {}

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

    //DO NOT REMOVE!!
    void for_dynamic_polymorphism_purpose() override {}

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

    //DO NOT REMOVE!!
    void for_dynamic_polymorphism_purpose() override {}

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

    //DO NOT REMOVE!!
    void for_dynamic_polymorphism_purpose() override {}

    ~ReturnStmt() {
        delete expression;
        expression = nullptr;

        std::cout << "Cleaned up ReturnStmt node...\n";
    }
};

#endif // AST_H
