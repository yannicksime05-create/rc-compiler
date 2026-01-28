#ifndef PARSER_H
#define PARSER_H

#include <stdexcept>
#include <sstream>
#include "ast.h"

using TT = TokenType;

class ParseError : public std::logic_error {

public:
    ParseError(const std::string& error_msg) : std::logic_error(error_msg) {}

    const char *what() {
        return std::logic_error::what();
    }

};

class Parser {
    std::vector<Token>& tokens;
    size_t pos;


    Token& get() {
        if(pos >= tokens.size()) return tokens.back();

        return tokens.at(pos++);
    }

    Token& current() { return tokens.at(pos); }

    /**
    *   This function is useful when:
    *   - we need to do an expect() (so that the program stops if there is an error),
    *   - but we also need to recover the skipped token if there were no error.
    */
    Token& previous() { return tokens.at(pos - 1); }

    Token& peek(size_t n = 1) {
        if(pos + n >= tokens.size()) return tokens.back();

        return tokens[pos + n];
    }

    void expect(TokenType t, const std::string& error_msg) {
        if( !is(t) ) {
            std::stringstream s;
            s << error_msg << " found: '" << current().value << "' instead! At line: " << current().loc.line << ", Column: " << current().loc.col;
            throw ParseError(s.str());
        }

        ++pos;
    }

    /**
    *   Tells if the current token is of a certain type.
    *
    *   @param t the type to check.
    *   @return true if so, false otherwise.
    */
    bool is(TokenType t) {
        return current().type == t;
    }

    bool is_assignment_operator() {
        return
        is(TT::ASSIGN) || is(TT::PLUS_ASSIGN) || is(TT::MINUS_ASSIGN) || is(TT::STAR_ASSIGN) || is(TT::SLASH_ASSIGN) || is(TT::MOD_ASSIGN) ||
        is(TT::BIT_AND_ASSIGN) || is(TT::BIT_OR_ASSIGN) || is(TT::BIT_XOR_ASSIGN) || is(TT::SHIFT_LEFT_ASSIGN) || is(TT::SHIFT_RIGHT_ASSIGN);
    }

    bool is_literal() {
        return is(TT::INTEGER) || is(TT::FLOAT) || is(TT::STRING);
    }



    Expr *parseExpression();
    Expr *parse_assignment();
    Expr *parse_conditional();
    Expr *parse_logical_or();
    Expr *parse_logical_and();
    Expr *parse_equality();
    Expr *parse_comparaison();
    Expr *parse_term();
    Expr *parse_factor();
    Expr *parse_unary();
    Expr *parse_postfix();
    Expr *parse_primary();




    Decl                    *parseDeclaration();
    TypeSpecifier           *parse_type_specifier();
    VariableDecl            *parse_variable_declaration(const TypeSpecifier& type);
    VariableDeclarator      *parse_variable_declarator(const std::string& tn);
    FunctionDecl            *parse_function_declaration(const TypeSpecifier& type);
    Parameter               *parse_function_parameters();




    Stmt                *parseStatement();
    CompoundStmt        *parse_compound_statement();
    ExpressionStmt      *parse_expression_statement();
    DeclarationStmt     *parse_declaration_statement();
    IfStmt              *parse_if_statement();
    SwitchStmt          *parse_switch_statement();
    CaseClause          *parse_case_clause();

    WhileStmt           *parse_while_statement();
    DoWhileStmt         *parse_do_while_statement();

    ForStmt             *parse_for_statement();

    ReturnStmt          *parse_return_statement();



public:
    Parser(std::vector<Token>& tokens) : tokens(tokens), pos(0) {}

    Program *parse() {
        std::vector<Stmt *> statements;
        do{
            statements.push_back(parseStatement());
        }while( !is(TT::END_OF_FILE) );

        return new Program(statements);
    }

};

#endif // PARSER_H
