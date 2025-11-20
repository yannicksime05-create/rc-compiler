#include "../include/printers.h"

int printer_indent = 0;

void ExprPrinter::print(const Expr *expr, const int n) {
    printer_indent = n;
    std::string ind(printer_indent, ' ');
    switch(expr->type) {
        case ANT::INT_LIT_NODE:
        case ANT::STRING_LIT_NODE:
        case ANT::DECIMAL_LIT_NODE: {
            printLiteralNodes(expr, ind);
            break;
        }
        case ANT::UNARY_EXP_NODE: {
            auto *n = static_cast<const UnaryExpr *>(expr);
            std::cout << ind << "{\n";
            std::cout << ind << "  node type: \"UnaryExpression\",\n";
            std::cout << ind << "  operator: \"" << n->op << "\",\n";
            print(n->expr, printer_indent + 2);
            std::cout << "\n" << ind << "}";
            break;
        }
        case ANT::ASSIGN_EXPR_NODE:
        case ANT::BINARY_EXPR_NODE: {
            printTwoOperandsNodes(expr, ind);
            break;
        }
        default:
            std::cout << "Unknown expression!" << std::endl;
            break;
    }
}

void ExprPrinter::printTwoOperandsNodes(const Expr *e, const std::string& ind) {
    std::cout << ind << "{\n";
    std::cout << ind << "  node type: ";
    switch(e->type) {
        case ANT::ASSIGN_EXPR_NODE: {
            std::cout << "\"AssignmentExpression\",\n";
            auto *n = static_cast<const AssignExpr *>(e);
            std::cout << ind << "  operator: \"" << n->op << "\",\n";
            std::cout << ind << "  left: ";
            print(n->target, printer_indent + 2);
            std::cout << ",\n";
            std::cout << ind << "  right: ";
            print(n->value, printer_indent + 2);
        }
        case ANT::BINARY_EXPR_NODE: {
            std::cout << "\"BinaryExpression\",\n";
            auto *n = static_cast<const BinaryExpr *>(e);
            std::cout << ind << "  operator: \"" << n->op << "\",\n";
            std::cout << ind << "  left: ";
            print(n->left, printer_indent + 2);
            std::cout << ",\n";
            std::cout << ind << "  right: ";
            print(n->right, printer_indent + 2);
        }
        default:
            break;
    }
    std::cout << "\n" << ind << "}";
}
