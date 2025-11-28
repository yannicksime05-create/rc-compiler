#include "../include/parser.h"
#include <sstream>

Stmt *Parser::parseStatement() {
    switch(current().type) {
        case TT::LBRACE:
            return parse_compound_statement();
            break;
        case TT::KW_IF:
            return parse_if_statement();
            break;
        case TT::KW_FOR:
            return parse_for_statement();
            break;
        default:
            return parse_expression_statement();
            break;
    }

    return nullptr;
}

CompoundStmt *Parser::parse_compound_statement() {
    get();

    std::vector<Stmt *> s;
    while( !is(TT::RBRACE) && !is(TT::END_OF_FILE) )
        s.push_back(parseStatement());

    expect(TT::RBRACE, "Error: Expected '}'");
    return new CompoundStmt(s);
}

ExprStmt *Parser::parse_expression_statement() {
    Expr *e = parseExpression();
    if(!e) {
        std::cerr << "From: parse_expression_statement\nError: unable to create expression" << std::endl;
        return nullptr;
    }
    expect(TT::SEMICOLON, "Error: Expected ';'");
    return new ExprStmt(e);
}

IfStmt *Parser::parse_if_statement() {
    get();

    expect(TT::LPAREN, "Error: Expected '('");
//    std::cout << "pos = " << pos << std::endl;
    Expr *condition = parseExpression();
    if(!condition) {
        std::cout << "From: parse_if_statement\nError: unable to create desired expression" << std::endl;
        return nullptr;
    }
    expect(TT::RPAREN, "Error: Expected ')'");
//    std::cout << "pos = " << pos << std::endl;

    Stmt *then_stmt = parseStatement();
//    std::cout << "pos = " << pos << std::endl;
    if(!then_stmt) {
        return nullptr;
    }
    Stmt *else_stmt = nullptr;

    if( is(TT::KW_ELSE) ) {
        get();
        else_stmt = parseStatement();
    }


    return new IfStmt(condition, then_stmt, else_stmt);
}

ForStmt *Parser::parse_for_statement() {
    get();

    expect(TT::LPAREN, "Error: Expected '('");
    std::cout << "pos = " << pos << std::endl;
    Expr *condition = parseExpression();
    if(!condition) {
        std::cout << "From: parse_if_statement\nError: unable to create desired expression" << std::endl;
        return nullptr;
    }
    expect(TT::RPAREN, "Error: Expected ')'");
    std::cout << "pos = " << pos << std::endl;
}

Expr *Parser::parseExpression() {
    return parse_assignment();
}

Expr *Parser::parse_assignment() { //a = b
    Expr *target = parse_logical_or();
    if(!target) {
        std::cout << "target is null" << std::endl;
        return nullptr;
    }
    if( is_assignment_operator() ) {
//        Token op = get();
//        Expr *value = parse_assignment();

        return new AssignExpr(target, get().value, parse_assignment());
    }

    return target;
}

Expr *Parser::parse_logical_or() { //a || b
    Expr *left = parse_logical_and();
    while( is(TT::OR) ) {
//        Token op = get();
//        Expr *right = parse_logical_and();

        left = new BinaryExpr(left, get().value, parse_logical_and());
    }

    return left;
}

Expr *Parser::parse_logical_and() {
    Expr *left = parse_equality();
    while( is(TT::AND) ) {
//        Token op = get();
//        Expr *right = parse_equality();

        left = new BinaryExpr(left, get().value, parse_equality());
    }

    return left;
}

Expr *Parser::parse_equality() { //a == b, a != b
    Expr *left = parse_comparaison();
    while( is(TT::EQUAL) || is(TT::NOT_EQUAL) ) {
//        Token op = get();
//        Expr *right = parse_comparaison();

        left = new BinaryExpr(left, get().value, parse_comparaison());
    }

    return left;
}

Expr *Parser::parse_comparaison() {// a < b, a <= b, a > b, a >= b
    Expr *left = parse_term();
    while( is(TT::LESS) || is(TT::LESS_EQUAL) || is(TT::GREATER) || is(TT::GREATER_EQUAL) ) {
//        Token op = get();
//        Expr *right = parse_term();

        left = new BinaryExpr(left, get().value, parse_term());
    }

    return left;
}

Expr *Parser::parse_term() {//a + b, a - b
    Expr *left = parse_factor();
    while( is(TT::PLUS) || is(TT::MINUS) ) {
//        Token op = get();
//        Expr *right = parse_factor();

        left = new BinaryExpr(left, get().value, parse_factor());
    }

    return left;
}

Expr *Parser::parse_factor() {//a * b, a / b, a % b
    Expr *left = parse_unary();
    while( is(TT::STAR) || is(TT::SLASH) || is(TT::MOD) ) {
//        Token op = get();
//        Expr *right = parse_unary();

        left = new BinaryExpr(left, get().value, parse_unary());
    }

    return left;
}

Expr *Parser::parse_unary() {
    return parse_primary();
}

Expr *Parser::parse_primary() {
    if( is(TT::LPAREN) ) {
        get();
        Expr *e = parseExpression();
        expect(TT::RPAREN, "Error: Expected ')'");
        return e;
    }

    if( is(TT::IDENTIFIER) )    return parse_identifier();

    if( is_literal() )    return parse_literal();

    std::cerr << "Unknow primary expression, at line: " << current().loc.line << ", column: " << current().loc.col << std::endl;
    return nullptr;
}

Expr *Parser::parse_literal() {
    Token t = get();

    switch(t.type) {
        case TT::INTEGER:
            return new IntNumberExpr(std::stoi(t.value));
        case TT::FLOAT:
            return new DecimalNumberExpr(std::stod(t.value));
        case TT::STRING:
            return new StringExpr(t.value);
        default:
            break;
    }

    return nullptr;
}

Expr *Parser::parse_identifier() {
    return new IdentifierExpr(get());
}



//Decl *Parser::ParseDeclaration() {
//    return var_declaration();
//}

//VarDecl *Parser::var_declaration() {
//    std::string tn = get().value;
//    std::string n = get().value;
//    if(current().value == "=") {
//        get();
//        return new VarDecl(tn, n, binary_expression());
//    }
//    else return new VarDecl(tn, n, nullptr);
//}

//double Parser::interpretBinaryExpr(BinaryExpr *e) {
//    double l, r;
//
//    switch(e->left->type) {
//        case ASTNodeType::BINARY_EXPR_NODE: {
//            l = interpretBinaryExpr( static_cast<BinaryExpr *>(e->left) );
//            break;
//        }
//        case ASTNodeType::INT_LIT_NODE: {
//            auto *le = static_cast<IntNumberExpr *>(e->left);
//            l = le->value;
//            break;
//        }
//        case ASTNodeType::DECIMAL_LIT_NODE: {
//            auto *le = static_cast<DecimalNumberExpr *>(e->left);
//            l = le->value;
//            break;
//        }
//        default:
//            break;
//    }
//
//    switch(e->right->type) {
//        case ASTNodeType::BINARY_EXPR_NODE: {
//            r = interpretBinaryExpr( static_cast<BinaryExpr *>(e->right) );
//            break;
//        }
//        case ASTNodeType::INT_LIT_NODE: {
//            auto *re = static_cast<IntNumberExpr *>(e->right);
//            r = re->value;
//            break;
//        }
//        case ASTNodeType::DECIMAL_LIT_NODE: {
//            auto *re = static_cast<DecimalNumberExpr *>(e->right);
//            r = re->value;
//            break;
//        }
//        default:
//            break;
//    }
//
//    switch(e->op) {
//        case '+': return l + r;
//        case '-': return l - r;
//        case '*': return l * r;
//        case '/': return l / r;
//        default:
//            break;
//    }
//
//    return 0;
//}
