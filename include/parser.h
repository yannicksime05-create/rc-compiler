#ifndef PARSER_H
#define PARSER_H

#include <stdexcept>
#include <sstream>
#include "ast.h"

using TT = TokenType;

//enum Precedence {
//    PREC_NONE               = -1,
//
//    PREC_DEFAULT            = 0,
//
//    PREC_COMMA              = 1,        // ,
//    PREC_ASSIGNMENT         = 2,       // = += -= ...
//    PREC_CONDITIONAL        = 3,       // ?:
//    PREC_LOGICAL_OR         = 4,       // ||
//    PREC_LOGICAL_AND        = 5,       // &&
//    PREC_EQUALITY           = 6,       // == !=
//    PREC_COMPARISON         = 7,       // < <= > >=
//    PREC_TERM               = 8,       // + -
//    PREC_FACTOR             = 9,       // * / %
//    PREC_PREFIX             = 10,      // - ! ++ --
//    PREC_POSTFIX            = 11       // () [] . ++ --
//};

enum Precedence {
    PREC_NONE = -1,
    PREC_DEFAULT,
    PREC_COMMA,          // ,
    PREC_ASSIGNMENT,     // = += -= *= ...
    PREC_CONDITIONAL,    // ?:
    PREC_LOGICAL_OR,     // ||
    PREC_LOGICAL_AND,    // &&
    PREC_BIT_OR,         // |
    PREC_BIT_XOR,        // ^
    PREC_BIT_AND,        // &
    PREC_EQUALITY,       // == !=
    PREC_COMPARISON,     // < <= > >=
    PREC_SHIFT,          // << >>
    PREC_TERM,           // + -
    PREC_FACTOR,         // * / %
    PREC_PREFIX,         // ! ~ - ++ --
    PREC_POSTFIX         // () [] . ++ --
};

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


    Token& peek(size_t n = 1) { return (pos + n >= tokens.size()) ? tokens.back() : tokens[pos + n]; }

    Token& get() { return (pos >= tokens.size()) ? tokens.back() : tokens.at(pos++); }

    Token& current() { return tokens.at(pos); }

    /**
    *   This function is useful when:
    *   - we need to do an expect() (so that the program stops if there is an error),
    *   - but we also need to recover the skipped token if there were no errors.
    */
    Token& previous() { return tokens.at(pos - 1); }

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
        is(TT::BIT_AND_ASSIGN) || is(TT::BIT_OR_ASSIGN) || is(TT::BIT_XOR_ASSIGN) || is(TT::LEFT_SHIFT_ASSIGN) || is(TT::RIGHT_SHIFT_ASSIGN);
    }

    bool is_literal() {
        return is(TT::INTEGER) || is(TT::FLOAT) || is(TT::STRING);
    }

    bool is_right_associative(TokenType t);
    bool is_postfix_operator(TokenType t);


    Precedence get_precedence(TokenType t);
    Expr *parseExpression(Precedence mbp = Precedence::PREC_DEFAULT);
    Expr *parse_primary();
    Expr *parse_postfix(Expr *lhs);




    Decl                    *parseDeclaration();
    TypeSpecifier           *parse_type_specifier();
    VariableDecl            *parse_variable_declaration(const TypeSpecifier& type, const std::string& name);
    VariableDeclarator      *parse_variable_declarator(const std::string& type_name, const std::string& name);
    FunctionDecl            *parse_function_declaration(const TypeSpecifier& type, const std::string& name);
    Parameter               *parse_function_parameters();




    Stmt                *parseStatement();
    bool                 starts_declaration();
    CompoundStmt        *parse_compound_statement();
    ExpressionStmt      *parse_expression_statement();
    DeclarationStmt     *parse_declaration_statement();
    IfStmt              *parse_if_statement();
    SwitchStmt          *parse_switch_statement();
    CaseClause          *parse_case_clause();

    WhileStmt           *parse_while_statement();
    DoWhileStmt         *parse_do_while_statement();

    Stmt                *dispatch_for_statements();
    ForStmt             *parse_for_statement();
//    RangeForStmt        *parse_rangefor_statement(VariableDecl *v);

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
