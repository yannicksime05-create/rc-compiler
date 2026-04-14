#include "../include/semantic analyser.h"
//
//std::optional<BuiltinTypes> SemanticAnalyser::lookup_builtin_types(const std::string& name) {
//    if (name == "bool")   return BuiltinTypes::BOOL;
//    if (name == "int")    return BuiltinTypes::INT;
//    if (name == "float")  return BuiltinTypes::FLOAT;
//    if (name == "string") return BuiltinTypes::STRING;
//    if (name == "void")   return BuiltinTypes::VOID;
//
//    return std::nullopt;
//}

void SemanticAnalyser::visit(Program& p) {
    manager.enter(ScopeType::GLOBAL);

    for(Stmt *s : p.statements) {
        if(s) s->accept(*this);
    }

    manager.exit();
}

//void SemanticAnalyser::visit(VariableDecl& d) {
//    Type t = resolve(d.declared_type);
//
//    for(VariableDeclarator *vd : d.declarations) {
//        Symbol *s = new Symbol(vd->variable_name, SymbolType::VARIABLE, t.is_constant, &t, vd);
//        if( !manager.current()->insert(s) )
//            throw SemanticError("Error: Redefinition of variable ''");
//
//        vd->symbol = s;
//    }
//}

//void SemanticAnalyser::visit(FunctionDecl& d) {
//    Type t = resolve(d.return_type);
//    Symbol *f = new Symbol(d.function_name, SymbolType::FUNCTION, t.is_constant, &t, &d);
//    if( !manager.current()->insert(f) )
//        throw SemanticError("Error: Redefinition of function ''");
//
//    d.symbol = f;
//    manager.enter(ScopeType::FUNCTION);
//    for(Parameter *p : d.parameters) {
//        Type pt = resolve(p->type_name);
//        Symbol *s = new Symbol(p->parameter_name, SymbolType::PARAMETER, pt.is_constant, &pt, p);
//        if( !manager.current()->insert(s) )
//            throw SemanticError("Error: Redefinition of parameter ''");
//
//        p->symbol = s;
//    }
//
//    if(d.body)
//        d.body->accept(*this);
//
//    manager.exit();
//}

void SemanticAnalyser::visit(CompoundStmt& s) {
    manager.enter(ScopeType::BLOCK);

    for(Stmt *st : s.statements) {
        if(st)  st->accept(*this);
    }

    manager.exit();
}

//
//Type SemanticAnalyser::resolve(TypeSpecifier& t) {
//    Type result;
//    result.is_constant = t.is_constant;
//
//    if(t.type_name == "auto") {
//        result.kind = TypeKind::AUTO;
//        return result;
//    }
//    else if(t.type_name == "any") {
//        result.kind = TypeKind::ANY;
//        return result;
//    }
//
//    result.kind = TypeKind::UNKNOWN;
//    return result;
//}
//
//void SemanticAnalyser::visit(IdentifierExpr& e) {
//    Symbol *s = manager.lookup(e.name.value);
//    if(!s)
//        throw SemanticError("use of undeclared identifier '" +e.name+ "'");
//
//    e.symbol = s;
//    e.resolved_type = s->declared_type;
//}
//
//void SemanticAnalyser::visit(CallExpr& e) {
//    Symbol *s = manager.lookup(e.);
//    if(!s)
//        throw SemanticError();
//
//
//}
//
//
//
//
//
//
//
//
//void SemanticAnalyser::visit(VariableDecl& d) {
//    Type type = resolve(d.declared_type);
//    if(type.kind == TypeKind::UNKNOWN)
//        throw SemanticError("Error: Unknown type: '" +d.declared_type.type_name+ "'!");
//
//    for(VariableDeclarator *vd : d.declarations) {
//        Symbol *s = new Symbol(vd->variable_name, SymbolType::VARIABLE, type.is_constant, &type, &d);
//        if( !manager.current()->insert(s) )
//            throw SemanticError("Error: Redifinition of '" +vd->variable_name+ "'!" );
//
//
//        if( (type.kind == TypeKind::AUTO || type.kind == TypeKind::ANY) && !vd->initializer ) {
//
//        }
//
//        if(vd->initializer) {
//            vd->initializer->accept(*this);
//        }
//    }
//}
//
//void SemanticAnalyser::visit(FunctionDecl& d) {
//    Type t = resolve(d.return_type);
//    Symbol *f = new Symbol(d.function_name, SymbolType::FUNCTION, t.is_constant, &t, &d);
//    if( !manager.current()->insert(f) ) {
//        throw SemanticError("Error: Redifinition of function '" +d.function_name+ "'!");
//    }
//
//    d.symbol = f;
//
//    manager.enter(ScopeType::FUNCTION);
//    for(const Parameter *p : d.parameters) {
//        Symbol *s = new Symbol(p->parameter_name, SymbolType::PARAMETER, p->type_name.is_constant);
//        if( !manager.current()->insert(s) ) {
//            throw SemanticError("duplicate parameter name '" + p->parameter_name + "'");
//        }
//    }
//
//    if(d.body) d.body->accept(*this);
//
//    manager.exit();
//}
//
//
//
//
//
//
//
//
//void SemanticAnalyser::visit(CompoundStmt& s) {
//    manager.enter(ScopeType::BLOCK);
//    for(Stmt *st : s.statements) {
//        if(st)
//            st->accept(*this);
//    }
//
//    manager.exit();
//}
//
//void SemanticAnalyser::visit(ExpressionStmt& s) {
//    s.expression->accept(*this);
//}
//
//void SemanticAnalyser::visit(DeclarationStmt& s) {
//    s.declaration->accept(*this);
//}
//
//void SemanticAnalyser::visit(IfStmt& s) {
//
//}
//
//void SemanticAnalyser::visit(ReturnStmt& s) {
//    if(manager.current()->get_type() != ScopeType::FUNCTION)
//        throw SemanticError("return statement outside function");
//
//
//}
