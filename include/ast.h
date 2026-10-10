#ifndef AST_H
#define AST_H

#include <iostream>
#include <vector>
#include "ast_base.h"
#include "symbol.h"

inline void set_start_and_end(ASTNode *n, Location s, Location e) {
    if(!n) return;

    n->start = s;
    n->end = e;
}

/**
*   We need Stmt inside FunctionDecl, and Decl inside DeclarationStmt,
*   so we create all the roots here, so that we can nest them in any way we want.
*/

struct Expr : ASTNode {
    Type *resolved_type = nullptr;

    Expr(ASTNodeType t) { node_type = t; }

    virtual ~Expr() {
        delete resolved_type;
        resolved_type = nullptr;
    }
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

    void accept(Visitor& v) override;

    ~Program() {
        for(const Stmt *s : statements) {
            delete s;
            s = nullptr;
        }

        std::cout << "Cleaned up Program node...\n";
    }
};



// --- Expressions ---
struct BoolExpr : Expr {
    bool value;

    BoolExpr(const Token& loc) : Expr(ASTNodeType::BOOL_LIT_NODE), value(loc.type == TokenType::KW_TRUE) {
        set_start_and_end(this, loc.start, loc.end);
    }

    void accept(Visitor& v) override;

    ~BoolExpr() {
        std::cout << "Cleaning up BoolExpr node...\n";
    }
};

struct IntNumberExpr : Expr {
    Token raw_value;
    long long value;
    std::string suffix; //The analyser sets this so that we don't recompute it in the generator.

    // NOTE: value in only needed in array bounds check in the analyser, so setting it here is probably not necessary.
    IntNumberExpr(const Token& rv) : Expr(ASTNodeType::INT_LIT_NODE), raw_value(rv) {
        set_start_and_end(this, rv.start, rv.end);

        std::string prefix;
        int base = 10;

        prefix += rv.value[0];
        prefix += std::tolower(rv.value[1]);

        if(prefix == "0b")      base = 2;
        else if(prefix == "0o") base = 8;
        else if(prefix == "0x") base = 16;


        value = std::stoll(rv.value, nullptr, base);
    }

    void accept(Visitor& v) override;

    ~IntNumberExpr() {
        std::cout << "Cleaning up IntNumberExpr node...\n";
    }
};

struct DecimalNumberExpr : Expr {
    Token raw_value;
    double value;

    DecimalNumberExpr(const Token& rv) : Expr(ASTNodeType::DECIMAL_LIT_NODE), raw_value(rv), value( std::stod(rv.value) ) {
        set_start_and_end(this, rv.start, rv.end);
    }

    void accept(Visitor& v) override;

    ~DecimalNumberExpr() {
        std::cout << "Cleaning up DecimalNumberExpr node...\n";
    }
};

struct CharExpr : Expr {
    std::string value;   // raw content between the quotes — 1 char, or a 2-char escape like \n

    CharExpr(const Token& loc) : Expr(ASTNodeType::CHAR_LIT_NODE), value(loc.value) {
        set_start_and_end(this, loc.start, loc.end);
    }

    void accept(Visitor& v) override;

    ~CharExpr() {
        std::cout << "Cleaning up CharExpr node...\n";
    }
};

struct StringExpr : Expr {
    std::string value;

    StringExpr(const Token& loc) : Expr(ASTNodeType::STRING_LIT_NODE), value(loc.value) {
        set_start_and_end(this, loc.start, loc.end);
    }

    void accept(Visitor& v) override;

    ~StringExpr() {
        std::cout << "Cleaning up StringExpr node...\n";
    }
};

struct ArrayLiteralExpr : Expr {
    std::vector<Expr *> elements;

    ArrayLiteralExpr(const std::vector<Expr *>& e, const Token& op_bracket, const Token& cl_bracket) : Expr(ASTNodeType::ARRAY_LIT_NODE), elements(e) {
        set_start_and_end(this, op_bracket.start, cl_bracket.end);
    }

    void accept(Visitor& v) override;

    ~ArrayLiteralExpr() {
        for(Expr *e : elements) {
            delete e;
            e = nullptr;
        }

        std::cout << "Cleaning ArrayLiteralExpr node...\n";
    }
};

struct IdentifierExpr : Expr {
    Token name;
    //Non-owning, so never call delete on it.
    Symbol *symbol = nullptr;

    IdentifierExpr(const Token& n) : Expr(ASTNodeType::IDENTIFIER_EXPR_NODE), name(n) {
        set_start_and_end(this, n.start, n.end);
    }

    void accept(Visitor& v) override;

    ~IdentifierExpr() {
        std::cout << "Cleaning up IdentifierExpr node...\n";
    }
};

struct BinaryExpr : Expr {
    Expr* left = nullptr;
    Token op;
    Expr* right = nullptr;

    BinaryExpr(Expr* l, const Token& o, Expr* r) : Expr(ASTNodeType::BINARY_EXPR_NODE), left(l), op(o), right(r) {
        if(l && r) set_start_and_end(this, l->start, r->end);
    }

    void accept(Visitor& v) override;

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
    Token op;
    Expr *expr = nullptr;

    UnaryExpr(const Token& o, Expr *e, bool p = true) : Expr(ASTNodeType::UNARY_EXP_NODE), is_prefix(p), op(o), expr(e) {
        if(e) {
            if(p) set_start_and_end(this, o.start, e->end);
            else  set_start_and_end(this, e->start, o.end);
        }
        else      set_start_and_end(this, o.start, o.end);
    }

    void accept(Visitor& v) override;

    ~UnaryExpr() {
        delete expr;
        expr = nullptr;
        std::cout << "Cleaned up UnaryExpr node...\n";
    }
};

struct AssignmentExpr : Expr {
    Expr* target = nullptr;        // usually an IdentifierExpr
    Token op;                       // "=" or "+=", "-=", etc.
    Expr* value = nullptr;         // the right-hand side expression

    AssignmentExpr(Expr* t, const Token& o, Expr* v) : Expr(ASTNodeType::ASSIGNMENT_EXPR_NODE), target(t), op(o), value(v) {
        if(t && v) set_start_and_end(this, t->start, v->end);
    }

    void accept(Visitor& v) override;

    ~AssignmentExpr() {
        delete target;
        target = nullptr;
        delete value;
        value = nullptr;
        std::cout << "Cleaned up AssignmentExpr node...\n";
    }
};

struct ConditionalExpr : Expr {
    Expr *condition = nullptr;
    Expr *if_true = nullptr;
    Expr *if_false = nullptr;

    ConditionalExpr(Expr *c, Expr *t, Expr *f) : Expr(ASTNodeType::CONDITIONAL_EXPR_NODE), condition(c), if_true(t), if_false(f) {
        if(c && f) set_start_and_end(this, c->start, f->end);
    }

    void accept(Visitor& v) override;

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
    //Non-owning, so never call delete on it.
    Symbol *symbol = nullptr;

    CallExpr(Expr *c, const std::vector<Expr *>& args, const Token& cl_brace) : Expr(ASTNodeType::CALL_EXPR_NODE), callee(c), arguments(args) {
        if(c) set_start_and_end(this, c->start, cl_brace.end);
    }

    void accept(Visitor& v) override;

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

//a.method
struct MemberAccessExpr : Expr {
    Expr *object = nullptr;
    Token member;
    //Non-owning, so never call delete on it.
    Symbol *symbol = nullptr;

    MemberAccessExpr(Expr *obj, const Token& m) : Expr(ASTNodeType::MEMBER_ACCESS_EXPR_NODE), object(obj), member(m) {
        if(obj) set_start_and_end(this, obj->start, m.end);
    }

    void accept(Visitor& v) override;

    ~MemberAccessExpr() {
        delete object;
        object = nullptr;

        std::cout << "Cleaned up MemberAccessExpr node...\n";
    }
};

struct SubscriptExpr : Expr {
    Expr *object = nullptr;
    std::vector<Expr *> indices;
    //Non-owning, so never call delete on it.
    Symbol *symbol = nullptr;

    SubscriptExpr(Expr *obj, const std::vector<Expr *>& idx, const Token& cl_bracket) : Expr(ASTNodeType::SUBSCRIPT_EXPR_NODE), object(obj), indices(idx) {
        if(obj) set_start_and_end(this, obj->start, cl_bracket.end);
    }

    void accept(Visitor& v) override;

    ~SubscriptExpr() {
        delete object;
        object = nullptr;
        for(const Expr *i : indices) {
            delete i;
            i = nullptr;
        }

        std::cout << "Cleaned up SubscriptExpr node...\n";
    }
};

struct SequenceExpr : Expr {
    std::vector<Expr *> expressions;

    SequenceExpr(const std::vector<Expr *>& exprs) : Expr(ASTNodeType::SEQUENCE_EXPR_NODE), expressions(exprs) {}

    void accept(Visitor& v) override;

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
    std::vector<std::string> qualifiers;
    Token type_name;
    std::vector<int> dimension;     //when int[size][size]

    TypeSpecifier(const std::vector<std::string>& qlfs, const Token& t, const std::vector<int>& dims) : qualifiers(qlfs), type_name(t), dimension(dims) {}

};

struct VariableDeclarator {
    Token variable_name;
    Expr *initializer = nullptr;
    Symbol *symbol = nullptr;

    VariableDeclarator(const Token& n, Expr *i = nullptr) : variable_name(n), initializer(i) {}

    ~VariableDeclarator() {
        delete initializer;
        initializer = nullptr;
        delete symbol;
        symbol = nullptr;

        std::cout << "Cleaned up VariableDeclarator...\n";
    }
};

struct VariableDecl : Decl {
    TypeSpecifier declared_type;
    std::vector<VariableDeclarator *> declarations;

    VariableDecl(const TypeSpecifier& t, const std::vector<VariableDeclarator *>& decls, const Token& sc = {}) : Decl(ASTNodeType::VAR_DECL_NODE), declared_type(t), declarations(decls) {
        set_start_and_end(this, declared_type.type_name.start, sc.end);
    }

    void accept(Visitor& v) override;

    ~VariableDecl() {
        for(const VariableDeclarator *vd : declarations) {
            delete vd;
            vd = nullptr;
        }

        std::cout << "Cleaned up VariableDecl node...\n";
    }
};

/**
*   FunctionDecl needs to be aware of this, so that's why it's here instead of in the Stmts section.
*/
struct CompoundStmt : Stmt {
    std::vector<Stmt *> statements;

    CompoundStmt(const std::vector<Stmt *>& s, const Token& op_brace, const Token& cl_brace) : Stmt(ASTNodeType::COMP_STMT_NODE), statements(s) {
        set_start_and_end(this, op_brace.start, cl_brace.end);
    }

    void accept(Visitor& v) override;

    ~CompoundStmt() {
        for(const Stmt *s : statements) {
            delete s;
            s = nullptr;
        }

        std::cout << "Cleaned up CompoundStmt node...\n";
    }
};

struct Parameter {
    TypeSpecifier type_name;
    Token parameter_name;
    Expr *default_value = nullptr;
    Symbol *symbol = nullptr;

    Parameter(const TypeSpecifier& t, const Token& n, Expr *dv) : type_name(t), parameter_name(n), default_value(dv) {}

    ~Parameter() {
        delete default_value;
        default_value = nullptr;
        delete symbol;
        symbol = nullptr;

        std::cout << "Cleaning up Param...\n";
    }
};

struct FunctionPrototype {
    TypeSpecifier return_type;
    Token function_name;
    std::vector<Parameter *> parameters;

    FunctionPrototype(const TypeSpecifier& rt, const Token& n, const std::vector<Parameter *>& p) : return_type(rt), function_name(n), parameters(p) {}

    ~FunctionPrototype() {
        for(const Parameter *p : parameters) {
            delete p;
            p = nullptr;
        }

        std::cout << "Cleaned up FunctionPrototype...\n";
    }
};

// --- Function Declaration Node ---
struct FunctionDecl : Decl {
    FunctionPrototype *prototype = nullptr;
    CompoundStmt *body = nullptr;
    Symbol *symbol = nullptr;

    FunctionDecl(FunctionPrototype *proto, CompoundStmt *b) : Decl(ASTNodeType::FUNC_DECL_NODE), prototype(proto), body(b) {
        if(proto && b) set_start_and_end(this, proto->return_type.type_name.start, b->end);
    }

    void accept(Visitor& v) override;

    ~FunctionDecl() {
        delete prototype;
        prototype = nullptr;
        delete body;
        body = nullptr;
        delete symbol;
        symbol = nullptr;

        std::cout << "Cleaned up FunctionDecl...\n";
    }
};







// --- Statements ---
struct ExpressionStmt : Stmt {
    Expr *expression = nullptr;

    ExpressionStmt(Expr *e, const Token& sc) : Stmt(ASTNodeType::EXPR_STMT_NODE), expression(e) {
        if(e) set_start_and_end(this, e->start, sc.end);
        else  set_start_and_end(this, sc.start, sc.end);
    }

    void accept(Visitor& v) override;

    ~ExpressionStmt() {
        delete expression;
        expression = nullptr;

        std::cout << "Cleaned up ExpressionStmt node...\n";
    }
};

struct DeclarationStmt : Stmt {
    Decl *declaration = nullptr;

    DeclarationStmt(Decl *d) : Stmt(ASTNodeType::DECL_STMT_NODE), declaration(d) {
        set_start_and_end(this, declaration->start, declaration->end);
    }

    void accept(Visitor& v) override;

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

    IfStmt(const Token& kw, Expr *c, Stmt *t = nullptr, Stmt *e = nullptr) : Stmt(ASTNodeType::IF_STMT_NODE), condition(c), then_statement(t), else_statement(e) {
        if(!e) set_start_and_end(this, kw.start, t->end);
        else   set_start_and_end(this, kw.start, e->end);
    }

    void accept(Visitor& v) override;

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
    std::vector<Expr *> expressions;
    CompoundStmt *body = nullptr;

    CaseClause(const std::vector<Expr*>& e, CompoundStmt *b) : expressions(e), body(b) {}

    ~CaseClause() {
        for(Expr *e : expressions) {
            if(e) {
                delete e;
                e = nullptr;
            }
        }
        delete body;
        body = nullptr;

        std::cout << "Cleaned up CaseClause...\n";
    }
};

struct SwitchStmt : Stmt {
    Expr *pattern = nullptr;
    std::vector<CaseClause *> cases;

    SwitchStmt(const Token& kw, Expr *p, const std::vector<CaseClause *>& c, const Token& cl_brace) : Stmt(ASTNodeType::SWITCH_STMT_NODE), pattern(p), cases(c) {
        set_start_and_end(this, kw.start, cl_brace.end);
    }

    void accept(Visitor& v) override;

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

    WhileStmt(const Token& kw, Expr *c, Stmt *b) : Stmt(ASTNodeType::WHILE_STMT_NODE), condition(c), body(b) {
        if(c && b) set_start_and_end(this, kw.start, b->end);
    }

    void accept(Visitor& v) override;

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

    DoWhileStmt(const Token& kw, const Token& sc, Stmt *b, Expr *c = nullptr) : Stmt(ASTNodeType::DO_WHILE_STMT_NODE), body(b), condition(c) {
        if(b && c) set_start_and_end(this, kw.start, sc.end);
    }

    void accept(Visitor& v) override;

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

    ForStmt(const Token& kw, Stmt *init, Expr *c, Expr *incr, Stmt *b) : Stmt(ASTNodeType::FOR_STMT_NODE), initialization(init), condition(c), increment(incr), body(b) {
        if(b) set_start_and_end(this, kw.start, b->end);
    }

    void accept(Visitor& v) override;

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

struct RangeForStmt : Stmt {
    VariableDecl *item = nullptr;
    Expr *range_initializer = nullptr;
    Stmt *body = nullptr;

    RangeForStmt(VariableDecl *i, Expr *ri, Stmt *b)
        : Stmt(ASTNodeType::RANGE_FOR_STMT_NODE), item(i), range_initializer(ri), body(b) {}

    void accept(Visitor& v) override;

    ~RangeForStmt() {
        delete item;
        item = nullptr;
        delete range_initializer;
        range_initializer = nullptr;
        delete body;
        body = nullptr;

        std::cout << "Cleaned up RangeForStmt node...\n";
    }
};

struct ReturnStmt : Stmt {
    ExpressionStmt *expr_stmt = nullptr;

    ReturnStmt(const Token& kw, ExpressionStmt *s = nullptr) : Stmt(ASTNodeType::RETURN_STMT_NODE), expr_stmt(s) {
        set_start_and_end(this, kw.start, expr_stmt->end);
    }

    void accept(Visitor& v) override;

    ~ReturnStmt() {
        delete expr_stmt;
        expr_stmt = nullptr;

        std::cout << "Cleaned up ReturnStmt node...\n";
    }
};

struct PrintStmt : Stmt {
    std::vector<Expr*> expressions;

    //The cpp generator needs these. They're set by the semantic analyser.
    bool has_fmt = false;               //true when the first expressions is a string specifying the format.
    int nb_placeholders = 0;            //this should be equal to expressions.size() - 1 if has_fmt = true.

    PrintStmt(const Token& kw, const std::vector<Expr*>& exprs, const Token& sc) : Stmt(ASTNodeType::PRINT_STMT_NODE), expressions(exprs) {
        set_start_and_end(this, kw.start, sc.end);
    }

    void accept(Visitor& v) override;

    ~PrintStmt() {
        for(const Expr *e : expressions) {
            delete e;
            e = nullptr;
        }

        std::cout << "Cleaned up PrintStmt node...\n";
    }
};

struct BreakStmt : Stmt {
    BreakStmt(const Token& kw) : Stmt(ASTNodeType::BREAK_STMT_NODE) {
        set_start_and_end(this, kw.start, {kw.end.col + 1, kw.end.line});
    }

    void accept(Visitor& v) override;

    ~BreakStmt() {
        std::cout << "Cleaned up BreakStmt node...\n";
    }
};

struct ContinueStmt : Stmt {
    ContinueStmt(const Token& kw) : Stmt(ASTNodeType::CONTINUE_STMT_NODE) {
        set_start_and_end(this, kw.start, {kw.end.col + 1, kw.end.line});
    }

    void accept(Visitor& v) override;

    ~ContinueStmt() {
        std::cout << "Cleaned up ContinueStmt node...\n";
    }
};

class Visitor {
public:
    virtual void visit(Program& p) = 0;

    virtual void visit(BoolExpr& e) = 0;
    virtual void visit(IntNumberExpr& e) = 0;
    virtual void visit(DecimalNumberExpr& e) = 0;
    virtual void visit(CharExpr& e) = 0;
    virtual void visit(StringExpr& e) = 0;
    virtual void visit(ArrayLiteralExpr& e) = 0;
    virtual void visit(IdentifierExpr& e) = 0;
    virtual void visit(BinaryExpr& e) = 0;
    virtual void visit(UnaryExpr& e) = 0;
    virtual void visit(AssignmentExpr& e) = 0;
    virtual void visit(ConditionalExpr& e) = 0;
    virtual void visit(CallExpr& e) = 0;
    virtual void visit(MemberAccessExpr& e) = 0;
    virtual void visit(SubscriptExpr& e) = 0;
    virtual void visit(SequenceExpr& e) = 0;

    virtual void visit(VariableDecl& d) = 0;
    virtual void visit(FunctionDecl& d) = 0;

    virtual void visit(CompoundStmt& s) = 0;
    virtual void visit(ExpressionStmt& s) = 0;
    virtual void visit(DeclarationStmt& s) = 0;
    virtual void visit(IfStmt& s) = 0;
    virtual void visit(SwitchStmt& s) = 0;
    virtual void visit(WhileStmt& s) = 0;
    virtual void visit(DoWhileStmt& s) = 0;
    virtual void visit(ForStmt& s) = 0;
    virtual void visit(RangeForStmt& s) = 0;
    virtual void visit(ReturnStmt& s) = 0;
    virtual void visit(PrintStmt& s) = 0;
    virtual void visit(BreakStmt& s) = 0;
    virtual void visit(ContinueStmt& s) = 0;

};

#endif // AST_H
