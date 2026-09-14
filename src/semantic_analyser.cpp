#include <sstream>
#include "../include/semantic_analyser.h"

using TT = TokenType;
using TK = TypeKind;

void SemanticAnalyser::visit(Program& p) {
    manager.enter(ScopeType::GLOBAL);

    for(Stmt *s : p.statements) {
        if(s) s->accept(*this);
    }

    manager.exit();
}

void SemanticAnalyser::visit(BoolExpr& e) {
    Type *tmp = new BuiltinType(BuiltinType::Types::BOOL);
    e.resolved_type = tmp ? tmp : nullptr;
}

void SemanticAnalyser::visit(IntNumberExpr& e) {
    Type *tmp = new BuiltinType(BuiltinType::Types::INT);
    e.resolved_type = tmp ? tmp : nullptr;
}

void SemanticAnalyser::visit(DecimalNumberExpr& e) {
    Type *tmp = new BuiltinType(BuiltinType::Types::FLOAT);
    e.resolved_type = tmp ? tmp : nullptr;
}

void SemanticAnalyser::visit(CharExpr& e) {
    Type *tmp = new BuiltinType(BuiltinType::Types::CHAR);
    e.resolved_type = tmp ? tmp : nullptr;
}

void SemanticAnalyser::visit(StringExpr& e) {
   Type *tmp = new BuiltinType(BuiltinType::Types::STRING);
    e.resolved_type = tmp ? tmp : nullptr;
}

//[1, 2, 3]
void SemanticAnalyser::visit(ArrayLiteralExpr& e) {
    if(e.elements.empty()) return;

    for(Expr *elem : e.elements) {
        if(elem) elem->accept(*this);
    }

    std::stringstream ss;
    bool has_error = false;
    const Type *first = e.elements[0]->resolved_type;
    for(size_t i = 1; i < e.elements.size(); ++i) {
        const Type *t = e.elements[i]->resolved_type;

        if( !checker.are_equals(first, t) ) {
            ss.str("");
            ss << "Error: Mixed types in array literal — expected all '"
               << checker.to_string(first) << "' but found '"
               << checker.to_string(t) << "' instead at element " << i+1 << "!";

            has_error = true;
            error(ss.str(), false);
//            throw SemanticError(ss.str());
        }
    }

    if(!has_error) e.resolved_type = new ArrayType(first->clone(), static_cast<int>(e.elements.size()));
}

void SemanticAnalyser::visit(IdentifierExpr& e) {
    Symbol *s = manager.lookup(e.name.value);
    if(!s) {
        std::stringstream ss;
        ss << "Use of undefined identifier '" << e.name.value << "'. Line: " << e.name.start.line << ", Col: " << e.name.start.col << ".\n";
        throw SemanticError(ss.str());
    }

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
    if(!result) throw SemanticError(checker.type_mismatch(lt, rt, e.op));

    e.resolved_type = result;
//    std::cout << "binary expression's resolved type = " << checker.to_string(e.resolved_type) << "\n";
}

void SemanticAnalyser::visit(UnaryExpr& e) {
    if(e.expr) e.expr->accept(*this);

    const Type *operand = e.expr->resolved_type;

    Type *result = checker.resolve_unary(operand, e.op.type);
    if(!result) {
        std::stringstream ss;
        ss << "Error: Operator '" << e.op.value
           << "' cannot be applied to type '" << checker.to_string(operand)
           << "'. Line: " << e.op.start.line << ".\n";
        throw SemanticError(ss.str());
    }

    e.resolved_type = result;
//    std::cout << "unary expression's resolved type = " << checker.to_string(e.resolved_type) << "\n";
}

void SemanticAnalyser::visit(AssignmentExpr& e) {
    if(e.target) e.target->accept(*this);
    if(e.value)  e.value->accept(*this);

    const Type *target_type = e.target->resolved_type;
    const Type *value_type = e.value->resolved_type;

    if(target_type && target_type->is_constant) {
        std::stringstream ss;
        ss << "Error: Cannot assign to a const variable. Line: " << e.op.start.line << ".\n";
        throw SemanticError(ss.str());
    }

    Type *result = checker.resolve_assignment(target_type, value_type, e.op.type);
    if(!result) throw SemanticError(checker.invalid_conversion(value_type, target_type, e.op));

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

    std::stringstream ss;
    if(!checker.is_bool(cond_type) && !checker.is_numeric(cond_type)) {
        ss << "Error: Ternary condition must be bool or numeric, found '"
           << checker.to_string(cond_type) << "'!\n";
        throw SemanticError(ss.str());
    }

    if(!checker.are_compatibles(if_true_type, if_false_type)) {
        ss << "Error: Ternary branches must have compatible types, found '" << checker.to_string(if_true_type) << "' and '" << checker.to_string(if_false_type) << "'!\n";
        throw SemanticError(ss.str());
    }

    e.resolved_type = if_true_type->clone();
}

void SemanticAnalyser::visit(CallExpr& e) {
    if(!e.callee) throw SemanticError("Missing callee in function call expression!");

    if(e.callee->node_type != ASTNodeType::IDENTIFIER_EXPR_NODE) throw SemanticError("Error: Only direct calls to named functions are supported.\n");

    std::stringstream ss;
    IdentifierExpr *callee = static_cast<IdentifierExpr*>(e.callee);

    Symbol *s = manager.lookup(callee->name.value);
    if(!s) {
        ss << "Error: Call to undefined function '" << callee->name.value << "'. Line: " << callee->name.start.line << ".";
        throw SemanticError(ss.str());
    }

    if(s->type != SymbolType::FUNCTION) {
        ss.str("");
        ss << "Error: '" << callee->name.value << "' is not a function! Line: " << callee->name.start.line << ".";
        throw SemanticError(ss.str());
    }
    e.symbol = s;

    for(Expr *arg : e.arguments) {
        if(arg) arg->accept(*this);
    }

    FunctionDecl *decl = static_cast<FunctionDecl*>(s->declaration);
    if(!decl) {
        ss << "Error: Declaration of symbol '" << s->name << "' not found!";
        throw SemanticError(ss.str());
    }

    size_t expected_args = 0, provided_args = e.arguments.size(), max_args = decl->parameters.size();
    for(Parameter *param : decl->parameters)  {
        if(!param->default_value) ++expected_args;
    }

    bool too_few_args = provided_args < expected_args, too_many_args = provided_args > max_args;
    if(too_few_args || too_many_args) {
        ss  << "Error: ";

        if(too_few_args)        ss << "too few ";
        else if(too_many_args)  ss << "too many ";

        ss  << "arguments to function '"
            << callee->name.value << "', expected ";

        if(too_few_args)        ss << expected_args << " at least, but only ";
        else if(too_many_args)  ss << max_args << " at most, but ";

        ss  << provided_args << " were provided."
            << " Line: "
            << callee->name.start.line << ".\n";
        throw SemanticError(ss.str());
    }

    bool has_error = false;
    for(size_t i = 0; i < provided_args; ++i) {
        const Type *arg_type = e.arguments[i]->resolved_type;
        const Type *param_type = decl->parameters[i]->symbol->declared_type;

        //This will print an error if param_type is auto or any.
        if(!checker.are_compatibles(arg_type, param_type)) {
            ss << "Error: Argument " << i+1 << " of '" << callee->name.value
               << "' expects '" << checker.to_string(param_type)
               << "' but got '" << checker.to_string(arg_type)
               << "'. Line: " << callee->name.start.line << ".\n";

            has_error = true;
            error(ss.str(), false);
        }
    }

    if(!has_error) e.resolved_type = s->declared_type->clone();
}

void SemanticAnalyser::visit(MemberAccessExpr& e) {
    if(e.object) e.object->accept(*this);
}

void SemanticAnalyser::visit(SubscriptExpr& e) {
    if(e.object) e.object->accept(*this);
    if(e.index)  e.index->accept(*this);

    const Type *obj_type = e.object->resolved_type;
    std::stringstream ss;
    if(!checker.is_array(obj_type)) {
        ss << "Error: Subscript operator '[]' requires an array type, found '" << checker.to_string(obj_type) << "' instead!";
        throw SemanticError(ss.str());
    }

    const Type *index_type = e.index->resolved_type;
    if(!checker.is_integral(index_type)) {
        ss << "Error: Array index must be an integer, found '" << checker.to_string(index_type) << "' instead!";
        throw SemanticError(ss.str());
    }

    e.resolved_type = static_cast<const ArrayType*>(obj_type)->element_type->clone();
    std::cout << "subscript expression's resolved type = " << checker.to_string(e.resolved_type) << "\n";
}

void SemanticAnalyser::visit(SequenceExpr& e) {
    for(Expr *exp : e.expressions) {
        if(exp) exp->accept(*this);
    }

    e.resolved_type = e.expressions.back()->resolved_type->clone();
//    std::cout << "sequence expression's resolved type = " << checker.to_string(e.resolved_type) << "\n";
}








void SemanticAnalyser::visit(VariableDecl& d) {
    Type *t = checker.resolve(d.declared_type);

    std::stringstream ss;
    for(VariableDeclarator *vd : d.declarations) {
        if( (t->is_constant || checker.is_any(t) || checker.is_auto(t)) && !vd->initializer) {
            ss.str("");
            ss << "Missing initialization for const/any/auto types! Line: " << vd->variable_name.start.line << ", col: " << vd->variable_name.start.col+1 << "\n";
            throw SemanticError(ss.str());
        }

        if(manager.current()->get_type() == ScopeType::GLOBAL && !t->is_constant) {
            ss << "Warning: Declaration of variable '" << vd->variable_name.value
               << "' happening in the global scope! Some other parts of your code might accidentally modify it. Either mark it const or remove it. Line: "
               << d.declared_type.type_name.start.line << "\n";
            warning(ss.str());
            ss.str("");
        }

        if(manager.lookup_current(vd->variable_name.value)) {
            ss.str("");
            ss << "Error: Redefinition of variable: '" << vd->variable_name.value << "'. Line: "
            << vd->variable_name.start.line << ", Col: " << vd->variable_name.start.col;
            throw SemanticError(ss.str());
        }

        if(vd->initializer) {
            vd->initializer->accept(*this);
            const Type *init_type = vd->initializer->resolved_type;

            if(checker.is_auto(t)) {
                AutoType *at = static_cast<AutoType*>(t);
                at->resolved = init_type->clone();
            }
            else if(!checker.is_assignable(init_type, t)) {
                throw SemanticError(checker.invalid_conversion(init_type, t, d.declared_type.type_name));
            }
        }

        Symbol *s = new Symbol(vd->variable_name.value, SymbolType::VARIABLE, t->is_constant, t->clone(), &d);
        manager.insert(s);
        vd->symbol = s;
    }

//    std::cout << "declared variable(s)' resolved type = " << checker.to_string(t) << "\n";
}

void SemanticAnalyser::check_fn_return_types(Type *ret_type, const Token& fn_name) {
    if(current_function_return_stmts.empty() && !checker.is_void(ret_type)) {
        std::stringstream ss;
        ss << "Error: No return statement in function returning non-void. Line: " << fn_name.start.line << "\n";

        throw SemanticError(ss.str());
    }

    if(checker.is_auto(ret_type)) {
        bool has_error = false;
        const Type *first_retstmt_type = current_function_return_stmts[0]->expression->resolved_type;

        for(size_t i = 1; i < current_function_return_stmts.size(); ++i) {
            const ReturnStmt *stmt = current_function_return_stmts[i];
            const Type *t = stmt->expression->resolved_type;

            if( !checker.are_compatibles(t, first_retstmt_type) ) {
                has_error = true;
                error(checker.invalid_conversion(t, first_retstmt_type, stmt->location), false);
            }
        }

        if(!has_error) {
            AutoType *tmp = static_cast<AutoType*>(ret_type);
            tmp->resolved = first_retstmt_type->clone();
            ret_type = tmp;
        }
    }
}

void SemanticAnalyser::visit(FunctionDecl& d) {
    std::stringstream ss;

    if(manager.lookup_current(d.function_name.value)) {
        ss << "Error: Redefinition of function: '" << d.function_name.value << "'. Line: " << d.function_name.start.line << ".\n";
        throw SemanticError(ss.str());
    }

    Type *return_type = checker.resolve(d.return_type);

    Symbol *f = new Symbol(d.function_name.value, SymbolType::FUNCTION, return_type->is_constant, return_type, &d);
    manager.insert(f);
    d.symbol = f;

    manager.enter(ScopeType::FUNCTION);

    //This allows recursion.
    manager.insert( new Symbol(d.function_name.value, SymbolType::FUNCTION, return_type->is_constant, return_type->clone(), &d) );

    for(Parameter *param : d.parameters) {
        if(manager.lookup_current(param->parameter_name.value)) {
            ss.str("");
            ss << "Error: Duplicate parameter name: '" << param->parameter_name.value << "'. Line: " << param->parameter_name.start.line << ".\n";
            throw SemanticError(ss.str());
        }

        Type *param_type = checker.resolve(param->type_name);
        if( checker.is_auto(param_type) ) {
            ss << "Error: auto is forbidden as a parameter's type. Line: " << param->parameter_name.start.line << "\n";
            error(ss.str(), false);
            ss.str("");
        }

        Symbol *p = new Symbol(param->parameter_name.value, SymbolType::PARAMETER, param_type->is_constant, param_type, &d);
        manager.insert(p);
        param->symbol = p;

        if(param->default_value) param->default_value->accept(*this);
    }

    current_function_symbol = f;
    is_function_scope = true;

    if(d.body) d.body->accept(*this);

    check_fn_return_types(return_type, d.function_name);
    current_function_return_stmts.clear();

    manager.exit();
    is_function_scope = false;
    current_function_symbol = nullptr;
}









void SemanticAnalyser::visit(CompoundStmt& s) {
    manager.enter(ScopeType::BLOCK);

    for(Stmt *st : s.statements) {
        if(st) st->accept(*this);
    }

    manager.exit();
}

void SemanticAnalyser::visit(ExpressionStmt& s) {
    if(s.expression) s.expression->accept(*this);
}

void SemanticAnalyser::visit(DeclarationStmt& s) {
    s.declaration->accept(*this);
}

void SemanticAnalyser::check_stmts_condition(Expr *condition, const Token& where) {
    Type *cond_type = nullptr;

    if(condition) {
        condition->accept(*this);
        cond_type = condition->resolved_type;
    }

    if( !checker.is_builtin(cond_type) ) {
        std::stringstream ss;
        ss << "Error: Condition must be of a builtin type, found '" << checker.to_string(cond_type) << "'. Line: " << where.start.line << ".\n";
        throw SemanticError(ss.str());
    }

    if( !checker.is_bool(cond_type) && !checker.is_numeric(cond_type) ) {
        std::stringstream ss;
        ss << "Error: Condition must be a numeric or boolean expression, found '" << checker.to_string(cond_type) << "'. Line: " << where.start.line << ".\n";
        throw SemanticError(ss.str());
    }
}

void SemanticAnalyser::visit(IfStmt& s) {
    check_stmts_condition(s.condition, s.location);
    if(s.then_statement)    s.then_statement->accept(*this);
    if(s.else_statement)    s.else_statement->accept(*this);
}

void SemanticAnalyser::visit(SwitchStmt& s) {
    if(s.pattern) s.pattern->accept(*this);
    ++switch_depth;
    for(CaseClause *c : s.cases) {
        for(Expr *e : c->expressions) if(e) e->accept(*this);
        if(c->body)         c->body->accept(*this);
    }
    --switch_depth;
}

void SemanticAnalyser::visit(WhileStmt& s) {
    check_stmts_condition(s.condition, s.location);
    ++loop_depth;
    if(s.body)      s.body->accept(*this);
    --loop_depth;
}

void SemanticAnalyser::visit(DoWhileStmt& s) {
    ++loop_depth;
    if(s.body)      s.body->accept(*this);
    --loop_depth;
    check_stmts_condition(s.condition, s.location);
}

void SemanticAnalyser::visit(ForStmt& s) {
    manager.enter(ScopeType::BLOCK);

    if(s.initialization) s.initialization->accept(*this);
    if(s.condition)      s.condition->accept(*this);
    if(s.increment)      s.increment->accept(*this);
    ++loop_depth;
    if(s.body)           s.body->accept(*this);
    --loop_depth;

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
    std::stringstream ss;
    if(!is_function_scope) {
        ss << "Can't return outside of a function! Line: " << s.location.start.line << "\n";
        throw SemanticError(ss.str());
    }

    const Type *fn_ret_type = current_function_symbol->declared_type;
    if(s.expression) {
        s.expression->accept(*this);
        const Type *ret_type = s.expression->resolved_type;

        if(!checker.are_compatibles(ret_type, fn_ret_type)) throw SemanticError(checker.invalid_conversion(ret_type, fn_ret_type, s.location));
    }
    else {
        if(!checker.is_void(fn_ret_type)) {
            ss << "Error: Missing return value in non-void function. Line: " << s.location.start.line << ".\n";
            throw SemanticError(ss.str());
        }
    }

    current_function_return_stmts.push_back(&s);
}

void SemanticAnalyser::visit(PrintStmt& s) {
    //print();                  good
    if(s.expressions.empty()) return;

    for(Expr *e : s.expressions) {
        if(e) e->accept(*this);
    }

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
    if(placeholders != expected_placeholders) {
        std::stringstream ss;
        ss << "print: format string has " << placeholders << " placeholder(s) but " << expected_placeholders << " placeholder(s) were expected! Line: " << s.location.start.line << ".\n";
        throw SemanticError(ss.str());
    }

    s.nb_placeholders = placeholders;
}

void SemanticAnalyser::visit(BreakStmt& s) {
    if(!loop_depth && !switch_depth) {
        std::stringstream ss;
        ss << "Error: Can't break outside of loops of switch! Line: " << s.location.start.line << "\n";
        throw SemanticError(ss.str());
    }
}

