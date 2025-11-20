#include "../include/parser.h"
#include <sstream>

Expr *Parser::binary_expression() {
    //expr    → term (( "+" | "-" ) term)*
    //term    → factor (( "*" | "/" ) factor)*
    //factor  → NUMBER | STRING | IDENTIFIER | "(" expr ")"

    Expr *e = term_expression(), *left = nullptr;
    while(current().value == "+" || current().value == "-") {
        left = e;
        e = new BinaryExpr(left, get().value[0], term_expression());
    }

    left = nullptr;
    return e;
}

Expr *Parser::term_expression() {
    Expr *e = factor_expression(), *left = nullptr;
    while(current().value == "*" || current().value == "/") {
        left = e;
        e = new BinaryExpr(left, get().value[0], factor_expression());
    }

    left = nullptr;
    return e;
}

Expr *Parser::factor_expression() {
    Expr *e = nullptr;
    if(current().value == "(") {
        get();
        e = binary_expression();
        if(current().value != ")") {
            std::stringstream s;
            s << "Error: Expected ')', found '" << current().value << "' instead! Line: " << current().loc.line << ", Col: " << current().loc.col;
            throw std::logic_error(s.str());
        }
        else get();
    }
    else if(current().value == "-" || current().value == "!") {
        std::string o = get().value;
        Expr *r = factor_expression();
        return new UnaryExpr(r, o);
    }

    if(!e) {
        e = literal_expression();
    }

    return e;
}

Expr *Parser::literal_expression() {
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

//UnaryExpr *Parser::unary_expression() {
//    new UnaryExpr(nullptr, )
//}

//Decl *Parser::ParseDeclaration() {
//    return var_declaration();
//}

VarDecl *Parser::var_declaration() {
    std::string tn = get().value;
    std::string n = get().value;
    if(current().value == "=") {
        get();
        return new VarDecl(tn, n, binary_expression());
    }
    else return new VarDecl(tn, n, nullptr);
}

double Parser::interpretBinaryExpr(BinaryExpr *e) {
    double l, r;

    switch(e->left->type) {
        case ASTNodeType::BINARY_EXPR_NODE: {
            l = interpretBinaryExpr( static_cast<BinaryExpr *>(e->left) );
            break;
        }
        case ASTNodeType::INT_LIT_NODE: {
            auto *le = static_cast<IntNumberExpr *>(e->left);
            l = le->value;
            break;
        }
        case ASTNodeType::DECIMAL_LIT_NODE: {
            auto *le = static_cast<DecimalNumberExpr *>(e->left);
            l = le->value;
            break;
        }
        default:
            break;
    }

    switch(e->right->type) {
        case ASTNodeType::BINARY_EXPR_NODE: {
            r = interpretBinaryExpr( static_cast<BinaryExpr *>(e->right) );
            break;
        }
        case ASTNodeType::INT_LIT_NODE: {
            auto *re = static_cast<IntNumberExpr *>(e->right);
            r = re->value;
            break;
        }
        case ASTNodeType::DECIMAL_LIT_NODE: {
            auto *re = static_cast<DecimalNumberExpr *>(e->right);
            r = re->value;
            break;
        }
        default:
            break;
    }

    switch(e->op) {
        case '+': return l + r;
        case '-': return l - r;
        case '*': return l * r;
        case '/': return l / r;
        default:
            break;
    }

    return 0;
}



//double Parser::evalBinary(BinaryExpr *e) {
//
//}
