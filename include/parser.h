#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "diagnostic.h"

using TT = TokenType;

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

class ParseError : public CompileError {

public:
    ParseError(const SourceRange& r, const std::string& msg) : CompileError(r, msg) {}
};

class Parser {
    std::vector<Token>& tokens;
    DiagnosticEngine& diagnostic;
    size_t pos = 0;

    // Used with parse_sub_statement() to report declarations happening in single statements
    bool is_compound_statement = false;

    // parse_for_statement uses peek(-2) to get the keyword back
    Token& peek(int n = 1) {
        size_t i = pos + static_cast<size_t>(n);

        if(i <= 0)                  return tokens.front();
        else if(i >= tokens.size()) return tokens.back();

        return tokens[i];
    }

    Token& get() { return pos >= tokens.size() ? tokens.back() : tokens[pos++]; }

    Token& current() { return pos >= tokens.size() ? tokens.back() : tokens[pos]; }

    /**
    *   This function is useful when:
    *   - we need to do an expect() (so that the program stops if there is an error),
    *   - but we also need to recover the skipped token if there were no errors.
    */
    Token& previous() { return peek(-1); }

    /**
    *   Tells if the current token is of a certain type.
    *
    *   @param t the type to check.
    *   @return true if so, false otherwise.
    */
    bool is(TokenType t) {
        return current().type == t;
    }

    bool is_primitive_type(size_t i) {
        Token t = tokens[i];
        switch(t.type) {
            case TT::KW_ANY:
            case TT::KW_AUTO:
            case TT::KW_BOOL:
            case TT::KW_BYTE:
            case TT::KW_CHAR:
            case TT::KW_DOUBLE:
            case TT::KW_FLOAT:
            case TT::KW_INT:
            case TT::KW_LONG:
//            case TT::KW_QUAD:
            case TT::KW_SHORT:
            case TT::KW_STRING:
            case TT::KW_UBYTE:
            case TT::KW_UINT:
            case TT::KW_USHORT:
            case TT::KW_ULONG:
            case TT::KW_VOID:      return true;

            default:               return false;
        }
    }

    bool is_right_associative(TokenType t);
    bool is_postfix_operator(TokenType t);

    std::string final_msg(const std::string& msg) {
        return msg + ", found '" + current().value + "' instead";
    }

    void error(const std::string& msg) {
        const std::string& s = final_msg(msg);

        throw ParseError(current().range, s);
    }

    void expect(TokenType t, const std::string& msg) {
        if( is(t) ) {
            ++pos;
            return;
        }

        // A missing ';' (or ')' ...) is where the user has to type it: right after the previous token,
        // not where the next token is.
        bool is_delimiter = t == TT::SEMICOLON || t == TT::COMMA || t == TT::COLON || t == TT::RPAREN || t == TT::RBRACE || t == TT::RBRACKET;
        if( pos > 0 && is_delimiter ) {
            SourceRange r;
            r.begin = r.end = tokens[pos - 1].range.end;

            throw ParseError(r, final_msg(msg));
        }

        error(msg);
    }

    bool starts_statement() {
        switch(current().type) {
            case TT::KW_IF:     case TT::KW_SWITCH: case TT::KW_WHILE:
            case TT::KW_DO:     case TT::KW_FOR:    case TT::KW_RETURN:
            case TT::KW_PRINT:  case TT::KW_BREAK:  case TT::KW_CONTINUE:
            case TT::LBRACE:
                return true;
            default:
                return false;
        }
    }

    bool starts_declaration() {
        switch(current().type) {
            case TT::KW_CONST: return true;
            default:
                if( is_primitive_type(pos) ) return true;

                if( is(TT::IDENTIFIER) && peek().type == TT::IDENTIFIER )
                    return true;
        }

        return false;
    }

    // skip to a safe point
    void synchronize() {
        int depth = 0;

        while( !is(TT::END_OF_FILE) ) {
//            if(depth == 0 && pos > start && starts_declaration()) return;

            if( is(TT::LBRACE) )                      ++depth;
            else if( is(TT::RBRACE) ) {
                if(depth > 0) --depth;
                get();

                // we're back at the global scope
                if(depth == 0) return;
                continue;
            }
            else if( is(TT::SEMICOLON) && depth == 0 ) {
                get();
                return;
            }

            get();
        }
    }




    Precedence get_precedence(TokenType t);
    Expr *parseExpression(Precedence mbp = Precedence::PREC_DEFAULT);
    Expr *parse_primary();
    Expr *parse_postfix(Expr *lhs);




    Decl                    *parse_top_level_declaration();
    Decl                    *parseDeclaration(bool at_file_scope = true);
    bool                     starts_variable_declaration(bool in_rangefor = false);
    TypeSpecifier            parse_type_specifier();
    VariableDeclarator      *parse_variable_declarator();
    VariableDecl            *parse_variable_declaration();
    Parameter               *parse_function_parameters();
    FunctionPrototype       *parse_function_prototype();
    FunctionDecl            *parse_function_declaration();




    Stmt                *parseStatement();
    Stmt                *parse_sub_statement();
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
    VariableDecl        *parse_rangefor_variable();
    RangeForStmt        *parse_rangefor_statement();
    ReturnStmt          *parse_return_statement();
    PrintStmt           *parse_print_statement();
    BreakStmt           *parse_break_statement();
    ContinueStmt        *parse_continue_statement();



public:
    Parser(std::vector<Token>& tokens, DiagnosticEngine& d) : tokens(tokens), diagnostic(d), pos(0) {}

    Program *parse() {
        std::vector<Decl *> declarations;
        while( !is(TT::END_OF_FILE) ) {
            try {
                declarations.push_back( parse_top_level_declaration() );
            }
            catch(const CompileError& e) {
                diagnostic.error(e);
                synchronize();
            }
        }

        return new Program(declarations);
    }
};

#endif // PARSER_H
