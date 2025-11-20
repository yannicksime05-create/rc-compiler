#ifndef PRINTERS_H
#define PRINTERS_H

#include <iostream>
#include "ast.h"

using ANT = ASTNodeType;

struct ExprPrinter {
    static void print(const Expr *expr, const int n = 0);

    static void printLiteralNodes(const Expr *e, const std::string& ind) {
        std::cout << ind << "{\n";
        std::cout << ind << "  node type: \"Literal\",\n";
        switch(e->type) {
            case ANT::INT_LIT_NODE: {
                auto *n = static_cast<const IntNumberExpr *>(e);
                std::cout << ind << "  value: " << n->value << "\n";
                break;
            }
            case ANT::DECIMAL_LIT_NODE: {
                auto *n = static_cast<const DecimalNumberExpr *>(e);
                std::cout << ind << "  value: " << n->value << "\n";
                break;
            }
            case ANT::STRING_LIT_NODE: {
                auto *n = static_cast<const StringExpr *>(e);
                std::cout << ind << "  value: " << n->value << "\n";
            }
            default:
                break;

        }
        std::cout << ind << "}";
    }

    static void printTwoOperandsNodes(const Expr *e, const std::string& ind);
};

struct StmtPrinter {

};

struct DeclPrinter {
    static void print(const Decl *d, const int indent = 0) {
        std::string ind(indent, ' ');
        switch(d->type) {
            case ANT::VAR_DECL_NODE: {
                auto *n = static_cast<const VarDecl *>(d);
                std::cout << ind << "{\n";
                std::cout << ind << "  node type: \"VariableDeclaration\",\n";
                std::cout << ind << "     type: \"" << n->type_name << "\",\n";
                std::cout << ind << "     name: \"" << n->name << "\",\n";
                std::cout << ind << "     initializer: ";
                if(!n->initializer) {
                    std::cout << "null\n" << ind << "}" << std::endl;
                    return;
                }
                ExprPrinter::print(n->initializer, indent + 6);
                std::cout << "\n" << ind << "}";
                break;
            }
            default:
                std::cout << "Unknown declaration!" << std::endl;
                break;
        }
    }
};

#endif // PRINTERS_H
