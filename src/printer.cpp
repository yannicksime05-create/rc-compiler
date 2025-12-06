#include "../include/printer.h"

void Printer::printExpression(const Expr *e) {
    switch(e->node_type) {
        case ANT::INT_LIT_NODE:
        case ANT::STRING_LIT_NODE:
        case ANT::DECIMAL_LIT_NODE: {
            print_literal_exprs(e);
            break;
        }
        case ANT::UNARY_EXP_NODE: {
            auto *ee = static_cast<const UnaryExpr *>(e);
            indent(); std::cout << "node type: UnaryExpression,\n";
            indent(); std::cout << "operator: \"" << ee->op << "\",\n";
            indent(); std::cout << "expression: {\n";
            exprs_printer_helper(ee->expr);
            indent(); std::cout << "}\n";
            break;
        }
        case ANT::BINARY_EXPR_NODE:
        case ANT::ASSIGNMENT_EXPR_NODE: {
            print_two_operands_exprs(e);
            break;
        }
        case ANT::IDENTIFIER_EXPR_NODE: {
            auto  *ee = static_cast<const IdentifierExpr *>(e);
            indent(); std::cout << "node type: Identifier,\n";
            indent(); std::cout << "name: \"" << ee->name.value << "\"\n";
            break;
        }
        default:
            std::cout << "Unknown expression!" << std::endl;
            break;
    }
}

void Printer::print_literal_exprs(const Expr *e) {
    indent(); std::cout << "node type: Literal,\n";

    switch(e->node_type) {
        case ANT::INT_LIT_NODE: {
            auto *ee = static_cast<const IntNumberExpr *>(e);
            indent(); std::cout << "value: " << ee->value << "\n";
            break;
        }
        case ANT::DECIMAL_LIT_NODE: {
            auto *ee = static_cast<const DecimalNumberExpr *>(e);
            indent(); std::cout << "value: " << ee->value << "\n";
            break;
        }
        case ANT::STRING_LIT_NODE: {
            auto *ee = static_cast<const StringExpr *>(e);
            indent(); std::cout << "value: " << ee->value << "\n";
        }
        default:
            break;

    }
}

void Printer::print_two_operands_exprs(const Expr *e) {
    indent(); std::cout << "node type: ";
    switch(e->node_type) {
        case ANT::ASSIGNMENT_EXPR_NODE: {
            auto *ee = static_cast<const AssignmentExpr *>(e);
            std::cout << "AssignmentExpression,\n";
            indent(); std::cout << "operator: \"" << ee->op << "\",\n";

            indent(); std::cout << "target: {\n";
            exprs_printer_helper(ee->target);
            indent(); std::cout << "},\n";

            indent(); std::cout << "value: {\n";
            exprs_printer_helper(ee->value);
            indent(); std::cout << "}\n";
            break;
        }
        case ANT::BINARY_EXPR_NODE: {
            auto *ee = static_cast<const BinaryExpr *>(e);
            std::cout << "BinaryExpression,\n";
            indent(); std::cout << "operator: \"" << ee->op << "\",\n";

            indent(); std::cout << "left: {\n";
            exprs_printer_helper(ee->left);
            indent(); std::cout << "},\n";

            indent(); std::cout << "right: {\n";
            exprs_printer_helper(ee->right);
            indent(); std::cout << "}\n";
            break;
        }
        default:
            break;
    }
}

void Printer::printDeclaration(const Decl *d) {
    switch(d->node_type) {
        case ANT::VAR_DECL_NODE: {
            auto *dd = static_cast<const VariableDecl *>(d);
            indent(); std::cout << "node type: VariableDeclaration,\n";
            indent(); std::cout << "type: " << dd->variable_type.type_name << ",\n";
            indent(); std::cout << "const: ";
            dd->variable_type.has_const ? (std::cout << "true,\n") : (std::cout << "false,\n");
            indent(); std::cout << "identifiers: [\n";
            nspace += tab_length;
            for(VariableDeclarator *vd : dd->declarations) {
                indent(); std::cout << "name: " << vd->variable_name << ",\n";
                indent(); std::cout << "init: ";
                if(!vd->initializer) {
                    std::cout << "null\n";
                }
                else {
                    std::cout << "{\n";
                    nspace += tab_length;
                    printExpression(vd->initializer);
                    nspace -= tab_length;
                    indent(); std::cout << "}\n";
                }
            }
            nspace -= tab_length;
            indent(); std::cout << "]\n";
            break;

        }
        case ANT::FUNC_DECL_NODE: {
            auto *dd = static_cast<const FunctionDecl *>(d);
            indent(); std::cout << "node type: FunctionDeclaration,\n";
            indent(); std::cout << "name: " << dd->function_name << ",\n";
            indent(); std::cout << "return type: " << dd->return_type.type_name << ",\n";
            indent(); std::cout << "const: ";
            dd->return_type.has_const ? (std::cout << "true,\n") : (std::cout << "false,\n");
            indent(); std::cout << "parameters: [\n";
            nspace += tab_length;
            for(Parameter *p : dd->parameters) {
                indent(); std::cout << "identifier: {\n";
                nspace += tab_length;
                indent(); std::cout << "name: " << p->parameter_name << ",\n";
                indent(); std::cout << "type: " << p->type_name.type_name<< ",\n";
                indent(); std::cout << "const: ";
                p->type_name.has_const ? (std::cout << "true,\n") : (std::cout << "false,\n");
                indent(); std::cout << "default value: ";
                if(p->default_value) {
                    std::cout << "{\n";
                    exprs_printer_helper(p->default_value);
                    indent(); std::cout << "}\n";
                }
                else std::cout << "null\n";
                nspace -= tab_length;
                indent(); std::cout << "},\n";
            }
            nspace -= tab_length;
            indent(); std::cout << "],\n";
            indent(); std::cout << "body: ";
            if(dd->body) {
                std::cout << "{\n";
                stmts_printer_helper(dd->body);
                indent(); std::cout << "}\n";
            }
            else std::cout << "null\n";
            break;
        }
        default:
            break;
    }
}

void Printer::printStatement(const Stmt *s) {
    switch(s->node_type) {
        case ANT::COMP_STMT_NODE: {
            auto *ss = static_cast<const CompoundStmt *>(s);
            indent(); std::cout << "node type: CompoundStatement,\n";
            indent(); std::cout << "body: [\n";
            nspace += tab_length;
            for(Stmt *st : ss->statements) {
                printStatement(st);
//                std::cout << ",\n";
            }

            nspace -= tab_length;
            indent(); std::cout << "]\n";
            break;
        }
        case ANT::IF_STMT_NODE: {
            auto *ss = static_cast<const IfStmt *>(s);
            indent(); std::cout << "node type: IfStatement,\n";
            indent(); std::cout << "test: {\n";
            exprs_printer_helper(ss->condition);
            indent(); std::cout << "},\n";
            indent(); std::cout << "then: {\n";
            stmts_printer_helper(ss->then_statement);
            if(ss->else_statement) {
                indent(); std::cout << "},\n";
                indent(); std::cout << "else: {\n";
                stmts_printer_helper(ss->else_statement);
                indent(); std::cout << "}\n";
            }
            else {
                indent(); std::cout << "}\n";
            }
            break;
        }
        case ANT::SWITCH_STMT_NODE: {
            auto *ss = static_cast<const SwitchStmt *>(s);
            indent(); std::cout << "node type: SwitchStatement,\n";
            indent(); std::cout << "pattern: {\n";
            exprs_printer_helper(ss->pattern);
            indent(); std::cout << "},\n";
            indent(); std::cout << "cases: [\n";
            nspace += tab_length;
            for(CaseClause *c : ss->cases) {
                indent(); std::cout << "test: {\n";
                exprs_printer_helper(c->expression);
                indent(); std::cout << "}\n";
                indent(); std::cout << "then: {\n";
                stmts_printer_helper(c->body);
                indent(); std::cout << "}\n,";
            }
            nspace -= tab_length;
            indent(); std::cout << "]\n";
            break;
        }
        case ANT::WHILE_STMT_NODE: {
            auto *ss = static_cast<const WhileStmt *>(s);
            indent(); std::cout << "node type: WhileStatement,\n";
            indent(); std::cout << "test: {\n";
            exprs_printer_helper(ss->condition);
            indent(); std::cout << "},\n";
            indent(); std::cout << "body: [\n";
            stmts_printer_helper(ss->body);
            indent(); std::cout << "]\n";
            break;
        }
        case ANT::DO_WHILE_STMT_NODE: {
            auto *ss = static_cast<const DoWhileStmt *>(s);
            indent(); std::cout << "node type: DoWhileStatement,\n";
            indent(); std::cout << "body: [\n";
            stmts_printer_helper(ss->body);
            indent(); std::cout << "],\n";
            indent(); std::cout << "test: {\n";
            exprs_printer_helper(ss->condition);
            indent(); std::cout << "}\n";
            break;
        }
        case ANT::RETURN_STMT_NODE: {
            auto *ss = static_cast<const ReturnStmt *>(s);
            indent(); std::cout << "node type: ReturnStatement,\n";
            indent(); std::cout << "expression: ";
            if(!ss->expression) std::cout << "null\n";
            else {
                std::cout << "{\n";
                exprs_printer_helper(ss->expression);
                indent(); std::cout << "}\n";
            }
            break;
        }
        case ANT::DECL_STMT_NODE: {
            auto *ss = static_cast<const DeclarationStmt *>(s);
            indent(); std::cout << "node type: DeclarationStatement,\n";
            indent(); std::cout << "declarations: {\n";
            nspace += tab_length;
            printDeclaration(ss->declaration);
            nspace -= tab_length;
            indent(); std::cout << "}\n";
            break;
        }
        default: {
            auto *ss = static_cast<const ExpressionStmt *>(s);
            indent(); std::cout << "node type: ExpressionStatement,\n";
            indent(); std::cout << "expression: {\n";
            exprs_printer_helper(ss->expression);
            indent(); std::cout << "}\n";
            break;
        }
    }
}

void Printer::exprs_printer_helper(Expr *e) {
    nspace += tab_length;
    printExpression(e);
    nspace -= tab_length;
}

void Printer::stmts_printer_helper(Stmt *s) {
    nspace += tab_length;
    printStatement(s);
    nspace -= tab_length;
}
