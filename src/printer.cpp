#include "../include/printer.h"

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
    indent(); std::cout << "node type: IntegerLiteral,\n";
    indent(); std::cout << "value: " << e.value << "\n";
}

void Printer::visit(DecimalNumberExpr& e) {
    indent(); std::cout << "node type: DecimalLiteral,\n";
    indent(); std::cout << "value: " << e.value << "\n";
}

void Printer::visit(StringExpr& e) {
    indent(); std::cout << "node type: StringLiteral,\n";
    indent(); std::cout << "value: " << e.value << "\n";
}

void Printer::visit(BoolExpr& e) {
    indent(); std::cout << "node type: BooleanLiteral,\n";
    indent(); std::cout << "value: " << (e.value ? "true\n" : "false\n");
}

void Printer::visit(IdentifierExpr& e) {
    indent(); std::cout << "node type: Identifier,\n";
    indent(); std::cout << "name: \"" << e.name.value << "\"\n";
}

void Printer::visit(BinaryExpr& e) {
    indent(); std::cout << "node type: BinaryExpression,\n";
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
    indent(); std::cout << "node type: AssignmentExpression,\n";
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
void Printer::visit(VariableDecl& d) {
    indent(); std::cout << "node type: VariableDeclaration,\n";
    indent(); std::cout << "type: " << d.declared_type.type_name << ",\n";
//    indent(); std::cout << "const: ";
//    d.declared_type.is_constant ? (std::cout << "true,\n") : (std::cout << "false,\n");
    indent(); std::cout << "identifiers: [\n";
    nspace += tab_length;
    for(const VariableDeclarator *vd : d.declarations) {
        indent(); std::cout << "name: " << vd->variable_name << ",\n";
        indent(); std::cout << "init: ";
        if(!vd->initializer) std::cout << "null\n";
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
//    indent(); std::cout << "const: ";
//    d.return_type.is_constant ? (std::cout << "true,\n") : (std::cout << "false,\n");
    indent(); std::cout << "parameters: ";
    if(d.parameters.empty()) std::cout << "[],\n";
    else {
        std::cout << "[\n";
        nspace += tab_length;
        for(const Parameter *p : d.parameters) {
            indent(); std::cout << "identifier: {\n";
            nspace += tab_length;
            indent(); std::cout << "name: " << p->parameter_name << ",\n";
            indent(); std::cout << "type: " << p->type_name.type_name<< ",\n";
            indent(); std::cout << "const: ";
//            p->type_name.is_constant ? (std::cout << "true,\n") : (std::cout << "false,\n");
//            indent(); std::cout << "default value: ";
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







//
void Printer::visit(CompoundStmt& s) {
    indent(); std::cout << "node type: CompoundStatement,\n";
    indent(); std::cout << "body: ";
    if(s.statements.empty()) std::cout << "[]\n";
    else {
        std::cout << "[\n";
        for(Stmt *st : s.statements)
            stmts_printer_helper(st);

        indent(); std::cout << "]\n";
    }
}

void Printer::visit(ExpressionStmt& s) {
    indent(); std::cout << "node type: ExpressionStatement,\n";
    indent(); std::cout << "expression: ";
    if(s.expression) {
        std::cout << "{\n";
        exprs_printer_helper(s.expression);
        indent(); std::cout << "}\n";
    }
    else std::cout << "null\n";
}

void Printer::visit(DeclarationStmt& s) {
//            indent(); std::cout << "node type: DeclarationStatement,\n";
//            indent(); std::cout << "declarations: {\n";
//            nspace += tab_length;

    s.declaration->accept(*this);
//    printDeclaration(d.declaration);

//            nspace -= tab_length;
//            indent(); std::cout << "}\n";
}

void Printer::visit(IfStmt& s) {
    indent(); std::cout << "node type: IfStatement,\n";
    indent(); std::cout << "test: {\n";
    exprs_printer_helper(s.condition);
    indent(); std::cout << "},\n";
    indent(); std::cout << "then: {\n";
    stmts_printer_helper(s.then_statement);
    if(!s.else_statement) {
        indent(); std::cout << "}\n";
    }
    else {
        indent(); std::cout << "},\n";
        indent(); std::cout << "else: {\n";
        stmts_printer_helper(s.else_statement);
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

void Printer::visit(WhileStmt& s) {
    indent(); std::cout << "node type: WhileStatement,\n";
    indent(); std::cout << "test: {\n";
    exprs_printer_helper(s.condition);
    indent(); std::cout << "},\n";
    indent(); std::cout << "body: [\n";
    stmts_printer_helper(s.body);
    indent(); std::cout << "]\n";
}

void Printer::visit(DoWhileStmt& s) {
    indent(); std::cout << "node type: DoWhileStatement,\n";
    indent(); std::cout << "body: ";
    if(!s.body) std::cout << "[],\n";
    else {
        std::cout << "[\n";
        stmts_printer_helper(s.body);
        indent(); std::cout << "],\n";
    }
    indent(); std::cout << "test: {\n";
    exprs_printer_helper(s.condition);
    indent(); std::cout << "}\n";
}

void Printer::visit(ForStmt& s) {
    indent(); std::cout << "node type: ForStatement,\n";
    indent(); std::cout << "initialization: ";
    if(!s.initialization) std::cout << "null,\n";
    else {
        std::cout << "{\n";
        stmts_printer_helper(s.initialization);
        indent(); std::cout << "},\n";
    }
    indent(); std::cout << "condition: ";
    if(!s.condition) std::cout << "null,\n";
    else {
        std::cout << "{\n";
        exprs_printer_helper(s.condition);
        indent(); std::cout << "},\n";
    }
    indent(); std::cout << "increment: ";
    if(!s.increment) std::cout << "null,\n";
    else {
        std::cout << "{\n";
        exprs_printer_helper(s.increment);
        indent(); std::cout << "},\n";
    }
    indent(); std::cout << "body: [\n";
    stmts_printer_helper(s.body);
    indent(); std::cout << "]\n";
}

//void Printer::visit(RangeForStmt& s) {
//    indent(); std::cout << "node type: RangeForStatement,\n";
//    indent(); std::cout << "initialization: ";
//    if(!s.item) std::cout << "null,\n";
//    else {
//        std::cout << "{\n";
//        s.item->accept(*this);
//        indent(); std::cout << "},\n";
//    }
//    indent(); std::cout << "range: ";
//    if(!s.range_initializer) std::cout << "null,\n";
//    else {
//        std::cout << "{\n";
//        exprs_printer_helper(s.range_initializer);
//        indent(); std::cout << "},\n";
//    }
//    indent(); std::cout << "body: [\n";
//    stmts_printer_helper(s.body);
//    indent(); std::cout << "]\n";
//}

void Printer::visit(ReturnStmt& s) {
    indent(); std::cout << "node type: ReturnStatement,\n";
    indent(); std::cout << "expression: ";
    if(!s.expression) std::cout << "null\n";
    else {
        std::cout << "{\n";
        exprs_printer_helper(s.expression);
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
