#ifndef AST_H
#define AST_H

#include <iostream>
#include <string>

enum class ASTNodeType {

    INT_LIT_NODE, DECIMAL_LIT_NODE, STRING_LIT_NODE,

    //Expressions Nodes
    IDENTIFIER_EXPR_NODE, BINARY_EXPR_NODE, UNARY_EXP_NODE, ASSIGN_EXPR_NODE,

    //Declarations Nodes
    VAR_DECL_NODE

};


// Base node type — every node inherits from this
struct ASTNode {
    virtual ~ASTNode() = default;
};



// --- Expressions ---
struct Expr : ASTNode {
    ASTNodeType type;
    Expr(ASTNodeType t) : type(t) {}

    virtual void print() {
        std::cout << "base expr" << std::endl;
    }

//    virtual ~Expr();
};

struct IntNumberExpr : Expr {
    int value;

    IntNumberExpr(int v) : Expr(ASTNodeType::INT_LIT_NODE), value(v) {}


    void print() override {
        std::cout << "Int number literal: " << value << std::endl;
    }
};

struct DecimalNumberExpr : Expr {
    double value;

    DecimalNumberExpr(double v) : Expr(ASTNodeType::DECIMAL_LIT_NODE), value(v) {}


    void print() override {
        std::cout << "Decimal number literal: " << value << std::endl;
    }
};

struct StringExpr : Expr {
    std::string value;

    StringExpr(std::string& v) : Expr(ASTNodeType::STRING_LIT_NODE), value(std::move(v)) {}

    void print() override {}
};

struct IdentifierExpr : Expr {
    Token name;

    IdentifierExpr(const Token& n) : Expr(ASTNodeType::IDENTIFIER_EXPR_NODE), name(n) {}

    void print() override {
    }
};

struct UnaryExpr : Expr {
    bool prefix;            //true if ++a, false if a++
    std::string op;
    Expr *expr = nullptr;

    UnaryExpr(Expr *e, std::string& o, bool p = true) : Expr(ASTNodeType::UNARY_EXP_NODE), prefix(p), op(o), expr(e) {}

    void print() override {
    }

    ~UnaryExpr() {
        delete expr;
        expr = nullptr;
    }
};

struct BinaryExpr : Expr {
    Expr* left = nullptr;
    char op;
    Expr* right = nullptr;

    BinaryExpr(Expr* l, char o, Expr* r)
        : Expr(ASTNodeType::BINARY_EXPR_NODE), left(l), op(std::move(o)), right(r) {}


    void print() override {
        std::cout << "Binary expression" << std::endl;

        if(!left) {
            std::cout << "Left operand is null" << std::endl;
            return;
        }
        std::cout << "  -----Left: ";
        left->print();

        std::cout << "  -----op = " << op << std::endl;

        if(!right) {
            std::cout << "Right operand is null" << std::endl;
            return;
        }
        std::cout << "  -----Right: ";
        right->print();

        std::cout << std::endl;
    }


    ~BinaryExpr() {
        delete left;
        left = nullptr;
        delete right;
        right = nullptr;
    }
};

struct AssignExpr : Expr {
    Expr* target = nullptr;        // usually a VariableExpr
    std::string op;                // "=" or "+=", "-=", etc.
    Expr* value = nullptr;         // the right-hand side expression

    AssignExpr(Expr* t, const std::string& o, Expr* v) : Expr(ASTNodeType::ASSIGN_EXPR_NODE), target(t), op(o), value(v) {}

    void print() override {
//        std::cout << "AssignmentExpr (" << op << "):" << std::endl;
//        std::cout << "  Target -> ";
//        target->print();
//        std::cout << "  Value -> ";
//        value->print();
    }

    ~AssignExpr() {
        delete target;
        target = nullptr;
        delete value;
        value = nullptr;
    }
};










// --- Statements ---
//struct Stmt : ASTNode {};
//
//struct ExprStmt : Stmt {
//    Expr* expr;
//    ExprStmt(Expr* e) : expr(e) {}
//};
//
//struct VarDeclStmt : Stmt {
//    std::string name;
//    Expr* initializer;
//    VarDeclStmt(std::string n, Expr* init)
//        : name(std::move(n)), initializer(init) {}
//};










// ---- Declarations -----
struct Decl : ASTNode {
    ASTNodeType type;
    Decl(ASTNodeType t) : type(t) {}
};

struct VarDecl : Decl {
    std::string type_name;
    std::string name;
    Expr *initializer = nullptr;

    VarDecl(const std::string& t, const std::string& n, Expr *i) : Decl(ASTNodeType::VAR_DECL_NODE), type_name(t), name(n), initializer(i) {}

    ~VarDecl() {
        delete initializer;
        initializer = nullptr;
    }
};

#endif // AST_H
