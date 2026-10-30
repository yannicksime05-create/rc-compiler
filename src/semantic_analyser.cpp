#include <sstream>
#include "../include/semantic_analyser.h"

using TT = TokenType;
using TK = TypeKind;
using BT = BuiltinType::Types;

bool SemanticAnalyser::unaryexpr_tiny_constant_folder(Expr *e, long long& v) {
    if(e->node_type == ASTNodeType::INT_LIT_NODE) {
        v = static_cast<IntNumberExpr*>(e)->value;
        return true;
    }

    if(e->node_type == ASTNodeType::UNARY_EXP_NODE) {
        UnaryExpr *tmp = static_cast<UnaryExpr*>(e);

        if(tmp->op.type == TT::MINUS && unaryexpr_tiny_constant_folder(tmp->expr, v)) {
            v = -v;
            return true;
        }
    }

    return false;
}

bool SemanticAnalyser::literal_fits(Expr *value, const Type *in) {
    const BuiltinType *target = static_cast<const BuiltinType*>(in);
    if(!target || !checker.is_numeric(target)) return false;

    long long v;
    if( unaryexpr_tiny_constant_folder(value, v) ) {
        switch(target->builtin) {
            case BT::INT8:  return v >= -128 && v <= 127;
            case BT::INT16: return v >= -32768 && v <= 32767;
            case BT::INT32: return v >= -2147483648LL && v <= 2147483647LL;
            case BT::INT64: return true;
            default:        return false;
        }
    }

    Expr *inner = value;
    while(inner->node_type == ASTNodeType::UNARY_EXP_NODE && static_cast<UnaryExpr*>(inner)->op.type == TT::MINUS)
        inner = static_cast<UnaryExpr*>(inner)->expr;

    return inner->node_type == ASTNodeType::DECIMAL_LIT_NODE && target->builtin == BT::FLOAT32;
}

void SemanticAnalyser::check_conversion(const Type *from, const Type *to, Expr *value) {
    switch( checker.classify_conversion(from, to) ) {
        case Conversion::INVALID: throw SemanticError(value->range, checker.invalid_conversion(from, to));

        case Conversion::NARROWING:
            if( !literal_fits(value, to) ) warning(value->range, checker.narrowing_conversion(from, to));
            break;

        default: break;
    }
}

void SemanticAnalyser::analyze_statement(Stmt *s) {
    if(!s) return;

    try{
        s->accept(*this);
    }
    catch(const CompileError& e) {
        diagnostic.error(e);
    }
}

void SemanticAnalyser::analyze_expression(Expr *e) {
    if(!e) return;

    try{
        e->accept(*this);
    }
    catch(const CompileError& ex) {
        diagnostic.error(ex);

        if(!e->resolved_type) e->resolved_type = new UnknownType(ex.what());
    }
}

void SemanticAnalyser::visit(Program& p) {
   ScopeGuard g(manager, ScopeType::GLOBAL);

    for(Stmt *s : p.statements) analyze_statement(s);
}

void SemanticAnalyser::visit(BoolExpr& e) {
    BuiltinType *tmp = new BuiltinType(BT::BOOL);
    e.resolved_type = tmp ? tmp : nullptr;
}

void SemanticAnalyser::visit(IntNumberExpr& e) {
    const std::string& raw = e.raw_value.value;
    std::string suffix;

    size_t i = raw.size() - 1, count = 2;
    while(count > 0 && std::isalpha(raw[i])) {
        char c = std::tolower(raw[i]);
        if( !std::isxdigit(c) ) suffix = c + suffix;
        --count;
        --i;
    }

    e.suffix = suffix;

    BuiltinType::Types t;
    if(suffix == "")        t = BT::INT32;
    else if(suffix == "o")  t = BT::INT8;
    else if(suffix == "s")  t = BT::INT16;
    else if(suffix == "l")  t = BT::INT64;
    else if(suffix == "u")  t = BT::UINT32;
    else if(suffix == "uo") t = BT::UINT8;
    else if(suffix == "us") t = BT::UINT16;
    else if(suffix == "ul") t = BT::UINT64;
    else                    throw SemanticError(e.range, "unknown integer suffix '" + suffix + "'");

    BuiltinType *tmp = new BuiltinType(t);
    e.resolved_type = tmp ? tmp : nullptr;
}

void SemanticAnalyser::visit(DecimalNumberExpr& e) {
    char suffix = std::tolower(e.raw_value.value.back());
    BuiltinType::Types t;

    if(suffix == 'f')   t = BT::FLOAT32;
    else                t = BT::FLOAT64;

    BuiltinType *tmp = new BuiltinType(t);
    e.resolved_type = tmp ? tmp : nullptr;
}

void SemanticAnalyser::visit(CharExpr& e) {
    BuiltinType *tmp = new BuiltinType(BT::CHAR);
    e.resolved_type = tmp ? tmp : nullptr;
}

void SemanticAnalyser::visit(StringExpr& e) {
    BuiltinType *tmp = new BuiltinType(BT::STRING);
    e.resolved_type = tmp ? tmp : nullptr;
}

//[1, 2, 3]
void SemanticAnalyser::visit(ArrayLiteralExpr& e) {
    if(e.elements.empty()) {
        e.resolved_type = new ArrayType(nullptr, 0);
        return;
    }

    for(Expr *elem : e.elements) analyze_expression(elem);

    bool has_error = false;
    const Type *first = e.elements[0]->resolved_type;
    for(size_t i = 1; i < e.elements.size(); ++i) {
        Expr *current_expr = e.elements[i];
        const Type *t = current_expr->resolved_type;

        if( !checker.are_equals(first, t) ) {
            has_error = true;
            error(current_expr->range, "mixed types in array literal: expected all '" + checker.to_string(first) + "' but found '" + checker.to_string(t) + "' instead at element " + std::to_string(i+1));
        }
    }

    if(has_error) e.resolved_type = new UnknownType("mixed types in array literal");
    else          e.resolved_type = new ArrayType(first->clone(), e.elements.size());
}

void SemanticAnalyser::visit(IdentifierExpr& e) {
    Symbol *s = manager.lookup(e.name.value);
    if(!s) throw SemanticError(e.range, "use of undefined identifier '" + e.name.value + "'");

    e.symbol = s;
    e.resolved_type = s->declared_type->clone();

//    std::cout << "identifier expression's resolved type = " << checker.to_string(e.resolved_type) << "\n";
}

void SemanticAnalyser::visit(BinaryExpr& e) {
    if(e.left)  e.left->accept(*this);
    if(e.right) e.right->accept(*this);

    const Type *lt = e.left->resolved_type;
    const Type *rt = e.right->resolved_type;

    Type *result = checker.resolve_binary(lt, rt, e.op.type);
    if(!result) throw SemanticError(e.range, checker.type_mismatch(lt, rt, e.op));

    e.resolved_type = result;
//    std::cout << "binary expression's resolved type = " << checker.to_string(e.resolved_type) << "\n";
}

void SemanticAnalyser::visit(UnaryExpr& e) {
    if(e.expr) e.expr->accept(*this);

    const Type *operand = e.expr->resolved_type;

    Type *result = checker.resolve_unary(operand, e.op.type);
    if(!result) throw SemanticError(e.range, "operator '" + e.op.value + "' cannot be applied to type '" + checker.to_string(operand) + "'");

    e.resolved_type = result;
//    std::cout << "unary expression's resolved type = " << checker.to_string(e.resolved_type) << "\n";
}

void SemanticAnalyser::visit(AssignmentExpr& e) {
    if(e.target) e.target->accept(*this);
    if(e.value)  e.value->accept(*this);

    const Type *target_type = e.target->resolved_type;
    const Type *value_type = e.value->resolved_type;

    if(target_type && target_type->is_constant) throw SemanticError(e.target->range, "cannot assign to a const variable");

    Type *result = checker.resolve_assignment(target_type, value_type, e.op.type);
    if(!result) throw SemanticError(e.range, checker.invalid_conversion(value_type, target_type));

    e.resolved_type = result;
//    std::cout << "assignment expression's resolved type = " << checker.to_string(e.resolved_type) << "\n";
}

void SemanticAnalyser::visit(ConditionalExpr& e) {
    if(e.condition) e.condition->accept(*this);
    if(e.if_true)   e.if_true->accept(*this);
    if(e.if_false)  e.if_false->accept(*this);

    const Type *cond_type     = e.condition->resolved_type;
    const Type *if_true_type  = e.if_true->resolved_type;
    const Type *if_false_type = e.if_false->resolved_type;

    if(!checker.is_bool(cond_type) && !checker.is_numeric(cond_type)) throw SemanticError(e.range, "ternary condition must be bool or numeric, found '" + checker.to_string(cond_type) + "' instead");

    if(!checker.are_compatibles(if_true_type, if_false_type)) throw SemanticError(e.range, "ternary branches must have compatible types, found '" + checker.to_string(if_true_type) + "' and '" + checker.to_string(if_false_type) + "'");

    e.resolved_type = if_true_type->clone();
}

void SemanticAnalyser::visit(CallExpr& e) {
    if(!e.callee) error(e.callee->range, "missing callee in function call expression");

    if(e.callee->node_type != ASTNodeType::IDENTIFIER_EXPR_NODE) error(e.callee->range, "only direct calls to named functions are supported");

    IdentifierExpr *callee = static_cast<IdentifierExpr*>(e.callee);
    const std::string& fn_name = callee->name.value;

    Symbol *s = manager.lookup(fn_name);
    if(!s) throw SemanticError(callee->range, "call to undefined function '" + fn_name + "'");

    if(s->type != SymbolType::FUNCTION) throw SemanticError(callee->range, "'" + fn_name + "' is not a function");
    e.symbol = s;

    for(Expr *arg : e.arguments) analyze_expression(arg);

    FunctionDecl *decl = static_cast<FunctionDecl*>(s->declaration);
    if(!decl) throw SemanticError(callee->range, "the declaration of '" + fn_name + "' is missing");

    const std::vector<Parameter*>& params = decl->prototype->parameters;
    size_t expected_args = 0, provided_args = e.arguments.size(), max_args = params.size();
    for(Parameter *p : params)  {
        if(!p->default_value) ++expected_args;
    }

    if(provided_args < expected_args) throw SemanticError(e.range, "too few arguments to function '" + fn_name + "': expected at least " + std::to_string(expected_args) + ", but only " + std::to_string(provided_args) + " were provided");

    if(provided_args > max_args) throw SemanticError(e.range, "too many arguments to function '" + fn_name + "': expected at most " + std::to_string(max_args) + ", but " + std::to_string(provided_args) + " were provided");

    bool has_error = false;
    for(size_t i = 0; i < provided_args; ++i) {
        const Symbol *param_symbol = params[i]->symbol;
//        if(!param_symbol) continue;

        Expr *arg = e.arguments[i];
        const Type *arg_type = arg->resolved_type;
        const Type *param_type = param_symbol->declared_type;

        switch( checker.classify_conversion(arg_type, param_type) ) {
            case Conversion::INVALID:
                error(arg->range, "argument " + std::to_string(i + 1) + " of '" + fn_name + "' expects '" +
                                  checker.to_string(param_type) + "' but got '" + checker.to_string(arg_type) + "'");
                break;

            case Conversion::NARROWING:
                if( !literal_fits(arg, param_type) ) warning(arg->range, checker.narrowing_conversion(arg_type, param_type));
                break;

            default:
                break;
        }
    }

    if(!has_error) e.resolved_type = s->declared_type->clone();
}

void SemanticAnalyser::visit(MemberAccessExpr& e) {
    if(e.object) e.object->accept(*this);
}

void SemanticAnalyser::visit(SubscriptExpr& e) {
    if(e.object) e.object->accept(*this);
    for(Expr *i : e.indices) analyze_expression(i);

    const Type *current_obj_type = e.object->resolved_type;

    for(size_t i = 0; i < e.indices.size(); ++i) {
        if(!checker.is_array(current_obj_type)) throw SemanticError(e.object->range, "too many indices in subscript: '" + checker.to_string(e.object->resolved_type) + "' only has " + std::to_string(i) + " dimension(s), but " + std::to_string(e.indices.size()) + " were provided");

        Expr *current_index = e.indices[i];
        const Type *index_type = current_index->resolved_type;
        if(!checker.is_integral(index_type)) throw SemanticError(current_index->range, "array index must be an integer, found '" + checker.to_string(index_type) + "' instead");

        const ArrayType *arr_type = static_cast<const ArrayType*>(current_obj_type);

        long long index;
        size_t length = arr_type->size;
        if( unaryexpr_tiny_constant_folder(current_index, index) && (index < 0 || index >= static_cast<long long>(length)) ) throw SemanticError(current_index->range, "index out of bounds. Array's size is " + std::to_string(length));

        current_obj_type = arr_type->element_type;
    }

    e.resolved_type = current_obj_type->clone();
//    std::cout << "subscript expression's resolved type = " << checker.to_string(e.resolved_type) << "\n";
}

void SemanticAnalyser::visit(SequenceExpr& e) {
    for(Expr *exp : e.expressions) analyze_expression(exp);
    e.resolved_type = e.expressions.back()->resolved_type->clone();
//    std::cout << "sequence expression's resolved type = " << checker.to_string(e.resolved_type) << "\n";
}








void SemanticAnalyser::visit(VariableDecl& d) {
    Type *t = checker.resolve(d.declared_type);
    if( checker.is_unknown(t) ) error(d.declared_type.type_name.range, static_cast<UnknownType*>(t)->why);

    for(VariableDeclarator *vd : d.declarations) {
        const Token& name = vd->variable_name;

        bool redefined = false;
        if(Symbol *prev_decl = manager.lookup_current(name.value)) {
            redefined = true;
            error(name.range, "redefinition of variable: '" + name.value + "'");
            note(prev_decl->declaration->range, "previous declaration is here");
        }

        if(manager.get_type() == ScopeType::GLOBAL && !t->is_constant) warning(name.range, "global variable '" + name.value + "' is not const: other parts of the code could modify it by accident");

        try{
            if(vd->initializer) {
                vd->initializer->accept(*this);
                Type *init_type = vd->initializer->resolved_type;

                if(checker.is_auto(t)) {
                    AutoType *a = static_cast<AutoType*>(t);
                    a->resolved = init_type->clone();
                }
                // eg: int[2] a = [];
                // initializer is ArrayType(null, 0) so we use the declared type to set the initializer's base type.
                else if(checker.is_array(t) && checker.is_array(init_type)) {
                    Type *init_type_base_type = checker.find_base_type( static_cast<ArrayType*>(init_type) );
                    if(!init_type_base_type) init_type_base_type = checker.find_base_type( static_cast<ArrayType*>(t) )->clone();
                }
                else check_conversion(init_type, t, vd->initializer);
            }
            else {
                if(t->is_constant || checker.is_any(t) || checker.is_auto(t)) throw SemanticError(name.range, "'" + name.value + "' must be initialized because it is const, auto or any");
            }
        }
        catch(const CompileError& e) {
            diagnostic.error(e);
        }

        if(redefined) continue;

        Symbol *s = new Symbol(name.value, SymbolType::VARIABLE, t->is_constant, t->clone(), &d);
        manager.insert(s);
        vd->symbol = s;
    }
//    std::cout << "declared variable(s)' resolved type = " << checker.to_string(t) << "\n";
}

void SemanticAnalyser::check_fn_return_types(Type *ret_type, const Token& fn_name) {
    bool function_has_return = !current_function_return_stmts.empty();

    if(checker.is_auto(ret_type)) {
        AutoType *a = static_cast<AutoType*>(ret_type);

        if(!function_has_return) {
            a->resolved = new BuiltinType(BT::VOID);
            return;
        }

        BuiltinType void_type = BuiltinType(BT::VOID);
        bool has_error = false;
        const Type *first = current_function_return_stmts[0]->expr_stmt->expression ? current_function_return_stmts[0]->expr_stmt->expression->resolved_type : &void_type;

        for(size_t i = 1; i < current_function_return_stmts.size(); ++i) {
            const ReturnStmt *s = current_function_return_stmts[i];
            const Type *t = s->expr_stmt->expression ? s->expr_stmt->expression->resolved_type : &void_type;

            if( !checker.are_compatibles(t, first) ) {
                has_error = true;
                error(s->range, checker.invalid_conversion(t, first));
            }
        }

        a->resolved = has_error ? new UnknownType("function '" + fn_name.value + "' return's type couldn't be resolved") : first->clone();
    }
    else if (!checker.is_void(ret_type) && !function_has_return) error(fn_name.range, "no return statement in function returning non-void");
}

void SemanticAnalyser::visit(FunctionDecl& d) {
    const Token& fn_name = d.prototype->function_name;

    Type *return_type = checker.resolve(d.prototype->return_type);
    if( checker.is_unknown(return_type) ) error(d.prototype->return_type.type_name.range, static_cast<UnknownType*>(return_type)->why);

    Symbol *f = new Symbol(fn_name.value, SymbolType::FUNCTION, return_type->is_constant, return_type, &d);
    d.symbol = f;

    if(Symbol *prev_decl = manager.lookup_current(fn_name.value)) {
        error(d.range, "redefinition of function '" + fn_name.value + "'");
        note(prev_decl->declaration->range, "previous declaration is here");
    }
    else manager.insert(f);

    //The function is visible from its own body through the enclosing scope, which is what allows recursion.
    ScopeGuard g(manager, ScopeType::FUNCTION);

    for(Parameter *param : d.prototype->parameters) {
        const Token& pname = param->parameter_name;
        Type *param_type = checker.resolve(param->type_name);

        if( checker.is_unknown(param_type) )    error(param->type_name.type_name.range, static_cast<UnknownType*>(param_type)->why);
        else if( checker.is_auto(param_type) )  error(param->type_name.type_name.range, "'auto' can't be used as the type of a parameter");

        Symbol *p = new Symbol(pname.value, SymbolType::PARAMETER, param_type->is_constant, param_type, &d);
        param->symbol = p;

        if( manager.lookup_current(pname.value) ) error(pname.range, "duplicate parameter name '" + pname.value + "'");
        else manager.insert(p);

        if(param->default_value) param->default_value->accept(*this);
    }

    current_function_symbol = f;
    is_function_scope = true;

    if(d.body) d.body->accept(*this);

    check_fn_return_types(return_type, fn_name);
    current_function_return_stmts.clear();
    is_function_scope = false;
}








void SemanticAnalyser::visit(CompoundStmt& s) {
    ScopeGuard g(manager, ScopeType::BLOCK);

    for(Stmt *st : s.statements) analyze_statement(st);

}

void SemanticAnalyser::visit(ExpressionStmt& s) {
    if(s.expression) s.expression->accept(*this);
}

void SemanticAnalyser::visit(DeclarationStmt& s) {
    s.declaration->accept(*this);
}

void SemanticAnalyser::check_stmts_condition(Expr *condition) {
    analyze_expression(condition);
    const Type *cond_type = condition->resolved_type;

    try {
        if( !checker.is_bool(cond_type) && !checker.is_numeric(cond_type) )
            throw SemanticError(condition->range, "condition must be a boolean or numeric expression, found '" + checker.to_string(cond_type) + "'");
    }
    catch(const CompileError& e) {
        diagnostic.error(e);
    }
}

void SemanticAnalyser::visit(IfStmt& s) {
    check_stmts_condition(s.condition);
    analyze_statement(s.then_statement);
    analyze_statement(s.else_statement);
}

void SemanticAnalyser::visit(SwitchStmt& s) {
    analyze_expression(s.pattern);

    ScopeGuard g(manager, ScopeType::SWITCH);

    for(CaseClause *c : s.cases) {
        for(Expr *e : c->expressions) analyze_expression(e);
        analyze_statement(c->body);
    }
}

void SemanticAnalyser::visit(WhileStmt& s) {
    check_stmts_condition(s.condition);

    ScopeGuard g(manager, ScopeType::LOOP);
    analyze_statement(s.body);
}

void SemanticAnalyser::visit(DoWhileStmt& s) {
    {
        ScopeGuard g(manager, ScopeType::LOOP);
        analyze_statement(s.body);
    }

    check_stmts_condition(s.condition);
}

void SemanticAnalyser::visit(ForStmt& s) {
    ScopeGuard g(manager, ScopeType::LOOP);

    analyze_statement(s.initialization);
    check_stmts_condition(s.condition);
    analyze_expression(s.increment);
    analyze_statement(s.body);
}

void SemanticAnalyser::visit(RangeForStmt& s) {
    ScopeGuard g(manager, ScopeType::LOOP);

    try {
        if(s.item) s.item->accept(*this);

        analyze_expression(s.range_initializer);
        const Type *range_type = s.range_initializer->resolved_type;
        if( !checker.is_array(range_type) ) throw SemanticError(s.range_initializer->range, "range must be of an array type, found '" + checker.to_string(range_type) + "' instead");
    }
    catch(const CompileError& e) {
        diagnostic.error(e);
    }

    analyze_statement(s.body);
}

void SemanticAnalyser::visit(ReturnStmt& s) {
    if(!is_function_scope) throw SemanticError(s.range, "can't return outside of a function");

    const Type *fn_ret_type = current_function_symbol->declared_type;
    Expr *value = s.expr_stmt->expression;

    if(value) {
        value->accept(*this);
        const Type *ret_type = value->resolved_type;

        check_conversion(ret_type, fn_ret_type, value);
    }
    else {
        if(!checker.is_void(fn_ret_type) && !checker.is_auto(fn_ret_type)) {
            throw SemanticError(s.range, "missing return value in a function that returns '" + checker.to_string(fn_ret_type) + "'");
        }
    }

    current_function_return_stmts.push_back(&s);

//    std::cout << "starts at line: " << s.start.line << ", col: " << s.start.col << ", ends at line: " << s.end.line << ", col: " << s.end.col << "\n";
}

void SemanticAnalyser::visit(PrintStmt& s) {
    //print();                  good
    if(s.expressions.empty()) return;

    for(Expr *e : s.expressions) analyze_expression(e);

    if(!s.expressions[0] || s.expressions[0]->node_type != ASTNodeType::STRING_LIT_NODE) return;

    s.has_fmt = true;
    const std::string& fmt = static_cast<StringExpr*>(s.expressions[0])->value;
    const size_t expected_placeholders = s.expressions.size() - 1;

    size_t placeholders = 0, matching_expr_index = 1;
    for(size_t i = 0; i < fmt.size(); ++i) {
        if(fmt[i] == '{' && i+1 < fmt.size() && fmt[i+1] == '}') {
            ++i;
            if(matching_expr_index <= expected_placeholders) {
                ++placeholders;
                ++matching_expr_index;
            }
        }
    }


    //print(x);                 good
    //print("x = {}", x);       good
    //print("x = {");           good
    //print("x = {}");          good
    //print("x = ", x);         wrong
    //print("x = {", x);        wrong
    if(placeholders != expected_placeholders) throw SemanticError(s.range, "format string has " + std::to_string(placeholders) + " placeholder(s) but " + std::to_string(expected_placeholders) + " argument(s) were expected");

    s.nb_placeholders = placeholders;
}

void SemanticAnalyser::visit(BreakStmt& s) {
    if( !manager.inside(ScopeType::LOOP) && !manager.inside(ScopeType::SWITCH) ) throw SemanticError(s.range, "can't break outside of loops or switch");
}

void SemanticAnalyser::visit(ContinueStmt& s) {
    if( !manager.inside(ScopeType::LOOP) ) throw SemanticError(s.range, "continue statement not within a loop");
}
