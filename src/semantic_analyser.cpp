#include "../include/semantic_analyser.h"

using TT = TokenType;
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

//For now it checks only int, but later, we'll add short, long, ...
bool SemanticAnalyser::is_integral_type(const Type *t) {
    if(!t) return false;

    return t->kind == TypeKind::BUILTIN && t->builtin == BuiltinTypes::INT;
}

//Same as is_integral_type, but for float, double and long double.
bool SemanticAnalyser::is_floating_type(const Type *t) {
    if(!t) return false;

    return t->kind == TypeKind::BUILTIN && t->builtin == BuiltinTypes::FLOAT;
}

bool SemanticAnalyser::is_numeric_type(const Type *t) {
    return is_integral_type(t) || is_floating_type(t);
}

bool SemanticAnalyser::is_string_type(const Type *t) {
    return t && t->kind == TypeKind::BUILTIN && t->builtin == BuiltinTypes::STRING;
}

bool SemanticAnalyser::is_bool_type(const Type *t) {
    return t && t->kind == TypeKind::BUILTIN && t->builtin == BuiltinTypes::BOOL;
}

const char *SemanticAnalyser::type_to_string(const Type *t) {
    switch(t->builtin) {
        case BuiltinTypes::BOOL:    return "bool";
        case BuiltinTypes::FLOAT:   return "float";
        case BuiltinTypes::INT:     return "int";
        case BuiltinTypes::STRING:  return "string";
        case BuiltinTypes::VOID:    return "void";
    }

    return "unknown";
}

Type *SemanticAnalyser::promote(const Type *left, const Type *right) {
    if(!is_numeric_type(left) || !is_numeric_type(right)) return nullptr;

    if(left->builtin == BuiltinTypes::FLOAT || right->builtin == BuiltinTypes::FLOAT)
        return new Type(TypeKind::BUILTIN, BuiltinTypes::FLOAT);

    return new Type(TypeKind::BUILTIN, BuiltinTypes::INT);
}

std::string SemanticAnalyser::type_mismatch(const Type *lt, Token& op, const Type *rt) {
    std::stringstream ss;
    ss << "Operands' types mismatch for operator '" << op.value << "' at line: " << op.start.line
    << ".\n Left is: " << type_to_string(lt) << ", Right is: " << type_to_string(rt) << ".\n";

    return ss.str();
}

Type SemanticAnalyser::resolve(TypeSpecifier& t) {
    Type result;
    result.is_constant = !t.qualifiers.empty();

    if(t.type_name == "any")    { result.kind = TypeKind::ANY;  return result; }
    if(t.type_name == "auto")   { result.kind = TypeKind::AUTO; return result; }
    if(t.type_name == "bool")   { result.kind = TypeKind::BUILTIN; result.builtin = BuiltinTypes::BOOL;     return result; }
    if(t.type_name == "int")    { result.kind = TypeKind::BUILTIN; result.builtin = BuiltinTypes::INT;      return result; }
    if(t.type_name == "float"
    || t.type_name == "double") { result.kind = TypeKind::BUILTIN; result.builtin = BuiltinTypes::FLOAT;    return result; }
    if(t.type_name == "string") { result.kind = TypeKind::BUILTIN; result.builtin = BuiltinTypes::STRING;   return result; }
    if(t.type_name == "void")   { result.kind = TypeKind::BUILTIN; result.builtin = BuiltinTypes::VOID;     return result; }

    result.kind = TypeKind::UNKNOWN;
    return result;
}


void SemanticAnalyser::visit(Program& p) {
    manager.enter(ScopeType::GLOBAL);

    for(Stmt *s : p.statements) {
        if(s) s->accept(*this);
    }

    manager.exit();
}

void SemanticAnalyser::visit(IntNumberExpr& e) {
    e.resolved_type = new Type(TypeKind::BUILTIN, BuiltinTypes::INT);
}

void SemanticAnalyser::visit(DecimalNumberExpr& e) {
    e.resolved_type = new Type(TypeKind::BUILTIN, BuiltinTypes::FLOAT);
}

void SemanticAnalyser::visit(StringExpr& e) {
    e.resolved_type = new Type(TypeKind::BUILTIN, BuiltinTypes::STRING);
}

void SemanticAnalyser::visit(BoolExpr& e) {
    e.resolved_type = new Type(TypeKind::BUILTIN, BuiltinTypes::BOOL);
}

void SemanticAnalyser::visit(IdentifierExpr& e) {
    Symbol *s = manager.lookup(e.name.value);
    if(!s) {
        ss.str("");
        ss << "Used of undefined variables. Line: " << e.name.start.line << ", Col: " << e.name.start.col << ".\n";
        throw SemanticError(ss.str());
    }

    e.symbol = s;
    e.resolved_type = s->declared_type;
}

void SemanticAnalyser::visit(BinaryExpr& e) {
    if(e.left)  e.left->accept(*this);
    if(e.right) e.right->accept(*this);

    const Type *lt = e.left->resolved_type;
    const Type *rt = e.right->resolved_type;

    //op: +
    //if int and int,          type = int
    //if float and float,      type = float
    //if float and int,        type = float
    //if string and string,    type = string
    //iff string and int,      type = string
    //iff string and float,    type = string

    //op: -
    //if int and int,       type = int
    //if float and float,   type = float
    //if float and int,     type = float
    //if string and string, type = string

    //op: *
    //if int and int,        type = int
    //if float and float,    type = float
    //if float and int,      type = float
    //iff string and int,    type = string

    //op: /
    //if int and int,        type = int
    //if float and float,    type = float
    //if float and int,      type = float
    //if string and string,  type = array of strings
    //iff string and int,    type = array of strings

    //op: %
    //if int and int,        type = int

//    if(!lt || !rt)

    switch(e.op.type) {
        // --- Arithmetic: + - * / %
        case TT::PLUS: {
            if( is_string_type(lt) && ( is_string_type(rt) || is_numeric_type(rt) ) ) {
                e.resolved_type = new Type(TypeKind::BUILTIN, BuiltinTypes::STRING);
                break;
            }

            Type *result = promote(lt, rt);
            if(!result) throw SemanticError(type_mismatch(lt, e.op, rt));

            e.resolved_type = result;
            break;
        }
        case TT::MINUS: {
            if(is_string_type(lt) && is_string_type(rt)) {
                e.resolved_type = new Type(TypeKind::BUILTIN, BuiltinTypes::STRING);
                break;
            }

            Type *result = promote(lt, rt);
            if(!result) throw SemanticError(type_mismatch(lt, e.op, rt));

            e.resolved_type = result;
            break;
        }
        case TT::STAR: {
//            if(is_string_type(lt) && !is_integral_type(rt)) throw SemanticError(type_mismatch(lt, e.op, rt));

            if(is_string_type(lt) && is_integral_type(rt)) {
                e.resolved_type = new Type(TypeKind::BUILTIN, BuiltinTypes::STRING);
                break;
            }

            Type *result = promote(lt, rt);
            if(!result) throw SemanticError(type_mismatch(lt, e.op, rt));

            e.resolved_type = result;
            break;
        }
        case TT::SLASH: {
            //e.resolved_type = array
            if(is_string_type(lt) && (is_string_type(rt) || is_integral_type(rt))) {
                e.resolved_type = new Type(TypeKind::ARRAY, lt);
                break;
            }

            Type *result = promote(lt, rt);
            if(!result) throw SemanticError(type_mismatch(lt, e.op, rt));

            e.resolved_type = result;
            break;
        }
        case TT::MOD: {
            if(!is_integral_type(lt) || !is_integral_type(rt)) {
                ss.str("");
                ss << "Operator: '" << e.op.value << "' requires integral type operands, found '"
                << type_to_string(lt) << "' and '" << type_to_string(rt) << "' instead. Line: " << e.op.start.line << ".";
                throw SemanticError(ss.str());
            }

            e.resolved_type = new Type(TypeKind::BUILTIN, BuiltinTypes::INT);
            break;
        }

        // --- Comparison: < <= > >= == !=
        case TT::LESS:
        case TT::LESS_EQUAL:
        case TT::GREATER:
        case TT::GREATER_EQUAL: {
            if(is_numeric_type(lt) && is_numeric_type(rt)) {
                e.resolved_type = new Type(TypeKind::BUILTIN, BuiltinTypes::BOOL);
                break;
            }

            throw SemanticError(type_mismatch(lt, e.op, rt));
        }
        case TT::EQUAL:
        case TT::NOT_EQUAL: {
            bool valid =    (is_bool_type(lt) && is_bool_type(rt))          ||
                            (is_string_type(lt) && is_string_type(rt))      ||
                            (is_numeric_type(lt) && is_numeric_type(rt));

            if(!valid)  throw SemanticError(type_mismatch(lt, e.op, rt));

            e.resolved_type = new Type(TypeKind::BUILTIN, BuiltinTypes::BOOL);
            break;
        }

        // --- Logical: && ||
        case TT::AND:
        case TT::OR: {
            if(!is_bool_type(lt) || !is_bool_type(rt)) {
                ss.str("");
                ss << "Operator: '" << e.op.value << "' requires boolean type operands, found '"
                << type_to_string(lt) << "' and '" << type_to_string(rt) << "' instead. Line: " << e.op.start.line << ".";
                throw SemanticError(ss.str());
            }

            e.resolved_type = new Type(TypeKind::BUILTIN, BuiltinTypes::BOOL);
            break;
        }

        // --- Bitwise: & | ^ << >>
        case TT::BIT_AND:
        case TT::BIT_OR:
        case TT::BIT_XOR:
        case TT::LEFT_SHIFT:
        case TT::RIGHT_SHIFT: {
            if(!is_integral_type(lt) || !is_integral_type(rt)) {
                ss.str("");
                ss << "Operator: '" << e.op.value << "' requires operands of type 'int', found '"
                << type_to_string(lt) << "' and '" << type_to_string(rt) << "' instead. Line: " << e.op.start.line << ".";
                throw SemanticError(ss.str());
            }

            e.resolved_type = new Type(TypeKind::BUILTIN, BuiltinTypes::INT);
            break;
        }
        default:
            ss.str("");
            ss << "'" << e.op.value << "' is not a binary operator. Found at line: " << e.op.start.line << ".";
            throw SemanticError(ss.str());
    }

}

void SemanticAnalyser::visit(UnaryExpr& e) {
    if(e.expr) e.expr->accept(*this);
    const Type *t = e.expr->resolved_type;

    switch(e.op.type) {
        case TT::PLUS:
        case TT::MINUS: {
            if(!is_numeric_type(t)) {
                ss.str("");
                ss << "Operator: '" << e.op.value << "' requires numeric type operand, found '"
                << type_to_string(t) << "' instead. Line: " << e.op.start.line << ".";
                throw SemanticError(ss.str());
            }

            e.resolved_type = new Type(*t);
            break;
        }
        case TT::NOT: {
            if(!is_bool_type(t)) {
                ss.str("");
                ss << "Operator: '" << e.op.value << "' requires boolean type operand, found '"
                << type_to_string(t) << "' instead. Line: " << e.op.start.line << ".";
                throw SemanticError(ss.str());
            }

            e.resolved_type = new Type(*t);
            break;
        }
        case TT::INCREMENT:
        case TT::DECREMENT: {
            if(!is_numeric_type(t)) {
                ss.str("");
                ss << "Operator: '" << e.op.value << "' requires numeric type operand, found '"
                << type_to_string(t) << "' instead. Line: " << e.op.start.line << ".";
                throw SemanticError(ss.str());
            }

            e.resolved_type = new Type(*t);
            break;
        }
        case TT::BIT_NOT: {
            if(!is_integral_type(t)) {
                ss.str("");
                ss << "Operator: '" << e.op.value << "' requires integral type operand, found '"
                << type_to_string(t) << "' instead. Line: " << e.op.start.line << ".";
                throw SemanticError(ss.str());
            }

            e.resolved_type = new Type(*t);
            break;
        }
        default:
            ss.str("");
            ss << "'" << e.op.value << "' is not an unary operator. Found at line: " << e.op.start.line << ".";
            throw SemanticError(ss.str());
    }
}

void SemanticAnalyser::visit(AssignmentExpr& e) {
    if(e.target) e.target->accept(*this);
    if(e.value)  e.value->accept(*this);

    const Type *tt = e.target->resolved_type;
    const Type *vt = e.value->resolved_type;

//    if(!tt) {
//        tt = new Type(TypeKind::UNKNOWN)
//    }

    switch(e.op.type) {
        case TT::ASSIGN: {
            if( (is_string_type(tt) && !is_string_type(vt)) ||
                (is_numeric_type(tt) && !is_numeric_type(vt))
              ) throw SemanticError(type_mismatch(tt, e.op, vt));

            e.resolved_type = new Type(*tt);
            break;
        }
        case TT::PLUS_ASSIGN:
        case TT::MINUS_ASSIGN:
        case TT::STAR_ASSIGN: {
            bool valid = ( is_numeric_type(tt) && is_numeric_type(vt) )                                 ||
                         ( e.op.type == TT::MINUS_ASSIGN && is_string_type(tt) && is_string_type(vt) )  ||
                         ( e.op.type == TT::STAR_ASSIGN && is_string_type(tt) && is_integral_type(vt) ) ||
                         ( e.op.type == TT::PLUS_ASSIGN && is_string_type(tt) && (is_string_type(vt) || is_numeric_type(vt)) );
            if(valid) {
                e.resolved_type = new Type(*tt);
                break;
            }

            throw SemanticError(type_mismatch(tt, e.op, vt));
        }
        case TT::SLASH_ASSIGN: {
            if(is_string_type(tt) && (is_string_type(vt) || is_integral_type(vt))) {
                e.resolved_type = new Type(TypeKind::ARRAY, tt); //array of strings
                break;
            }

            if(is_numeric_type(tt) && is_numeric_type(vt)) {
                e.resolved_type = new Type(*tt);
                break;
            }

            throw SemanticError(type_mismatch(tt, e.op, vt));
        }
        case TT::MOD_ASSIGN:
        case TT::BIT_OR_ASSIGN:
        case TT::BIT_AND_ASSIGN:
        case TT::BIT_XOR_ASSIGN:
        case TT::LEFT_SHIFT_ASSIGN:
        case TT::RIGHT_SHIFT_ASSIGN: {
            if(is_integral_type(tt) && is_integral_type(vt)) {
                e.resolved_type = new Type(*tt);
                break;
            }

            throw SemanticError(type_mismatch(tt, e.op, vt));
        }

        default:
            ss.str("");
            ss << "'" << e.op.value << "' is not an assignment operator. Found at line: " << e.op.start.line << ".";
            throw SemanticError(ss.str());
//            throw SemanticError("'" +e.op.value+ "' is not an assignment operator. Found at line: " +e.op.start.line+ ".");
    }
}

void SemanticAnalyser::visit(ConditionalExpr& e) {
    if(e.condition) e.condition->accept(*this);
    if(e.if_true)   e.if_true->accept(*this);
    if(e.if_false)  e.if_false->accept(*this);

    const Type *ct = e.condition->resolved_type;
    const Type *itt = e.if_true->resolved_type;
    const Type *ift = e.if_false->resolved_type;

    if(!is_bool_type(ct)) {
        ss.str("");
        ss << "";
        throw SemanticError("");
    }

    if( (!is_string_type(itt) || !is_string_type(ift)) && (!is_numeric_type(itt) || !is_numeric_type(ift)) ) {
        ss.str("");
        ss << "";
        throw SemanticError("");
    }

    e.resolved_type = new Type(*itt);
}

void SemanticAnalyser::visit(CallExpr& e) {
    if(e.callee) e.callee->accept(*this);
    for(Expr *exp : e.arguments) {
        if(exp) exp->accept(*this);
    }


}

void SemanticAnalyser::visit(MemberAccessExpr& e) {
    if(e.object) e.object->accept(*this);
}

void SemanticAnalyser::visit(SubscriptExpr& e) {
    if(e.object) e.object->accept(*this);
    if(e.index)  e.index->accept(*this);

    const Type *obt = e.object->resolved_type;
    const Type *it = e.index->resolved_type;

    if(!is_integral_type(it)) {
        ss.str("");
        ss << "Invalid type for array subscript: ";
        throw SemanticError(ss.str());
    }

}

void SemanticAnalyser::visit(SequenceExpr& e) {
    for(Expr *exp : e.expressions) {
        if(exp) exp->accept(*this);
    }

    e.resolved_type = e.expressions.back()->resolved_type;
}








void SemanticAnalyser::visit(VariableDecl& d) {
    Type *t = new Type(resolve(d.declared_type));

    for(VariableDeclarator *vd : d.declarations) {
        if(vd->initializer) vd->initializer->accept(*this);

        if(manager.lookup_current(vd->variable_name.value)) {
            ss.str("");
            ss << "Error: Redefinition of variable: " << vd->variable_name.value << ". Line: "
            << vd->variable_name.start.line << ", Col: " << vd->variable_name.start.col;
            throw SemanticError(ss.str());
        }

//        if( (t.kind == TypeKind::ANY || t.kind == TypeKind::AUTO) && !vd->initializer )


        Symbol *s = new Symbol(vd->variable_name.value, SymbolType::VARIABLE, t->is_constant, t, &d);
        manager.insert(s);
        vd->symbol = s;
    }
}

void SemanticAnalyser::visit(FunctionDecl& d) {
    if(manager.lookup_current(d.function_name.value)) {
        ss.str("");
        ss << "Error: Redefinition of function: '" << d.function_name.value << "'. Line: " << d.function_name.start.line << ".\n";
        throw SemanticError(ss.str());
    }

    Type *return_type = new Type(resolve(d.return_type));
    Symbol *f = new Symbol(d.function_name.value, SymbolType::FUNCTION, return_type->is_constant, return_type, &d);
    manager.insert(f);
    d.symbol = f;

    manager.enter(ScopeType::FUNCTION);
    for(Parameter *param : d.parameters) {
        if(manager.lookup_current(param->parameter_name.value)) {
            ss.str("");
            ss << "Error: Duplicate parameter name: '" << param->parameter_name.value << "'. Line: " << param->parameter_name.start.line << ".\n";
            throw SemanticError(ss.str());
        }

        Type *param_type = new Type(resolve(param->type_name));
        Symbol *p = new Symbol(param->parameter_name.value, SymbolType::PARAMETER, param_type->is_constant, param_type, &d);
        manager.insert(p);
        param->symbol = p;

        if(param->default_value) param->default_value->accept(*this);
    }

    if(d.body) d.body->accept(*this);
    manager.exit();
}









void SemanticAnalyser::visit(CompoundStmt& s) {
    manager.enter(ScopeType::BLOCK);

    for(Stmt *st : s.statements) {
        if(st) st->accept(*this);
    }

    manager.exit();
}

void SemanticAnalyser::visit(ExpressionStmt& s) {
    s.expression->accept(*this);
}

void SemanticAnalyser::visit(DeclarationStmt& s) {
    s.declaration->accept(*this);
}

void SemanticAnalyser::visit(IfStmt& s) {
    if(s.condition)         s.condition->accept(*this);
    if(s.then_statement)    s.then_statement->accept(*this);
    if(s.else_statement)    s.else_statement->accept(*this);
}

void SemanticAnalyser::visit(SwitchStmt& s) {
    if(s.pattern) s.pattern->accept(*this);
    for(CaseClause *c : s.cases) {
        if(c->expression)   c->expression->accept(*this);
        if(c->body)         c->body->accept(*this);
    }
}

void SemanticAnalyser::visit(WhileStmt& s) {
    if(s.condition) s.condition->accept(*this);
    if(s.body)      s.body->accept(*this);
}

void SemanticAnalyser::visit(DoWhileStmt& s) {
    if(s.body)      s.body->accept(*this);
    if(s.condition) s.condition->accept(*this);
}

void SemanticAnalyser::visit(ForStmt& s) {
    manager.enter(ScopeType::BLOCK);
    if(s.initialization) s.initialization->accept(*this);
    if(s.condition)      s.condition->accept(*this);
    if(s.increment)      s.increment->accept(*this);
    if(s.body)           s.body->accept(*this);
    manager.exit();
}

void SemanticAnalyser::visit(RangeForStmt& s) {
    manager.enter(ScopeType::BLOCK);
    if(s.item)              s.item->accept(*this);
    if(s.range_initializer) s.range_initializer->accept(*this);
    if(s.body)              s.body->accept(*this);
    manager.exit();
}

void SemanticAnalyser::visit(ReturnStmt& s) {
    if(s.expression) s.expression->accept(*this);
}

