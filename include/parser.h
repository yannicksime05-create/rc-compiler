#ifndef PARSER_H
#define PARSER_H

#include "printer.h"

using TT = TokenType;

class TypeTable {
    void addType(const std::string& name) {

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

    Token& peek() {
        if(pos >= tokens.size()) return  tokens.back();

        return tokens[pos + 1];
    }

    void expect(TokenType t, const std::string& error_msg) {
        if( !is(t) ) {
            std::cerr << "From expect:\n" << error_msg << " found: '" << current().value << "' instead! At line: " << current().loc.line << ", Column: " << current().loc.col;
            std::cout << std::endl;
            return;
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
        is(TT::ASSIGN) || is(TT::PLUS_EQUAL) || is(TT::MINUS_EQUAL) || is(TT::STAR_EQUAL) || is(TT::SLASH_EQUAL) || is(TT::MOD_EQUAL);
    }

    bool is_literal() {
        return is(TT::INTEGER) || is(TT::FLOAT) || is(TT::STRING);
    }



    Expr *parseExpression();
    Expr *parse_assignment();
    Expr *parse_logical_or();
    Expr *parse_logical_and();
    Expr *parse_equality();
    Expr *parse_comparaison();
    Expr *parse_term();
    Expr *parse_factor();
    Expr *parse_unary();

    Expr *parse_postfix();

    Expr *parse_primary();
    Expr *parse_literal();
    Expr *parse_identifier();




    Decl                    *parseDeclaration();
    TypeSpecifier           *parse_type_specifier();
    VariableDecl            *parse_variable_declaration();
    VariableDeclarator      *parse_variable_declarator();
    FunctionDecl            *parse_function_declaration();
    Parameter               *parse_function_parameters();




    Stmt                *parseStatement();
    CompoundStmt        *parse_compound_statement();
    ExpressionStmt      *parse_expression_statement();
    DeclarationStmt     *parse_declaration_statement();
    IfStmt              *parse_if_statement();

    //FIXME!!
    SwitchStmt          *parse_switch_statement();
    CaseClause          *parse_case_clause();

    WhileStmt           *parse_while_statement();
    DoWhileStmt         *parse_do_while_statement();
    ForStmt             *parse_for_statement();
    ReturnStmt          *parse_return_statement();



public:
    Parser(std::vector<Token>& tokens) : tokens(tokens), pos(0) {}

//    std::vector<Stmt *> parse() {
//        std::vector<Stmt *> statements;
//
//        return statements;
//    }

    void parse() {
        Stmt *s = parseStatement();
        if(!s) {
            std::cout << "statement is null" << std::endl;
            return;
        }
        Printer p;
        p.print(s);
        delete s;
        s = nullptr;

//        VarDecl *vd = var_declaration();
//
//        if(!vd) {
//            std::cout << "e is null" << std::endl;
//            return;
//        }
//        DeclPrinter::print(vd);
//        std::cout << "\n\nResult = " << interpretBinaryExpr(e) << std::endl;
//        delete vd;
//        vd = nullptr;
    }

    double interpretBinaryExpr(BinaryExpr *e);
//    double evalBinary(BinaryExpr *be);

};

#endif // PARSER_H
