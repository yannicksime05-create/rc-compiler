#include "../include/parser.h"
#include <sstream>

Expr *Parser::parseExpression() {
//    Expr *e = parse_assignment();
//    while( is(TT::COMMA) ) {
//        e = new CommaExpr();
//    }
//
//    return e;

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

        return new AssignmentExpr(target, get().value, parse_assignment());
    }

    return target;
}

Expr *Parser::parse_conditional() {
    Expr *condition = parse_logical_or();
    if( is(TT::QUESTION) ) {
        get();
        Expr *e = parseExpression();
        expect(TT::COLON, "Error: Expected ':' in conditional operator!");
        return new ConditionalExpr(condition, e, parse_conditional());
    }

    return condition;
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
    if( is(TT::MINUS) || is(TT::NOT) || is(TT::PLUS_PLUS) || is(TT::MINUS_MINUS) ) {
        return new UnaryExpr(get().value, parse_unary());
    }

    return parse_postfix();
}

Expr *Parser::parse_postfix() {
    Expr *primary = parse_primary();

    while(true) {
        if( is(TT::LPAREN) ) {
            get();
            if( is(TT::RPAREN) ) {
                get();
                primary = new CallExpr(primary);
            }
            else {
                std::vector<Expr *> args;
                args.push_back(parseExpression());
                while( is(TT::COMMA) ) {
                    get();
                    args.push_back(parseExpression());
                }
                expect(TT::RPAREN, "Error: Expected ')' after argument list");
                primary = new CallExpr(primary, args);
            }
        }
        else if( is(TT::LBRACKET) ) {
            get();
            Expr *index = parseExpression();
            expect(TT::RBRACKET, "Error: Expected closing ']' to complete subscript expression");
            primary = new SubscriptExpr(primary, index);
        }
        else if( is(TT::DOT) ) {
            get();
            expect(TT::IDENTIFIER, "Error: Expected identifier for member expressions");
            primary = new MemberAccessExpr(primary, previous().value);
        }
        else if( is(TT::PLUS_PLUS) || is(TT::MINUS_MINUS) ) {
            primary = new UnaryExpr(get().value, primary, false);
        }
        else break;
    }

    return primary;
}

Expr *Parser::parse_primary() {
    if( is(TT::LPAREN) ) {
        get();
        Expr *e = parseExpression();
        expect(TT::RPAREN, "Error: Expected closing ')' of primary expression");
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








Decl *Parser::parseDeclaration() {
    return parse_function_declaration();
}

TypeSpecifier *Parser::parse_type_specifier() {
    bool c = false;
    if( is(TT::KW_CONST) ) {
        get();
        c = true;
    }

    return new TypeSpecifier(get().value, c);
}

VariableDecl *Parser::parse_variable_declaration() {
//    std::cout << "current = " << current().value << std::endl;
    TypeSpecifier type = *parse_type_specifier();

    if( !is(TT::IDENTIFIER) ) {
        std::cout << "Expected identifier\n";
        return nullptr;
    }
    std::vector<VariableDeclarator *> decls;
    do{
        if( is(TT::COMMA) ) get();
        decls.push_back( parse_variable_declarator() );
    }while( is(TT::COMMA) );
    expect(TT::SEMICOLON, "Error: Expected ';'");

    return new VariableDecl(type, decls);
}

VariableDeclarator *Parser::parse_variable_declarator() {
    std::string name = get().value;
    if( is(TT::EQUAL) ) get();
    Expr *init = parseExpression();

    return new VariableDeclarator(name, init);
}

FunctionDecl *Parser::parse_function_declaration() {
    TypeSpecifier type = *parse_type_specifier();

    expect(TT::IDENTIFIER, "Error: Expected function's name");
    std::string name = previous().value;

    expect(TT::LPAREN, "Error: Expected '(' after function's name");
    if( !is(TT::RPAREN) ) {
        std::vector<Parameter *> parameters;
        parameters.push_back(parse_function_parameters());
        while( is(TT::COMMA) ) {
            get();
            parameters.push_back(parse_function_parameters());
        }
        expect(TT::RPAREN, "Error: Expected ')' after parameter list");

        Stmt *body = parseStatement();

        return new FunctionDecl(type, name, body, parameters);
    }
    expect(TT::RPAREN, "Error: Expected ')' after parameter list");

    Stmt *body = parseStatement();

    return new FunctionDecl(type, name, body);
}

Parameter *Parser::parse_function_parameters() {
    TypeSpecifier type = *parse_type_specifier();

    expect(TT::IDENTIFIER, "Error: Expected parameter's name in function declaration");
    std::string name = previous().value;

    if( is(TT::ASSIGN) ) {
        get();
        Expr *default_value = parseExpression();

        return new Parameter(type, name, default_value);
    }

    return new Parameter(type, name);
}








Stmt *Parser::parseStatement() {
    switch(current().type) {
        case TT::LBRACE:    return parse_compound_statement();
        case TT::KW_IF:     return parse_if_statement();
        case TT::KW_SWITCH: return parse_switch_statement();
        case TT::KW_WHILE:  return parse_while_statement();
        case TT::KW_DO:     return parse_do_while_statement();
        case TT::KW_FOR:    return parse_for_statement();
        case TT::KW_RETURN: return parse_return_statement();

        case TT::KW_CONST:
        case TT::KW_AUTO:
        case TT::KW_ANY:
        case TT::KW_INT:
        case TT::KW_FLOAT:
        case TT::KW_BOOL:
        case TT::KW_STRING:
            return parse_declaration_statement();
        default:
            return parse_expression_statement();
    }

    return nullptr;
}

CompoundStmt *Parser::parse_compound_statement() {
//    get();
    expect(TT::LBRACE, "Error: Expected '{' to start compound statement");

    std::vector<Stmt *> s;
    while( !is(TT::RBRACE) && !is(TT::END_OF_FILE) )
        s.push_back(parseStatement());

    expect(TT::RBRACE, "Error: Expected '}' to end compound statement");
    return new CompoundStmt(s);
}

ExpressionStmt *Parser::parse_expression_statement() {
    Expr *e = parseExpression();
    if(!e) {
        std::cerr << "From: parse_expression_statement\nError: unable to create expression" << std::endl;
        return nullptr;
    }
    expect(TT::SEMICOLON, "Error: Expected ';'");
    return new ExpressionStmt(e);
}

DeclarationStmt *Parser::parse_declaration_statement() {
    Decl *d = parseDeclaration();
    if(!d) {
        std::cerr << "From: parse_declaration_statement\nError: unable to create declaration" << std::endl;
        return nullptr;
    }

    return new DeclarationStmt(d);
}

IfStmt *Parser::parse_if_statement() {
    get();

    expect(TT::LPAREN, "Error: Expected '(' after keyword if");
    Expr *condition = parseExpression();
    if(!condition) {
        std::cout << "From: parse_if_statement\nError: unable to create desired expression" << std::endl;
        return nullptr;
    }
    expect(TT::RPAREN, "Error: Expected closing ')' for if-statement");

    Stmt *then_stmt = parseStatement();
    if(!then_stmt) {
        return nullptr;
    }

    if( is(TT::KW_ELSE) ) {
        get();
        Stmt *else_stmt = parseStatement();
        return new IfStmt(condition, then_stmt, else_stmt);
    }

    return new IfStmt(condition, then_stmt);
}

SwitchStmt *Parser::parse_switch_statement() {
    get();
    expect(TT::LPAREN, "Error: Expected '(' after 'switch'");
    Expr *e = parseExpression();
    if(!e) {
        std::cerr << "couldn't create switch expression" << std::endl;
        return nullptr;
    }
    expect(TT::RPAREN, "Error: Expected ')' after switch expression");

    expect(TT::LBRACE, "Error: Expected '{' to start switch body");
    std::vector<CaseClause *> cases;
    while( !is(TT::RBRACE) && !is(TT::END_OF_FILE)  ) {
        if( is(TT::KW_CASE) || is(TT::KW_DEFAULT) )   cases.push_back(parse_case_clause());
        else {
            expect(TT::KW_CASE, "Error: Expected 'case' inside switch");
        }
    }

    expect(TT::RBRACE, "Error: Expected '}' at the end of switch");
    return new SwitchStmt(e, cases);
}

CaseClause *Parser::parse_case_clause() {
    expect(TT::KW_CASE, "Error: Expected 'case'");
    Expr *e = parseExpression();
    if(!e) {
        std::cerr << "couldn't create case expression" << std::endl;
        return nullptr;
    }
    expect(TT::COLON, "Error: Expected ':' after case expression");

    CompoundStmt *body = parse_compound_statement();

    return new CaseClause(e, body);
}

WhileStmt *Parser::parse_while_statement() {
    get();

    expect(TT::LPAREN, "Error: Expected '(' after 'while'");
    Expr *condition = parseExpression();
    if(!condition) {
        std::cout << "From: parse_while_statement\nError: test is null" << std::endl;
        return nullptr;
    }
    expect(TT::RPAREN, "Error: Expected ')' after while expression");

    Stmt *body = parseStatement();
    if(!body) {
        std::cout << "From: parse_while_statement\nError: statement is null" << std::endl;
        return nullptr;
    }
    return new WhileStmt(condition, body);
}

DoWhileStmt *Parser::parse_do_while_statement() {
    get();

    Stmt *body = parseStatement();
    if(!body) {
        std::cout << "From: parse_do_while_statement\nError: statement is null" << std::endl;
        return nullptr;
    }
    expect(TT::KW_WHILE, "Error: Expected 'while' after 'do'");
    expect(TT::LPAREN, "Error: Expected '(' after 'while'");

    Expr *condition = parseExpression();
    if(!condition) {
        std::cout << "From: parse_do_while_statement\nError: test is null" << std::endl;
        return nullptr;
    }
    expect(TT::RPAREN, "Error: Expected ')' after while expression");
    expect(TT::SEMICOLON, "Error: Expected ';' at the end of do..while");

    return new DoWhileStmt(body, condition);
}

ForStmt *Parser::parse_for_statement() {
    get();

    expect(TT::LPAREN, "Error: Expected '('");
//    std::cout << "pos = " << pos << std::endl;
    Expr *condition = parseExpression();
    if(!condition) {
        std::cout << "From: parse_if_statement\nError: unable to create desired expression" << std::endl;
        return nullptr;
    }
    expect(TT::RPAREN, "Error: Expected ')'");
    std::cout << "pos = " << pos << std::endl;

    return nullptr;
}

ReturnStmt *Parser::parse_return_statement() {
    get();

    Expr *e = parseExpression();
    expect(TT::SEMICOLON, "Error: Expected ';' at the end of return statement");

    return new ReturnStmt(e);
}

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
