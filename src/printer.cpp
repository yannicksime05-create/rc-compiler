#include "../include/printer.h"

//void Printer::printExpression(const Expr *expr) {
//    switch(expr->node_type) {
//        case ANT::INT_LIT_NODE:
//        case ANT::STRING_LIT_NODE:
//        case ANT::DECIMAL_LIT_NODE: {
//            print_literal_exprs(expr);
//            break;
//        }
//        case ANT::IDENTIFIER_EXPR_NODE: {
//            auto *e = static_cast<const IdentifierExpr *>(expr);
//            indent(); std::cout << "node type: Identifier,\n";
//            indent(); std::cout << "name: \"" << e->name.value << "\"\n";
//            break;
//        }
//        case ANT::UNARY_EXP_NODE: {
//            auto *e = static_cast<const UnaryExpr *>(expr);
//            indent(); std::cout << "node type: UnaryExpression,\n";
//            indent(); std::cout << "operator: \"" << e->op << "\",\n";
//            indent(); std::cout << "prefix: ";
//            e->is_prefix ? (std::cout << "true,\n") : (std::cout << "false,\n");
//            indent(); std::cout << "expression: {\n";
//            exprs_printer_helper(e->expr);
//            indent(); std::cout << "}\n";
//            break;
//        }
//        case ANT::BINARY_EXPR_NODE:
//        case ANT::ASSIGNMENT_EXPR_NODE: {
//            print_two_operands_exprs(expr);
//            break;
//        }
//        case ANT::CONDITIONAL_EXPR_NODE: {
//            auto *e = static_cast<const ConditionalExpr *>(expr);
//            indent(); std::cout << "node type: ConditionalExpression,\n";
//            indent(); std::cout << "condition: {\n";
//            exprs_printer_helper(e->condition);
//            indent(); std::cout << "},\n";
//            indent(); std::cout << "then: {\n";
//            exprs_printer_helper(e->if_true);
//            indent(); std::cout << "},\n";
//            indent(); std::cout << "else: {\n";
//            exprs_printer_helper(e->if_false);
//            indent(); std::cout << "}\n";
//            break;
//        }
//        case ANT::CALL_EXPR_NODE: {
//            auto *e = static_cast<const CallExpr *>(expr);
//            indent(); std::cout << "node type: CallExpression,\n";
//            indent(); std::cout << "callee: {\n";
//            exprs_printer_helper(e->callee);
//            indent(); std::cout << "}\n";
//            indent(); std::cout << "arguments: ";
//            if( e->arguments.empty() ) {
//                std::cout << "[]\n";
//            }
//            else {
//                std::cout << "[\n";
//                for(const Expr *exp : e->arguments) {
//                    exprs_printer_helper(exp);
//                }
//                indent(); std::cout << "]\n";
//            }
//            break;
//        }
//        case ANT::MEMBER_ACCESS_EXPR_NODE: {
//            auto *e = static_cast<const MemberAccessExpr *>(expr);
//            indent(); std::cout << "node type: MemberAccessExpression,\n";
//            indent(); std::cout << "object: {\n";
//            exprs_printer_helper(e->object);
//            indent(); std::cout << "}\n";
//            indent(); std::cout << "property: " << e->member << "\n";
////            indent(); std::cout << "}\n";
//            break;
//        }
//        case ANT::SUBSCRIPT_EXPR_NODE: {
//            auto *e = static_cast<const SubscriptExpr *>(expr);
//            indent(); std::cout << "node type: SubscriptExpression,\n";
//            indent(); std::cout << "object: {\n";
//            exprs_printer_helper(e->object);
//            indent(); std::cout << "}\n";
//            indent(); std::cout << "index: {\n";
//            exprs_printer_helper(e->index);
//            indent(); std::cout << "}\n";
//            break;
//        }
//        case ANT::SEQUENCE_EXPR_NODE: {
//            auto *e = static_cast<const SequenceExpr *>(expr);
//            indent(); std::cout << "node type: SequenceExpression,\n";
//            indent(); std::cout << "expressions: ";
//            if(e->expressions.empty())  std::cout << "[]\n";
//            else {
//                std::cout << "[\n";
//                for(const Expr *exp : e->expressions) {
//                    exprs_printer_helper(exp);
//                }
//                indent(); std::cout << "]\n";
//            }
//            break;
//        }
//        default:
//            std::cout << "Unknown expression!" << std::endl;
//            break;
//    }
//}

//void Printer::print_literal_exprs(const Expr *expr) {
//    indent(); std::cout << "node type: Literal,\n";
//
//    switch(expr->node_type) {
//        case ANT::INT_LIT_NODE: {
//            auto *e = static_cast<const IntNumberExpr *>(expr);
//            indent(); std::cout << "value: " << e->value << "\n";
//            break;
//        }
//        case ANT::DECIMAL_LIT_NODE: {
//            auto *e = static_cast<const DecimalNumberExpr *>(expr);
//            indent(); std::cout << "value: " << e->value << "\n";
//            break;
//        }
//        case ANT::STRING_LIT_NODE: {
//            auto *e = static_cast<const StringExpr *>(expr);
//            indent(); std::cout << "value: " << e->value << "\n";
//        }
//        default:
//            break;
//
//    }
//}

//void Printer::print_two_operands_exprs(const Expr *expr) {
//    indent(); std::cout << "node type: ";
//    switch(expr->node_type) {
//        case ANT::ASSIGNMENT_EXPR_NODE: {
//            auto *e = static_cast<const AssignmentExpr *>(expr);
//            std::cout << "AssignmentExpression,\n";
//            indent(); std::cout << "operator: \"" << e->op << "\",\n";
//
//            indent(); std::cout << "target: {\n";
//            exprs_printer_helper(e->target);
//            indent(); std::cout << "},\n";
//
//            indent(); std::cout << "value: {\n";
//            exprs_printer_helper(e->value);
//            indent(); std::cout << "}\n";
//            break;
//        }
//        case ANT::BINARY_EXPR_NODE: {
//            auto *e = static_cast<const BinaryExpr *>(expr);
//            std::cout << "BinaryExpression,\n";
//            indent(); std::cout << "operator: \"" << e->op << "\",\n";
//
//            indent(); std::cout << "left: {\n";
//            exprs_printer_helper(e->left);
//            indent(); std::cout << "},\n";
//
//            indent(); std::cout << "right: {\n";
//            exprs_printer_helper(e->right);
//            indent(); std::cout << "}\n";
//            break;
//        }
//        default:
//            break;
//    }
//}

void Printer::visit(Program& p) {
    std::cout << "AST:" << std::endl;
    indent(); std::cout << "Program: {\n";
    nspace += tab_length;
    indent(); std::cout << "body: [\n";
    for(Stmt *s : p.statements) {
        stmts_printer_helper(s);
    }
    indent(); std::cout << "]\n";
    nspace -= tab_length;
    indent(); std::cout << "}\n";
}







void Printer::visit(IntNumberExpr& e) {
    indent(); std::cout << "node type: Literal,\n";
    indent(); std::cout << "value: " << e.value << "\n";
}

void Printer::visit(DecimalNumberExpr& e) {
    indent(); std::cout << "node type: Literal,\n";
    indent(); std::cout << "value: " << e.value << "\n";
}

void Printer::visit(StringExpr& e) {
    indent(); std::cout << "node type: Literal,\n";
    indent(); std::cout << "value: " << e.value << "\n";
}

void Printer::visit(IdentifierExpr& e) {
    indent(); std::cout << "node type: Identifier,\n";
    indent(); std::cout << "name: \"" << e.name.value << "\"\n";
}

void Printer::visit(BinaryExpr& e) {
    indent(); std::cout << "node type: ";
    std::cout << "BinaryExpression,\n";
    indent(); std::cout << "operator: \"" << e.op << "\",\n";
    indent(); std::cout << "left: {\n";
    exprs_printer_helper(e.left);
    indent(); std::cout << "},\n";
    indent(); std::cout << "right: {\n";
    exprs_printer_helper(e.right);
    indent(); std::cout << "}\n";
}

void Printer::visit(UnaryExpr& e) {
    indent(); std::cout << "node type: UnaryExpression,\n";
    indent(); std::cout << "operator: \"" << e.op << "\",\n";
    indent(); std::cout << "prefix: ";
    e.is_prefix ? (std::cout << "true,\n") : (std::cout << "false,\n");
    indent(); std::cout << "expression: {\n";
    exprs_printer_helper(e.expr);
    indent(); std::cout << "}\n";
}

void Printer::visit(AssignmentExpr& e) {
    indent(); std::cout << "node type: ";
    std::cout << "AssignmentExpression,\n";
    indent(); std::cout << "operator: \"" << e.op << "\",\n";
    indent(); std::cout << "target: {\n";
    exprs_printer_helper(e.target);
    indent(); std::cout << "},\n";
    indent(); std::cout << "value: {\n";
    exprs_printer_helper(e.value);
    indent(); std::cout << "}\n";
}

void Printer::visit(ConditionalExpr& e) {
    indent(); std::cout << "node type: ConditionalExpression,\n";
    indent(); std::cout << "condition: {\n";
    exprs_printer_helper(e.condition);
    indent(); std::cout << "},\n";
    indent(); std::cout << "then: {\n";
    exprs_printer_helper(e.if_true);
    indent(); std::cout << "},\n";
    indent(); std::cout << "else: {\n";
    exprs_printer_helper(e.if_false);
    indent(); std::cout << "}\n";
}

void Printer::visit(CallExpr& e) {
    indent(); std::cout << "node type: CallExpression,\n";
    indent(); std::cout << "callee: {\n";
    exprs_printer_helper(e.callee);
    indent(); std::cout << "}\n";
    indent(); std::cout << "arguments: ";
    if( e.arguments.empty() ) {
        std::cout << "[]\n";
    }
    else {
        std::cout << "[\n";
        for(Expr *exp : e.arguments) {
            exprs_printer_helper(exp);
        }
        indent(); std::cout << "]\n";
    }
}

void Printer::visit(MemberAccessExpr& e) {
    indent(); std::cout << "node type: MemberAccessExpression,\n";
    indent(); std::cout << "object: {\n";
    exprs_printer_helper(e.object);
    indent(); std::cout << "}\n";
    indent(); std::cout << "property: " << e.member << "\n";
//    indent(); std::cout << "}\n";
}

void Printer::visit(SubscriptExpr& e) {
    indent(); std::cout << "node type: SubscriptExpression,\n";
    indent(); std::cout << "object: {\n";
    exprs_printer_helper(e.object);
    indent(); std::cout << "}\n";
    indent(); std::cout << "index: {\n";
    exprs_printer_helper(e.index);
    indent(); std::cout << "}\n";
}

void Printer::visit(SequenceExpr& e) {
    indent(); std::cout << "node type: SequenceExpression,\n";
    indent(); std::cout << "expressions: ";
    if(e.expressions.empty())  std::cout << "[]\n";
    else {
        std::cout << "[\n";
        for(Expr *exp : e.expressions) {
            exprs_printer_helper(exp);
        }
        indent(); std::cout << "]\n";
    }
}







//
//void Printer::printDeclaration(const Decl *decl) {
//    switch(decl->node_type) {
//        case ANT::VAR_DECL_NODE: {
//            auto *d = static_cast<const VariableDecl *>(decl);
//            indent(); std::cout << "node type: VariableDeclaration,\n";
//            indent(); std::cout << "type: " << d->variable_type.type_name << ",\n";
//            indent(); std::cout << "const: ";
//            d->variable_type.is_constant ? (std::cout << "true,\n") : (std::cout << "false,\n");
//            indent(); std::cout << "identifiers: [\n";
//            nspace += tab_length;
//            for(const VariableDeclarator *vd : d->declarations) {
//                indent(); std::cout << "name: " << vd->variable_name << ",\n";
//                indent(); std::cout << "init: ";
//                if(!vd->initializer) {
//                    std::cout << "null\n";
//                }
//                else {
//                    std::cout << "{\n";
//                    nspace += tab_length;
//                    printExpression(vd->initializer);
//                    nspace -= tab_length;
//                    indent(); std::cout << "}\n";
//                }
//                std::cout << "\n";
//            }
//            nspace -= tab_length;
//            indent(); std::cout << "]\n";
//            break;
//
//        }
//        case ANT::FUNC_DECL_NODE: {
//            auto *d = static_cast<const FunctionDecl *>(decl);
//            indent(); std::cout << "node type: FunctionDeclaration,\n";
//            indent(); std::cout << "name: " << d->function_name << ",\n";
//            indent(); std::cout << "return type: " << d->return_type.type_name << ",\n";
//            indent(); std::cout << "const: ";
//            d->return_type.is_constant ? (std::cout << "true,\n") : (std::cout << "false,\n");
//            indent(); std::cout << "parameters: ";
//            if(d->parameters.empty()) {
//                std::cout << "[],\n";
//            }
//            else {
//                std::cout << "[\n";
//                nspace += tab_length;
//                for(const Parameter *p : d->parameters) {
//                    indent(); std::cout << "identifier: {\n";
//                    nspace += tab_length;
//                    indent(); std::cout << "name: " << p->parameter_name << ",\n";
//                    indent(); std::cout << "type: " << p->type_name.type_name<< ",\n";
//                    indent(); std::cout << "const: ";
//                    p->type_name.is_constant ? (std::cout << "true,\n") : (std::cout << "false,\n");
//                    indent(); std::cout << "default value: ";
//                    if(p->default_value) {
//                        std::cout << "{\n";
//                        exprs_printer_helper(p->default_value);
//                        indent(); std::cout << "}\n";
//                    }
//                    else std::cout << "null\n";
//                    nspace -= tab_length;
//                    indent(); std::cout << "},\n";
//                }
//                nspace -= tab_length;
//                indent(); std::cout << "],\n";
//            }
//            indent(); std::cout << "body: ";
//            if(d->body) {
//                std::cout << "{\n";
//                stmts_printer_helper(d->body);
//                indent(); std::cout << "}\n";
//            }
//            else std::cout << "null\n";
//            break;
//        }
//        default:
//            break;
//    }
//}

void Printer::visit(VariableDecl& d) {
    indent(); std::cout << "node type: VariableDeclaration,\n";
    indent(); std::cout << "type: " << d.variable_type.type_name << ",\n";
    indent(); std::cout << "const: ";
    d.variable_type.is_constant ? (std::cout << "true,\n") : (std::cout << "false,\n");
    indent(); std::cout << "identifiers: [\n";
    nspace += tab_length;
    for(const VariableDeclarator *vd : d.declarations) {
        indent(); std::cout << "name: " << vd->variable_name << ",\n";
        indent(); std::cout << "init: ";
        if(!vd->initializer) {
            std::cout << "null\n";
        }
        else {
            std::cout << "{\n";
            nspace += tab_length;
            vd->initializer->accept(*this);
//            printExpression(vd->initializer);
            nspace -= tab_length;
            indent(); std::cout << "}\n";
        }
        std::cout << "\n";
    }
    nspace -= tab_length;
    indent(); std::cout << "]\n";
}

void Printer::visit(FunctionDecl& d) {
    indent(); std::cout << "node type: FunctionDeclaration,\n";
    indent(); std::cout << "name: " << d.function_name << ",\n";
    indent(); std::cout << "return type: " << d.return_type.type_name << ",\n";
    indent(); std::cout << "const: ";
    d.return_type.is_constant ? (std::cout << "true,\n") : (std::cout << "false,\n");
    indent(); std::cout << "parameters: ";
    if(d.parameters.empty()) {
        std::cout << "[],\n";
    }
    else {
        std::cout << "[\n";
        nspace += tab_length;
        for(const Parameter *p : d.parameters) {
            indent(); std::cout << "identifier: {\n";
            nspace += tab_length;
            indent(); std::cout << "name: " << p->parameter_name << ",\n";
            indent(); std::cout << "type: " << p->type_name.type_name<< ",\n";
            indent(); std::cout << "const: ";
            p->type_name.is_constant ? (std::cout << "true,\n") : (std::cout << "false,\n");
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
    }
    indent(); std::cout << "body: ";
    if(d.body) {
        std::cout << "{\n";
        stmts_printer_helper(d.body);
        indent(); std::cout << "}\n";
    }
    else std::cout << "null\n";
}








//void Printer::printStatement(const Stmt *stmt) {
//    switch(stmt->node_type) {
//        case ANT::COMP_STMT_NODE: {
//            auto *s = static_cast<const CompoundStmt *>(stmt);
//            indent(); std::cout << "node type: CompoundStatement,\n";
//            indent(); std::cout << "body: ";
//            if(s->statements.empty()) std::cout << "[]\n";
//            else {
//                std::cout << "[\n";
//                for(const Stmt *st : s->statements)
//                    stmts_printer_helper(st);
//
//                indent(); std::cout << "]\n";
//            }
//            break;
//        }
//        case ANT::IF_STMT_NODE: {
//            auto *s = static_cast<const IfStmt *>(stmt);
//            indent(); std::cout << "node type: IfStatement,\n";
//            indent(); std::cout << "test: {\n";
//            exprs_printer_helper(s->condition);
//            indent(); std::cout << "},\n";
//            indent(); std::cout << "then: {\n";
//            stmts_printer_helper(s->then_statement);
//            if(s->else_statement) {
//                indent(); std::cout << "},\n";
//                indent(); std::cout << "else: {\n";
//                stmts_printer_helper(s->else_statement);
//                indent(); std::cout << "}\n";
//            }
//            else {
//                indent(); std::cout << "}\n";
//            }
//            break;
//        }
//        case ANT::SWITCH_STMT_NODE: {
//            auto *s = static_cast<const SwitchStmt *>(stmt);
//            indent(); std::cout << "node type: SwitchStatement,\n";
//            indent(); std::cout << "pattern: {\n";
//            exprs_printer_helper(s->pattern);
//            indent(); std::cout << "},\n";
//            indent(); std::cout << "cases: [\n";
//            nspace += tab_length;
//            for(const CaseClause *c : s->cases) {
//                indent(); std::cout << "test: {\n";
//                exprs_printer_helper(c->expression);
//                indent(); std::cout << "}\n";
//                indent(); std::cout << "then: {\n";
//                stmts_printer_helper(c->body);
//                indent(); std::cout << "},\n";
//            }
//            nspace -= tab_length;
//            indent(); std::cout << "]\n";
//            break;
//        }
//        case ANT::WHILE_STMT_NODE: {
//            auto *s = static_cast<const WhileStmt *>(stmt);
//            indent(); std::cout << "node type: WhileStatement,\n";
//            indent(); std::cout << "test: {\n";
//            exprs_printer_helper(s->condition);
//            indent(); std::cout << "},\n";
//            indent(); std::cout << "body: [\n";
//            stmts_printer_helper(s->body);
//            indent(); std::cout << "]\n";
//            break;
//        }
//        case ANT::DO_WHILE_STMT_NODE: {
//            auto *s = static_cast<const DoWhileStmt *>(stmt);
//            indent(); std::cout << "node type: DoWhileStatement,\n";
//            indent(); std::cout << "body: [\n";
//            stmts_printer_helper(s->body);
//            indent(); std::cout << "],\n";
//            indent(); std::cout << "test: {\n";
//            exprs_printer_helper(s->condition);
//            indent(); std::cout << "}\n";
//            break;
//        }
//        case ANT::FOR_STMT_NODE: {
//            auto *s = static_cast<const ForStmt *>(stmt);
//            indent(); std::cout << "node type: ForStatement,\n";
//            indent(); std::cout << "initialization: {\n";
//            stmts_printer_helper(s->initialization);
//            indent(); std::cout << "},\n";
//            indent(); std::cout << "condition: {\n";
//            exprs_printer_helper(s->condition);
//            indent(); std::cout << "},\n";
//            indent(); std::cout << "increment: {\n";
//            exprs_printer_helper(s->increment);
//            indent(); std::cout << "},\n";
//            indent(); std::cout << "body: [\n";
//            stmts_printer_helper(s->body);
//            indent(); std::cout << "]\n";
//            break;
//        }
//        case ANT::RETURN_STMT_NODE: {
//            auto *s = static_cast<const ReturnStmt *>(stmt);
//            indent(); std::cout << "node type: ReturnStatement,\n";
//            indent(); std::cout << "expression: ";
//            if(!s->expression) std::cout << "null\n";
//            else {
//                std::cout << "{\n";
//                exprs_printer_helper(s->expression);
//                indent(); std::cout << "}\n";
//            }
//            break;
//        }
//        case ANT::DECL_STMT_NODE: {
//            auto *s = static_cast<const DeclarationStmt *>(stmt);
////            indent(); std::cout << "node type: DeclarationStatement,\n";
////            indent(); std::cout << "declarations: {\n";
////            nspace += tab_length;
//            printDeclaration(s->declaration);
////            nspace -= tab_length;
////            indent(); std::cout << "}\n";
//            break;
//        }
//        default: {
//            auto *s = static_cast<const ExpressionStmt *>(stmt);
//            indent(); std::cout << "node type: ExpressionStatement,\n";
//            indent(); std::cout << "expression: {\n";
//            exprs_printer_helper(s->expression);
//            indent(); std::cout << "}\n";
//            break;
//        }
//    }
//}

void Printer::visit(CompoundStmt& c) {
    indent(); std::cout << "node type: CompoundStatement,\n";
    indent(); std::cout << "body: ";
    if(c.statements.empty()) std::cout << "[]\n";
    else {
        std::cout << "[\n";
        for(Stmt *st : c.statements)
            stmts_printer_helper(st);

        indent(); std::cout << "]\n";
    }
}

void Printer::visit(ExpressionStmt& e) {
    indent(); std::cout << "node type: ExpressionStatement,\n";
    indent(); std::cout << "expression: {\n";
    exprs_printer_helper(e.expression);
    indent(); std::cout << "}\n";
}

void Printer::visit(DeclarationStmt& d) {
//            indent(); std::cout << "node type: DeclarationStatement,\n";
//            indent(); std::cout << "declarations: {\n";
//            nspace += tab_length;

    d.declaration->accept(*this);
//    printDeclaration(d.declaration);

//            nspace -= tab_length;
//            indent(); std::cout << "}\n";
}

void Printer::visit(IfStmt& i) {
    indent(); std::cout << "node type: IfStatement,\n";
    indent(); std::cout << "test: {\n";
    exprs_printer_helper(i.condition);
    indent(); std::cout << "},\n";
    indent(); std::cout << "then: {\n";
    stmts_printer_helper(i.then_statement);
    if(i.else_statement) {
        indent(); std::cout << "},\n";
        indent(); std::cout << "else: {\n";
        stmts_printer_helper(i.else_statement);
        indent(); std::cout << "}\n";
    }
    else {
        indent(); std::cout << "}\n";
    }
}

void Printer::visit(SwitchStmt& s) {
    indent(); std::cout << "node type: SwitchStatement,\n";
    indent(); std::cout << "pattern: {\n";
    exprs_printer_helper(s.pattern);
    indent(); std::cout << "},\n";
    indent(); std::cout << "cases: [\n";
    nspace += tab_length;
    for(const CaseClause *c : s.cases) {
        indent(); std::cout << "test: {\n";
        exprs_printer_helper(c->expression);
        indent(); std::cout << "}\n";
        indent(); std::cout << "then: {\n";
        stmts_printer_helper(c->body);
        indent(); std::cout << "},\n";
    }
    nspace -= tab_length;
    indent(); std::cout << "]\n";
}

void Printer::visit(WhileStmt& w) {
    indent(); std::cout << "node type: WhileStatement,\n";
    indent(); std::cout << "test: {\n";
    exprs_printer_helper(w.condition);
    indent(); std::cout << "},\n";
    indent(); std::cout << "body: [\n";
    stmts_printer_helper(w.body);
    indent(); std::cout << "]\n";
}

void Printer::visit(DoWhileStmt& dw) {
    indent(); std::cout << "node type: DoWhileStatement,\n";
    indent(); std::cout << "body: [\n";
    stmts_printer_helper(dw.body);
    indent(); std::cout << "],\n";
    indent(); std::cout << "test: {\n";
    exprs_printer_helper(dw.condition);
    indent(); std::cout << "}\n";
}

void Printer::visit(ForStmt& f) {
    indent(); std::cout << "node type: ForStatement,\n";
    indent(); std::cout << "initialization: {\n";
    stmts_printer_helper(f.initialization);
    indent(); std::cout << "},\n";
    indent(); std::cout << "condition: {\n";
    exprs_printer_helper(f.condition);
    indent(); std::cout << "},\n";
    indent(); std::cout << "increment: {\n";
    exprs_printer_helper(f.increment);
    indent(); std::cout << "},\n";
    indent(); std::cout << "body: [\n";
    stmts_printer_helper(f.body);
    indent(); std::cout << "]\n";
}

void Printer::visit(ReturnStmt& r) {
    indent(); std::cout << "node type: ReturnStatement,\n";
    indent(); std::cout << "expression: ";
    if(!r.expression) std::cout << "null\n";
    else {
        std::cout << "{\n";
        exprs_printer_helper(r.expression);
        indent(); std::cout << "}\n";
    }
}




void Printer::exprs_printer_helper(Expr *e) {
    nspace += tab_length;
    e->accept(*this);
    nspace -= tab_length;
}

void Printer::stmts_printer_helper(Stmt *s) {
    nspace += tab_length;
    s->accept(*this);
    nspace -= tab_length;
}
