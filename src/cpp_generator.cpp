#include "../include/cpp_generator.h"

void CppGenerator::visit(Program& p) {
    for(Stmt *s : p.statements) {
        if(s) s->accept(*this);
    }
}







void CppGenerator::visit(BoolExpr& e) {
    out << (e.value ? "true" : "false");
}

void CppGenerator::visit(IntNumberExpr& e) {
    out << e.value;
}

void CppGenerator::visit(DecimalNumberExpr& e) {
    out << e.value;
}

void CppGenerator::visit(StringExpr& e) {
    out << e.value;
}

void CppGenerator::visit(ArrayLiteralExpr& e) {
    out << "{";
    for(Expr *expr : e.elements) {
        if(expr) expr->accept(*this);
    }
    out << "}";
}

void CppGenerator::visit(IdentifierExpr& e) {
    out << e.name.value;
}

void CppGenerator::visit(BinaryExpr& e) {
    e.left->accept(*this);
    out << " " << e.op.value << " ";
    e.right->accept(*this);
}

void CppGenerator::visit(UnaryExpr& e) {
    if(e.is_prefix) {
        out << e.op.value;
        e.expr->accept(*this);
    }
    else {
        e.expr->accept(*this);
        out << e.op.value;
    }
}

void CppGenerator::visit(AssignmentExpr& e) {
    e.target->accept(*this);
    out << " " << e.op.value << " ";
    e.value->accept(*this);
}

void CppGenerator::visit(ConditionalExpr& e) {
    out << "(";
    e.condition->accept(*this);
    out << " ? ";
    e.if_true->accept(*this);
    out << " : ";
    e.if_false->accept(*this);
    out << ")";
}

void CppGenerator::visit(CallExpr& e) {
    e.callee->accept(*this);
    out << "(";
    for(size_t i = 0; i < e.arguments.size(); ++i) {
        e.arguments[i]->accept(*this);
        if(i + 1 < e.arguments.size()) out << ", ";
    }
    out << ")";
}

void CppGenerator::visit(MemberAccessExpr& e) {}

void CppGenerator::visit(SubscriptExpr& e) {}

void CppGenerator::visit(SequenceExpr& e) {
    out << "(";
    for(size_t i = 0; i < e.expressions.size(); ++i) {
        e.expressions[i]->accept(*this);
        if(i + 1 < e.expressions.size()) out << ", ";
    }
    out << ")";
}







void CppGenerator::visit(VariableDecl& d) {
    out << map_type(d.declared_type) << " ";
    for(size_t i = 0; i < d.declarations.size(); ++i) {
        VariableDeclarator *vd = d.declarations[i];

        out << vd->variable_name.value;
        if(vd->initializer) {
            out << " = ";
            vd->initializer->accept(*this);
        }

        if(i + 1 < d.declarations.size()) out << ", ";
    }
    out << ";\n";
}

void CppGenerator::visit(FunctionDecl& d) {
    out << map_type(d.return_type) << " " << d.function_name.value << "(";
    for(size_t i = 0; i < d.parameters.size(); ++i) {
        Parameter *p = d.parameters[i];

        out << map_type(p->type_name) << " " << p->parameter_name.value;
        if(p->default_value) {
            out << " = ";
            p->default_value->accept(*this);
        }

        if(i + 1 < d.parameters.size()) out << ", ";
    }
    out << ") ";

    if(d.body) d.body->accept(*this);
    else out << "{}\n";

    out << "\n";
}






void CppGenerator::visit(CompoundStmt& s) {
    out << "{\n";
    indent_level += TAB;
    for(Stmt *st : s.statements) {
        indent();
        if(st) st->accept(*this);
    }
    indent_level -= TAB;
    indent(); out << "}\n";
}

void CppGenerator::visit(ExpressionStmt& s) {
    if(s.expression) s.expression->accept(*this);
    out << ";\n";
}

void CppGenerator::visit(DeclarationStmt& s) {
    Decl *d = s.declaration;

    bool is_var_decl = d->node_type == ASTNodeType::VAR_DECL_NODE;
    d->accept(*this);

//    if(is_var_decl) out << ";\n";
}

void CppGenerator::visit(IfStmt& s) {
    out << "if(";
    s.condition->accept(*this);
    out << ") ";
    s.then_statement->accept(*this);
    if(s.else_statement) {
        indent(); out << "else ";
        s.else_statement->accept(*this);
    }
}

void CppGenerator::visit(SwitchStmt& s) {
    out << "switch(";
    s.pattern->accept(*this);
    out << ") {\n";
    indent_level += TAB;
    for(CaseClause *c : s.cases) {
        indent();
        if(c->expression) {
            out << "case ";
            c->expression->accept(*this);
            out << ":";
        }
        else {
            out << "default:";
        }

        c->body->accept(*this);
    }
    indent_level -= TAB;
    out << "}\n";
}

void CppGenerator::visit(WhileStmt& s) {
    out << "while(";
    s.condition->accept(*this);
    out << ") ";
    s.body->accept(*this);
}

void CppGenerator::visit(DoWhileStmt& s) {
    out << "do(";
    s.body->accept(*this);
    out << "while(";
    s.condition->accept(*this);
    out << ");";
}

void CppGenerator::visit(ForStmt& s) {
    out << "for(";
    //This is either an ExpressionStmt or a DeclarationStmt containing a VariableDecl and in both cases, the semicolon is automatically added, so no need to it here again.
    if(s.initialization) s.initialization->accept(*this);
    if(s.condition) {
        out << " ";
        s.condition->accept(*this);
    }
    //expressinons don't add semicolon on their own so we manually add it.
    out << ";";
    if(s.increment) {
        out << " ";
        s.increment->accept(*this);
    }
    out << ") ";
    s.body->accept(*this);
}

void CppGenerator::visit(RangeForStmt& s) {}

void CppGenerator::visit(ReturnStmt& s) {
    out << "return";
    if(s.expression) {
        out << " ";
        s.expression->accept(*this);
    }
    out << ";\n";
}

void CppGenerator::visit(PrintStmt& s) {
    if(s.expressions.empty()) {
        out << "std::cout << \"\";\n";
        return;
    }

    out << "std::cout";
    if(!s.has_fmt) {
        for(Expr *e : s.expressions) {
            out << " << ";
            if(e) e->accept(*this);
        }

        out << ";\n";
        return;
    }

    const std::string& fmt = static_cast<StringExpr*>(s.expressions[0])->value;
    size_t start = 0, end = fmt.find("{}", start), i = 1;
    int nb_placeholders = s.nb_placeholders;


    while(nb_placeholders) {
        out << " << \"" << fmt.substr(start, end - start) << "\" << ";
        if(s.expressions[i]) s.expressions[i]->accept(*this);

        start = end + 2;
        end = fmt.find("{}", start);
        --nb_placeholders;
        ++i;
    }
    out << " << \"" << fmt.substr(start) << "\"";

    out << ";\n";
}
