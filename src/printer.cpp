#include "../include/printer.h"

void Printer::printStatement(const Stmt *s) {
    switch(s->type) {
        case ANT::COMP_STMT_NODE: {
            break;
        }
        case ANT::IF_STMT_NODE: {
            auto *ss = static_cast<const IfStmt *>(s);
            indent(); std::cout << "{\n";
            indent(); std::cout << "node type: IfStatement,\n";
            indent(); std::cout << "condition: {\n";
            nspace += tab_length;
            printExpression(ss->condition);
            nspace -= tab_length;
            indent(); std::cout << "},\n";
            indent(); std::cout << "then:\n";
            nspace += tab_length;
            printStatement(ss->then_statement);
            nspace -= tab_length;
//            indent(); std::cout << ",\n";
            if(ss->else_statement) {
                indent(); std::cout << "else:\n";
                nspace += tab_length;
                printStatement(ss->else_statement);
                nspace -= tab_length;
            }
            indent(); std::cout << "}\n";
            break;
        }
        default: {
            auto *ss = static_cast<const ExprStmt *>(s);
            indent(); std::cout << "{\n";
            indent(); std::cout << "node type: ExpressionStatement,\n";
            indent(); std::cout << "expression: {\n";
            nspace += tab_length;
            printExpression(ss->expr);
            nspace -= tab_length;
            indent(); std::cout << "}\n";
            break;
        }
    }
}

void Printer::printExpression(const Expr *e) {
    switch(e->type) {
        case ANT::INT_LIT_NODE:
        case ANT::STRING_LIT_NODE:
        case ANT::DECIMAL_LIT_NODE: {
            print_literal_exprs(e);
            break;
        }
        case ANT::UNARY_EXP_NODE: {
            auto *n = static_cast<const UnaryExpr *>(e);
//            indent(); std::cout << "{\n";
            indent(); std::cout << "node type: UnaryExpression,\n";
            indent(); std::cout << "operator: \"" << n->op << "\",\n";
            indent(); std::cout << "expression: {\n";
            exprs_printer_helper(n->expr);
            indent(); std::cout << "}\n";

//            std::cout << ind << "{\n";
//            std::cout << ind << "  node type: \"UnaryExpression\",\n";
//            std::cout << ind << "  operator: \"" << n->op << "\",\n";
//            print(n->expr, indent + 2);
//            std::cout << "\n" << ind << "}";
            break;
        }
        case ANT::BINARY_EXPR_NODE:
        case ANT::ASSIGNMENT_EXPR_NODE: {
            print_two_operands_exprs(e);
            break;
        }
        case ANT::IDENTIFIER_EXPR_NODE: {
            auto *n = static_cast<const IdentifierExpr *>(e);
            indent(); std::cout << "node type: Identifier,\n";
            indent(); std::cout << "name: \"" << n->name.value << "\"\n";
            break;
        }
        default:
            std::cout << "Unknown expression!" << std::endl;
            break;
    }
}

void Printer::print_literal_exprs(const Expr *e) {
    indent(); std::cout << "node type: Literal,\n";
    indent(); std::cout << "{\n";
    nspace += tab_length;

    switch(e->type) {
        case ANT::INT_LIT_NODE: {
            auto *n = static_cast<const IntNumberExpr *>(e);
            indent(); std::cout << "value: " << n->value << "\n";
            break;
        }
        case ANT::DECIMAL_LIT_NODE: {
            auto *n = static_cast<const DecimalNumberExpr *>(e);
            indent(); std::cout << "  value: " << n->value << "\n";
            break;
        }
        case ANT::STRING_LIT_NODE: {
            auto *n = static_cast<const StringExpr *>(e);
            indent(); std::cout << "  value: " << n->value << "\n";
        }
        default:
            break;

    }
    nspace -= tab_length;
    indent(); std::cout << "}\n";
}

void Printer::print_two_operands_exprs(const Expr *e) {
    indent(); std::cout << "node type: ";
    switch(e->type) {
        case ANT::ASSIGNMENT_EXPR_NODE: {
            auto *n = static_cast<const AssignExpr *>(e);
            std::cout << "AssignmentExpression,\n";
            indent(); std::cout << "operator: \"" << n->op << "\",\n";

            indent(); std::cout << "target: {\n";
            exprs_printer_helper(n->target);
            indent(); std::cout << "},\n";

            indent(); std::cout << "value: {\n";
            exprs_printer_helper(n->value);
            indent(); std::cout << "}\n";
            break;
        }
        case ANT::BINARY_EXPR_NODE: {
            auto *n = static_cast<const BinaryExpr *>(e);
            std::cout << "BinaryExpression,\n";
            indent(); std::cout << "operator: \"" << n->op << "\",\n";

            indent(); std::cout << "left: {\n";
            exprs_printer_helper(n->left);
            indent(); std::cout << "},\n";

            indent(); std::cout << "right: {\n";
            exprs_printer_helper(n->right);
            indent(); std::cout << "}\n";
            break;
        }
        default:
            break;
    }
}

void Printer::exprs_printer_helper(Expr *e) {
    nspace += tab_length;
    printExpression(e);
    nspace -= tab_length;
}
