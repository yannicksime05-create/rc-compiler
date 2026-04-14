#include "../include/parser.h"
#include <sstream>

Precedence Parser::get_precedence(TokenType t) {
    switch(t) {
        default:                            return Precedence::PREC_NONE;
        case TT::COMMA:                     return Precedence::PREC_COMMA;

        case TT::ASSIGN:
        case TT::MOD_ASSIGN:
        case TT::STAR_ASSIGN:
        case TT::PLUS_ASSIGN:
        case TT::MINUS_ASSIGN:
        case TT::SLASH_ASSIGN:
        case TT::BIT_OR_ASSIGN:
        case TT::BIT_AND_ASSIGN:
        case TT::BIT_XOR_ASSIGN:
        case TT::LEFT_SHIFT_ASSIGN:
        case TT::RIGHT_SHIFT_ASSIGN:        return Precedence::PREC_ASSIGNMENT;

        case TT::QUESTION:                  return Precedence::PREC_CONDITIONAL;
        case TT::OR:                        return Precedence::PREC_LOGICAL_OR;
        case TT::AND:                       return Precedence::PREC_LOGICAL_AND;

        case TT::BIT_OR:                    return Precedence::PREC_BIT_OR;
        case TT::BIT_XOR:                   return Precedence::PREC_BIT_XOR;
        case TT::BIT_AND:                   return Precedence::PREC_BIT_AND;

        case TT::EQUAL:
        case TT::NOT_EQUAL:                 return Precedence::PREC_EQUALITY;

        case TT::LESS:
        case TT::GREATER:
        case TT::LESS_EQUAL:
        case TT::GREATER_EQUAL:             return Precedence::PREC_COMPARISON;

        case TT::LEFT_SHIFT:
        case TT::RIGHT_SHIFT:               return Precedence::PREC_SHIFT;

        case TT::PLUS:
        case TT::MINUS:                     return Precedence::PREC_TERM;

        case TT::MOD:
        case TT::STAR:
        case TT::SLASH:                     return Precedence::PREC_FACTOR;

        case TT::BIT_NOT:                   return Precedence::PREC_PREFIX;

        case TT::DOT:
        case TT::LPAREN:
        case TT::LBRACKET:
        case TT::PLUS_PLUS:
        case TT::MINUS_MINUS:               return Precedence::PREC_POSTFIX;
    }
}

bool Parser::is_right_associative(TokenType t) {
    switch(t) {
        case TT::ASSIGN:
        case TT::MOD_ASSIGN:
        case TT::STAR_ASSIGN:
        case TT::PLUS_ASSIGN:
        case TT::MINUS_ASSIGN:
        case TT::SLASH_ASSIGN:
        case TT::BIT_OR_ASSIGN:
        case TT::BIT_AND_ASSIGN:
        case TT::BIT_XOR_ASSIGN:
        case TT::LEFT_SHIFT_ASSIGN:
        case TT::RIGHT_SHIFT_ASSIGN:
        case TT::QUESTION: // conditional
            return true;

        default:
            return false;
    }
}

bool Parser::is_postfix_operator(TokenType t) {
    switch(t) {
        case TT::LPAREN:
        case TT::LBRACKET:
        case TT::DOT:
        case TT::PLUS_PLUS:
        case TT::MINUS_MINUS:
            return true;
        default:
            return false;
    }
}

Expr *Parser::parseExpression(Precedence mbp) {
    Expr *lhs = parse_primary();
    if(!lhs) return nullptr;

    while(true) {
        TokenType t = current().type;
        Precedence lbp = get_precedence(t);
        if(lbp < mbp) break;

        Precedence rbp = static_cast<Precedence>( lbp + (is_right_associative(t) ? -1 : 1) );
//        std::cout << "op: " << current().value << " lbp: " << lbp << ", rbp: " << rbp << ", min_bp: " << mbp  << std::endl;

        if( is_postfix_operator(t) ) {
            lhs = parse_postfix(lhs);
            continue;
        }

        if(t == TT::QUESTION) {
            get();
            Expr *then_expr = parseExpression();
            expect(TT::COLON, "Error: Expected ':' in ternary operator!");
            Expr *else_expr = parseExpression();
            lhs = new ConditionalExpr(lhs, then_expr, else_expr);
            continue;
        }

        Token op = get();

        Expr *rhs = parseExpression(rbp);
        if(!rhs) return nullptr;

        if(lbp == Precedence::PREC_ASSIGNMENT)    lhs = new AssignmentExpr(lhs, op.value, rhs);
        else if(lbp == Precedence::PREC_COMMA)    lhs = new SequenceExpr( {lhs, rhs} );
        else                                      lhs = new BinaryExpr(lhs, op.value, rhs);

    }

    return lhs;
}

//NUD
Expr *Parser::parse_primary() {
    switch(current().type) {
        case TT::IDENTIFIER:
            return new IdentifierExpr( get() );
        case TT::INTEGER:
            return new IntNumberExpr( std::stod(get().value) );
        case TT::FLOAT:
            return new DecimalNumberExpr( std::stof(get().value) );
        case TT::STRING:
            return new StringExpr( get().value );

        case TT::MINUS:
        case TT::NOT:
        case TT::BIT_NOT:
        case TT::PLUS_PLUS:
        case TT::MINUS_MINUS:
            return new UnaryExpr(get().value, parseExpression(Precedence::PREC_PREFIX));

        case TT::LPAREN: {
            get();
            Expr *e = parseExpression();
            expect(TT::RPAREN, "Error: Expected closing ')' after expression!");
            return e;
        }
        default:
            std::cerr << "Error: Unknow primary Expression!" << std::endl;
            return nullptr;
    }
}

Expr *Parser::parse_postfix(Expr *lhs) {
    if( is(TT::LPAREN) ) {
        get();

        if( !is(TT::RPAREN) ) {
            std::vector<Expr *> args;
            args.push_back(parseExpression(Precedence::PREC_POSTFIX));
            while( is(TT::COMMA) ) {
                get();
                args.push_back(parseExpression(Precedence::PREC_POSTFIX));
            }
            expect(TT::RPAREN, "Error: Expected ')' after argument list");

            lhs = new CallExpr(lhs, args);
        }
        else {
            get();
            lhs = new CallExpr(lhs);
        }
    }
    else if( is(TT::LBRACKET) ) {
        get();
        Expr *index = parseExpression(Precedence::PREC_POSTFIX);
        expect(TT::RBRACKET, "Error: Expected closing ']' to complete subscript expression");

        lhs = new SubscriptExpr(lhs, index);
    }
    else if( is(TT::DOT) ) {
        get();
        expect(TT::IDENTIFIER, "Error: Expected identifier for member expressions");

        lhs = new MemberAccessExpr(lhs, previous().value);
    }
    else if( is(TT::PLUS_PLUS) || is(TT::MINUS_MINUS) ) {
        lhs = new UnaryExpr(get().value, lhs, false);
    }

    return lhs;
}








Decl *Parser::parseDeclaration() {
    TypeSpecifier *type = parse_type_specifier();
    expect(TT::IDENTIFIER, "Expected identifier");

    /** If the previous token was actually an identifier, we need to take it back */
    --pos;

    if( peek().is(TT::LPAREN) )
        return parse_function_declaration(*type);
    else
        return parse_variable_declaration(*type);

    return nullptr;
}

TypeSpecifier *Parser::parse_type_specifier() {
    bool c = false;
    if( is(TT::KW_CONST) ) {
        get();
        c = true;
    }

    return new TypeSpecifier(get().value, c);
}

VariableDecl *Parser::parse_variable_declaration(const TypeSpecifier& type) {
//    TypeSpecifier type = *parse_type_specifier();

//    expect(TT::IDENTIFIER, "Expected identifier after type name");
//
//    /** If the previous token was actually an identifier, we need to take it back */
//    --pos;

    std::vector<VariableDeclarator *> decls;
    decls.push_back(parse_variable_declarator(type.type_name));
    while( is(TT::COMMA) ) {
        get();
        decls.push_back(parse_variable_declarator(type.type_name));
    }
    expect(TT::SEMICOLON, "Error: Expected ';' at the end of variables' declarations");

    return new VariableDecl(type, decls);
}

VariableDeclarator *Parser::parse_variable_declarator(const std::string& tn) {
    std::string name = get().value;

    if(tn == "auto" || tn == "any" || tn == "number") {
        expect(TT::ASSIGN, "Missing initialization for deduced types!");
        --pos;
    }

    if( is(TT::ASSIGN) ) {
        get();
        Expr *init = parseExpression();

        return new VariableDeclarator(name, init);
    }

    return new VariableDeclarator(name);
}

FunctionDecl *Parser::parse_function_declaration(const TypeSpecifier& type) {
    std::string name = get().value;

    expect(TT::LPAREN, "Error: Expected '(' after function's name");
    if( !is(TT::RPAREN) ) {
        std::vector<Parameter *> parameters;
        parameters.push_back(parse_function_parameters());
        while( is(TT::COMMA) ) {
            get();
            parameters.push_back(parse_function_parameters());
        }
        expect(TT::RPAREN, "Error: Expected ')' after parameter list");

        return new FunctionDecl(type, name, parse_compound_statement(), parameters);
    }
    expect(TT::RPAREN, "Error: Expected ')' after parameter list");

    return new FunctionDecl(type, name, parse_compound_statement());
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
    expect(TT::SEMICOLON, "Error: Expected ';' at the end of expression");
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
        std::cerr << "Error: Expected primary expression for if-statement" << std::endl;
        return nullptr;
    }
    expect(TT::RPAREN, "Error: Expected closing ')' for if-statement");

    Stmt *then_stmt = parseStatement();
    if(!then_stmt) {
        return new IfStmt(condition);
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
//        throw ParseError("Expected primary-expression after '('");
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

//for(int n : v)
//for(int n = 0; ...)
//bool Parser::is_rangefor_pattern() {
//
//}

Stmt *Parser::dispatch_for_statements() {
    get();
    expect(TT::LPAREN, "Error: Expected '(' after keyword 'for'");

//    if(is_rangefor_pattern()) return parse_rangefor_statement();

    return parse_for_statement();
}

ForStmt *Parser::parse_for_statement() {
    Stmt *init = parseStatement();
//    expect(TT::SEMICOLON, "Error: Expected ';' after for-loop initialization");

    Expr *condition = parseExpression();
    if(!condition) {
        std::cerr << "From: parse_for_statement\nError: unable to create desired expression" << std::endl;
//        return nullptr;
    }
    expect(TT::SEMICOLON, "Error: Expected ';' after for-loop condition");

    Expr *incr = parseExpression();
    expect(TT::RPAREN, "Error: Expected ')' after for-loop increment");

    Stmt *body = parseStatement();

    return new ForStmt(init, condition, incr, body);
}

//RangeForStmt *Parser::parse_rangefor_statement(VariableDecl *v) {
//    expect(TT::COLON, "");
//    Expr *range = parseExpression();
//    expect(TT::RPAREN, "Error: Expected ')' after for-loop");
//    return new RangeForStmt(v, range, parseStatement());
//}

ReturnStmt *Parser::parse_return_statement() {
    get();

    Expr *e = parseExpression();
    expect(TT::SEMICOLON, "Error: Expected ';' at the end of return statement");

    return new ReturnStmt(e);
}
