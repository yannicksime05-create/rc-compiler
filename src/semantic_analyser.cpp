#include <sstream>
#include "../include/semantic_analyser.h"

using TT = TokenType;
using TK = TypeKind;

bool SemanticAnalyser::is_builtin_type(const Type *t) {
    return t && t->kind == TK::BUILTIN;
}

//For now it checks only int, but later, we'll add short, long, ...
bool SemanticAnalyser::is_integral_type(const BuiltinType *t) {
    return t && t->builtin == BuiltinType::Types::INT;
}

//Same as is_integral_type, but for float, double and long double.
bool SemanticAnalyser::is_floating_type(const BuiltinType *t) {
    return t && t->builtin == BuiltinType::Types::FLOAT;
}

bool SemanticAnalyser::is_numeric_type(const BuiltinType *t) {
    return is_integral_type(t) || is_floating_type(t);
}

bool SemanticAnalyser::is_string_type(const BuiltinType *t) {
    return t && t->builtin == BuiltinType::Types::STRING;
}

bool SemanticAnalyser::is_bool_type(const BuiltinType *t) {
    return t && t->builtin == BuiltinType::Types::BOOL;
}

std::string SemanticAnalyser::type_to_string(const Type *t) {
    if(!t) return "unknown";

    std::stringstream ss;
    if(t->is_constant) ss << "const ";

    switch(t->kind) {
        case TK::ARRAY: {
            const ArrayType *at = static_cast<const ArrayType*>(t);
            ss << arraytype_to_string(at);
            break;
        }
        case TK::BUILTIN: {
            const BuiltinType *bt = static_cast<const BuiltinType*>(t);
            ss << builtintype_to_string(bt);
            break;
        }
//        case TK::FUNCTION: {
//            const FunctionType *ft = static_cast<const FunctionType*>(t);
//            ss << functiontype_to_string(ft);
//        }
        default: ss << "unknown";
    }

    return ss.str();
}

std::string SemanticAnalyser::builtintype_to_string(const BuiltinType *t) {
    switch(t->builtin) {
        case BuiltinType::Types::BOOL:    return "bool";
        case BuiltinType::Types::FLOAT:   return "float";
        case BuiltinType::Types::INT:     return "int";
        case BuiltinType::Types::STRING:  return "string";
        case BuiltinType::Types::VOID:    return "void";
    }

    return "unknown";
}

std::string SemanticAnalyser::arraytype_to_string(const ArrayType *t) {
//    ARRAY(size=3, element: ARRAY(size=4, element: INT))
//  int[3][4]
    const ArrayType *tmp = t;
    while(tmp->element_type->kind == TK::ARRAY) tmp = static_cast<const ArrayType*>(tmp->element_type);

    std::stringstream ss;
    ss << type_to_string(tmp->element_type);

    tmp = t;
    ss << "[" << tmp->size << "]";
    while(tmp->element_type->kind == TK::ARRAY) {
        tmp = static_cast<const ArrayType*>(tmp->element_type);
        ss << "[" << tmp->size << "]";
    }
    tmp = nullptr;

    return ss.str();
}

//std::string SemanticAnalyser::functiontype_to_string(const FunctionType *t) {
//    std::stringstream ss;
//    ss << type_to_string(t->return_type) << " " << t->name << "(";
//
//    for(size_t i = 0; i < t->parameters_types.size(); ++i) {
//        ss << type_to_string(t->parameters_types[i]);
//
//        if(i < t->parameters_types.size()) ss << ", ";
//    }
//    ss << ")";
//
//    return ss.str();
//}

BuiltinType *SemanticAnalyser::promote(const BuiltinType *left, const BuiltinType *right) {
    if(!is_numeric_type(left) || !is_numeric_type(right)) return nullptr;

    if(left->builtin == BuiltinType::Types::FLOAT || right->builtin == BuiltinType::Types::FLOAT)
        return new BuiltinType(BuiltinType::Types::FLOAT);

    return new BuiltinType(BuiltinType::Types::INT);
}

std::string SemanticAnalyser::type_mismatch(const Type *lt, Token& op, const Type *rt) {
    std::stringstream ss;
    ss << "Operands' types mismatch for operator '" << op.value << "' at line: " << op.start.line
    << ".\n Left is: '" << type_to_string(lt) << "', right is: '" << type_to_string(rt) << "'.\n";

    return ss.str();
}

Type *SemanticAnalyser::resolve_type_name(Token& t) {
    switch(t.type) {
//        case TT::KW_ANY:    return new Type(TK::ANY);
//        case TT::KW_AUTO:   return new Type(TK::AUTO);

        case TT::KW_BOOL:   return new BuiltinType(BuiltinType::Types::BOOL);
        case TT::KW_INT:    return new BuiltinType(BuiltinType::Types::INT);

        case TT::KW_FLOAT:
        case TT::KW_DOUBLE: return new BuiltinType(BuiltinType::Types::FLOAT);

        case TT::KW_STRING: return new BuiltinType(BuiltinType::Types::STRING);
//        case TT::KW_VOID:
        default:            return new Type(TK::UNKNOWN);
    }
}

Type *SemanticAnalyser::resolve(TypeSpecifier& t) {
    Type *tmp = resolve_type_name(t.type_name);
//    //for now it only constains 'const'
    tmp->is_constant = !t.qualifiers.empty();

//        //int[3]            (kind: ARRAY, size: 3, elem_type: (kind: BUILTIN, builtin: INT))
//        //int[3][4]         (kind: ARRAY, size: 3, elem_type: (kind: ARRAY, size: 4, elem_type: (kind: BUILTIN, builtin: INT)))
    while( !t.dimension.empty() ) {
        Type *elem_type = tmp;

        tmp = new ArrayType(elem_type, t.dimension.back());
        t.dimension.pop_back();
    }

    return tmp;
}


void SemanticAnalyser::visit(Program& p) {
    manager.enter(ScopeType::GLOBAL);

    for(Stmt *s : p.statements) {
        if(s) s->accept(*this);
    }

    manager.exit();
}

void SemanticAnalyser::visit(BoolExpr& e) {
    e.resolved_type = new BuiltinType(BuiltinType::Types::BOOL);
}

void SemanticAnalyser::visit(IntNumberExpr& e) {
    e.resolved_type = new BuiltinType(BuiltinType::Types::INT);
}

void SemanticAnalyser::visit(DecimalNumberExpr& e) {
    e.resolved_type = new BuiltinType(BuiltinType::Types::FLOAT);
}

void SemanticAnalyser::visit(StringExpr& e) {
    e.resolved_type = new BuiltinType(BuiltinType::Types::STRING);
}

//[1, 2, 3]
void SemanticAnalyser::visit(ArrayLiteralExpr& e) {
    if(e.elements.empty()) return;

    for(Expr *elem : e.elements) {
        if(elem) elem->accept(*this);
    }

    std::stringstream ss;
    Type *first = e.elements[0]->resolved_type, *t = nullptr;
    for(size_t i = 1; i < e.elements.size(); ++i) {
        t = e.elements[i]->resolved_type;

        if(!t || t->kind != first->kind) {
            ss.str("");
            ss << "Error: Mixed types in array literal — expected all '"
               << type_to_string(first) << "' but found '"
               << type_to_string(t) << "' instead at element " << i+1 << "!";
            throw SemanticError(ss.str());
        }
    }
    t = nullptr;

    e.resolved_type = new ArrayType(first->clone(), static_cast<int>(e.elements.size()));
    first = nullptr;
}

void SemanticAnalyser::visit(IdentifierExpr& e) {
    Symbol *s = manager.lookup(e.name.value);
    if(!s) {
        std::stringstream ss;
        ss << "Used of undefined identifier. Line: " << e.name.start.line << ", Col: " << e.name.start.col << ".\n";
        throw SemanticError(ss.str());
    }

    e.symbol = s;
    e.resolved_type = s->declared_type->clone();

    std::cout << "identifier expression's resolved type = " << type_to_string(e.resolved_type) << "\n";
}

void SemanticAnalyser::visit(BinaryExpr& e) {
    if(e.left)  e.left->accept(*this);
    if(e.right) e.right->accept(*this);

    const Type *tmp1 = e.left->resolved_type;
    const Type *tmp2 = e.right->resolved_type;

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

    std::stringstream ss;
    if( !is_builtin_type(tmp1) || !is_builtin_type(tmp2) ) {
        ss << "Non builtin type not supported yet" << e.op.start.line << ".\n";
        throw SemanticError(ss.str());

    }

    const BuiltinType *lt = static_cast<const BuiltinType*>(tmp1);
    const BuiltinType *rt = static_cast<const BuiltinType*>(tmp2);
    tmp1 = tmp2 = nullptr;

    switch(e.op.type) {
        // --- Arithmetic: + - * / %
        case TT::PLUS: {
            if( is_string_type(lt) && ( is_string_type(rt) || is_numeric_type(rt) ) ) {
                e.resolved_type = new BuiltinType(BuiltinType::Types::STRING);
                break;
            }

            BuiltinType *result = promote(lt, rt);
            if(!result) throw SemanticError(type_mismatch(lt, e.op, rt));

            e.resolved_type = result;
            break;
        }
        case TT::MINUS: {
            if(is_string_type(lt) && is_string_type(rt)) {
                e.resolved_type = new BuiltinType(BuiltinType::Types::STRING);
                break;
            }

            BuiltinType *result = promote(lt, rt);
            if(!result) throw SemanticError(type_mismatch(lt, e.op, rt));

            e.resolved_type = result;
            break;
        }
        case TT::STAR: {
            if(is_string_type(lt) && is_integral_type(rt)) {
                e.resolved_type = new BuiltinType(BuiltinType::Types::STRING);
                break;
            }

            BuiltinType *result = promote(lt, rt);
            if(!result) throw SemanticError(type_mismatch(lt, e.op, rt));

            e.resolved_type = result;
            break;
        }
        case TT::SLASH: {
            if(is_string_type(lt) && (is_string_type(rt) || is_integral_type(rt))) {
                e.resolved_type = new ArrayType(lt->clone(), -1);
                break;
            }

            BuiltinType *result = promote(lt, rt);
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

            e.resolved_type = lt->clone();
            break;
        }

        // --- Comparison: < <= > >= == !=
        case TT::LESS:
        case TT::LESS_EQUAL:
        case TT::GREATER:
        case TT::GREATER_EQUAL: {
            bool valid = ( is_numeric_type(lt) && is_numeric_type(rt) ) || ( is_string_type(lt) && is_string_type(rt) );
            if(!valid) throw SemanticError(type_mismatch(lt, e.op, rt));

            e.resolved_type = new BuiltinType(BuiltinType::Types::BOOL);
            break;
        }
        case TT::EQUAL:
        case TT::NOT_EQUAL: {
            bool valid =    (is_bool_type(lt) && is_bool_type(rt))          ||
                            (is_string_type(lt) && is_string_type(rt))      ||
                            (is_numeric_type(lt) && is_numeric_type(rt));

            if(!valid)  throw SemanticError(type_mismatch(lt, e.op, rt));

            e.resolved_type = new BuiltinType(BuiltinType::Types::BOOL);
            break;
        }

        // --- Logical: && ||
        case TT::AND:
        case TT::OR: {
//            if(!is_bool_type(lt) || !is_bool_type(rt)) {
//                ss.str("");
//                ss << "Operator: '" << e.op.value << "' requires boolean type operands, found '"
//                << type_to_string(lt) << "' and '" << type_to_string(rt) << "' instead. Line: " << e.op.start.line << ".";
//                throw SemanticError(ss.str());
//            }

            e.resolved_type = new BuiltinType(BuiltinType::Types::BOOL);
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

            e.resolved_type = new BuiltinType(BuiltinType::Types::INT);
            break;
        }
        default:
            ss.str("");
            ss << "'" << e.op.value << "' is not a binary operator. Found at line: " << e.op.start.line << ".";
            throw SemanticError(ss.str());
    }

    std::cout << "binary expression's resolved type = " << type_to_string(e.resolved_type) << "\n";
}

void SemanticAnalyser::visit(UnaryExpr& e) {
    if(e.expr) e.expr->accept(*this);
    const Type *tmp = e.expr->resolved_type;

    std::stringstream ss;
    if( !is_builtin_type(tmp) ) {
        ss << "Non builtin type not supported yet" << e.op.start.line << ".\n";
        throw SemanticError(ss.str());
    }

    const BuiltinType *t = static_cast<const BuiltinType*>(tmp);
    tmp = nullptr;

    switch(e.op.type) {
        // --- - +
        case TT::PLUS:
        case TT::MINUS: {
            if(!is_numeric_type(t)) {
                ss.str("");
                ss << "Operator: '" << e.op.value << "' requires numeric type operand, found '"
                << type_to_string(t) << "' instead. Line: " << e.op.start.line << ".";
                throw SemanticError(ss.str());
            }

            e.resolved_type = t->clone();
            break;
        }
        // --- !
        case TT::NOT: {
            if( !is_bool_type(t) && !is_numeric_type(t) ) {
                ss.str("");
                ss << "Operator: '" << e.op.value << "' requires boolean type operand, found '"
                << type_to_string(t) << "' instead. Line: " << e.op.start.line << ".";
                throw SemanticError(ss.str());
            }

            e.resolved_type = new BuiltinType(BuiltinType::Types::BOOL);
            break;
        }
        // --- ++ --
        case TT::INCREMENT:
        case TT::DECREMENT: {
            if(!is_numeric_type(t)) {
                ss.str("");
                ss << "Operator: '" << e.op.value << "' requires numeric type operand, found '"
                << type_to_string(t) << "' instead. Line: " << e.op.start.line << ".";
                throw SemanticError(ss.str());
            }

            e.resolved_type = t->clone();
            break;
        }
        // --- ~
        case TT::BIT_NOT: {
            if(!is_integral_type(t)) {
                ss.str("");
                ss << "Operator: '" << e.op.value << "' requires integral type operand, found '"
                << type_to_string(t) << "' instead. Line: " << e.op.start.line << ".";
                throw SemanticError(ss.str());
            }

            e.resolved_type = t->clone();
            break;
        }
        default:
            ss.str("");
            ss << "'" << e.op.value << "' is not an unary operator. Found at line: " << e.op.start.line << ".";
            throw SemanticError(ss.str());
    }

    std::cout << "unary expression's resolved type = " << type_to_string(e.resolved_type) << "\n";
}

void SemanticAnalyser::visit(AssignmentExpr& e) {
    if(e.target) e.target->accept(*this);
    if(e.value)  e.value->accept(*this);

    const Type *tmp1 = e.target->resolved_type;
    const Type *tmp2 = e.value->resolved_type;

    std::stringstream ss;
    if( !is_builtin_type(tmp1) || !is_builtin_type(tmp2) ) {
        ss << "Non builtin type not supported yet" << e.op.start.line << ".\n";
        throw SemanticError(ss.str());
    }

    const BuiltinType *target_type = static_cast<const BuiltinType*>(tmp1);
    const BuiltinType *value_type = static_cast<const BuiltinType*>(tmp2);
    tmp1 = tmp2 = nullptr;

    switch(e.op.type) {
        case TT::ASSIGN: {
            if( (is_string_type(target_type) && !is_string_type(value_type)) ||
                (is_numeric_type(target_type) && !is_numeric_type(value_type))
              ) throw SemanticError(type_mismatch(target_type, e.op, value_type));

            e.resolved_type = target_type->clone();
            break;
        }
        case TT::PLUS_ASSIGN:
        case TT::MINUS_ASSIGN:
        case TT::STAR_ASSIGN: {
            bool valid = ( is_numeric_type(target_type) && is_numeric_type(value_type) )                                 ||
                         ( e.op.type == TT::MINUS_ASSIGN && is_string_type(target_type) && is_string_type(value_type) )  ||
                         ( e.op.type == TT::STAR_ASSIGN && is_string_type(target_type) && is_integral_type(value_type) ) ||
                         ( e.op.type == TT::PLUS_ASSIGN && is_string_type(target_type) && (is_string_type(value_type) || is_numeric_type(value_type)) );
            if(valid) {
                e.resolved_type = target_type->clone();
                break;
            }

            throw SemanticError(type_mismatch(target_type, e.op, value_type));
        }
        case TT::SLASH_ASSIGN: {
            if(is_string_type(target_type) && (is_string_type(value_type) || is_integral_type(value_type))) {
//                e.resolved_type = new Type(TK::ARRAY, target_type); //array of strings
                e.resolved_type = new ArrayType(target_type->clone(), -1);
                break;
            }

            if(is_numeric_type(target_type) && is_numeric_type(value_type)) {
                e.resolved_type = target_type->clone();
                break;
            }

            throw SemanticError(type_mismatch(target_type, e.op, value_type));
        }
        case TT::MOD_ASSIGN:
        case TT::BIT_OR_ASSIGN:
        case TT::BIT_AND_ASSIGN:
        case TT::BIT_XOR_ASSIGN:
        case TT::LEFT_SHIFT_ASSIGN:
        case TT::RIGHT_SHIFT_ASSIGN: {
            if(is_integral_type(target_type) && is_integral_type(value_type)) {
                e.resolved_type = target_type->clone();
                break;
            }

            throw SemanticError(type_mismatch(target_type, e.op, value_type));
        }

        default:
            ss.str("");
            ss << "'" << e.op.value << "' is not an assignment operator. Found at line: " << e.op.start.line << ".";
            throw SemanticError(ss.str());
    }

    std::cout << "assignment expression's resolved type = " << type_to_string(e.resolved_type) << "\n";
}

void SemanticAnalyser::visit(ConditionalExpr& e) {
    if(e.condition) e.condition->accept(*this);
    if(e.if_true)   e.if_true->accept(*this);
    if(e.if_false)  e.if_false->accept(*this);

    const Type *tmp1 = e.condition->resolved_type;
    const Type *tmp2 = e.if_true->resolved_type;
    const Type *tmp3 = e.if_false->resolved_type;

    std::stringstream ss;
    if( !is_builtin_type(tmp1) || !is_builtin_type(tmp2) || !is_builtin_type(tmp3) ) {
        ss << "Non builtin type not supported yet.\n";
        throw SemanticError(ss.str());
    }

    const BuiltinType *condition_type = static_cast<const BuiltinType*>(tmp1);
    const BuiltinType *if_true_type = static_cast<const BuiltinType*>(tmp2);
    const BuiltinType *if_false_type = static_cast<const BuiltinType*>(tmp3);
    tmp1 = tmp2 = tmp3 = nullptr;

    if(!is_bool_type(condition_type)) {
        ss.str("");
        ss << "Error: Ternary condition must be bool, found '" << type_to_string(condition_type) << "'!";
        throw SemanticError(ss.str());
    }

    bool both_numeric = is_numeric_type(if_true_type) && is_numeric_type(if_false_type);
    bool both_string  = is_string_type(if_true_type)  && is_string_type(if_false_type);
    bool both_bool    = is_bool_type(if_true_type)    && is_bool_type(if_false_type);

    if(!both_numeric && !both_string && !both_bool) {
        ss.str("");
        ss << "Error: Ternary branches must have compatible types, found '" << type_to_string(if_true_type) << "' and '" << type_to_string(if_false_type) << "'!";
        throw SemanticError(ss.str());
    }

    e.resolved_type = if_true_type->clone();
}

void SemanticAnalyser::visit(CallExpr& e) {
    if(!e.callee) throw SemanticError("Missing callee in function call expression!");

    std::stringstream ss;

    //callee is only an IdentifierExpr for now.
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

    size_t expected_args = 0, provided_args = e.arguments.size();
    for(Parameter *param : decl->parameters)  {
        if(!param->default_value) ++expected_args;
    }

    bool too_few_args = provided_args < expected_args, too_many_args = provided_args > decl->parameters.size();
    if(too_few_args || too_many_args) {
        ss  << "Error: ";

        if(too_few_args) ss << "too few ";
        else if(too_many_args) ss << "too many ";

        ss  << "arguments provided for function '"
            << callee->name.value << "', expected "
            << expected_args << " at least, but only "
            << provided_args << " were provided. Line: "
            << callee->name.start.line << ".\n";
        throw SemanticError(ss.str());
    }

    for(size_t i = 0; i < provided_args; ++i) {
        const Type *arg_type = e.arguments[i]->resolved_type;
        const Type *param_type = decl->parameters[i]->symbol->declared_type;

        bool valid_arg_type = arg_type->kind == param_type->kind;
        if( is_builtin_type(arg_type) && is_builtin_type(param_type) ) {
            const BuiltinType *tmp_a = static_cast<const BuiltinType*>(arg_type);
            const BuiltinType *tmp_p = static_cast<const BuiltinType*>(param_type);

            bool promotion = is_numeric_type(tmp_a) && is_numeric_type(tmp_p);

            tmp_a = tmp_p = nullptr;
            valid_arg_type = valid_arg_type && promotion;
        }

        if(!valid_arg_type) {
            ss << "Error: Argument " << i+1 << " of '" << callee->name.value
               << "' expects '" << type_to_string(param_type)
               << "' but got '" << type_to_string(arg_type)
               << "'. Line: " << callee->name.start.line << ".\n";
            throw SemanticError(ss.str());
        }
        arg_type = param_type = nullptr;
    }
    decl = nullptr;
    callee = nullptr;

    e.resolved_type = s->declared_type->clone();
    s = nullptr;

}

void SemanticAnalyser::visit(MemberAccessExpr& e) {
    if(e.object) e.object->accept(*this);
}

void SemanticAnalyser::visit(SubscriptExpr& e) {
    if(e.object) e.object->accept(*this);
    if(e.index)  e.index->accept(*this);

    const Type *tmp1 = e.object->resolved_type;
    std::stringstream ss;
    if(tmp1->kind != TK::ARRAY) {
        ss.str("");
        ss << "Error: Subscript operator '[]' requires an array type, found '" << type_to_string(tmp1) << "' instead!";
        tmp1 = nullptr;
        throw SemanticError(ss.str());
    }

    const Type *tmp2 = e.index->resolved_type;
    if( !is_builtin_type(tmp2) ) {

    }

    const ArrayType *obj_type = static_cast<const ArrayType*>(tmp1);
    const BuiltinType *index_type = static_cast<const BuiltinType*>(tmp2);

    if(!is_integral_type(index_type)) {
        ss.str("");
        ss << "Error: Array index must be an integer, found '" << type_to_string(index_type) << "' instead!";
        throw SemanticError(ss.str());
    }

    e.resolved_type = obj_type->element_type->clone();
    std::cout << "subscript expression's resolved type = " << type_to_string(e.resolved_type) << "\n";
}

void SemanticAnalyser::visit(SequenceExpr& e) {
    for(Expr *exp : e.expressions) {
        if(exp) exp->accept(*this);
    }

    e.resolved_type = e.expressions.back()->resolved_type->clone();
    std::cout << "sequence expression's resolved type = " << type_to_string(e.resolved_type) << "\n";
}








void SemanticAnalyser::visit(VariableDecl& d) {
    Type *t = resolve(d.declared_type);

    std::cout << "declared variable(s)' resolved type = " << type_to_string(t) << "\n";

    std::stringstream ss;
    for(VariableDeclarator *vd : d.declarations) {
        if( (t->is_constant || t->kind == TK::ANY || t->kind == TK::AUTO) && !vd->initializer ) {
            ss.str("");
            ss << "Missing initialization for const/any/auto types! Line: " << vd->variable_name.start.line << ", col: " << vd->variable_name.start.col+1 << "\n";
            throw SemanticError(ss.str());
        }

        if(vd->initializer) vd->initializer->accept(*this);

        if(manager.lookup_current(vd->variable_name.value)) {
            ss.str("");
            ss << "Error: Redefinition of variable: " << vd->variable_name.value << ". Line: "
            << vd->variable_name.start.line << ", Col: " << vd->variable_name.start.col;
            throw SemanticError(ss.str());
        }

        if(vd->initializer && is_builtin_type(t) && is_builtin_type(vd->initializer->resolved_type)) {
//            const Type *init_type = vd->initializer->resolved_type;
            const BuiltinType *tmp1 = static_cast<const BuiltinType*>(t);
            const BuiltinType *tmp2 = static_cast<const BuiltinType *>(vd->initializer->resolved_type);

            bool valid = ( is_numeric_type(tmp1) && is_numeric_type(tmp2) ) || ( is_string_type(tmp1) && is_string_type(tmp2) );

            if(!valid) {
                ss.str("");
                ss << "Invalid conversion from: '" << type_to_string(tmp2) << "' to: '" << type_to_string(tmp1) << "'. Line: " << d.declared_type.type_name.start.line << "\n";
                throw SemanticError(ss.str());
            }

            tmp1 = tmp2 = nullptr;
//            init_type = nullptr;
        }


        Symbol *s = new Symbol(vd->variable_name.value, SymbolType::VARIABLE, t->is_constant, t->clone(), &d);
        manager.insert(s);
        vd->symbol = s;
    }
}

void SemanticAnalyser::visit(FunctionDecl& d) {
    std::stringstream ss;
    if(manager.lookup_current(d.function_name.value)) {
        ss << "Error: Redefinition of function: '" << d.function_name.value << "'. Line: " << d.function_name.start.line << ".\n";
        throw SemanticError(ss.str());
    }

    Type *return_type = resolve(d.return_type);
    Symbol *f = new Symbol(d.function_name.value, SymbolType::FUNCTION, return_type->is_constant, return_type, &d);
    manager.insert(f);
    d.symbol = f;

    manager.enter(ScopeType::FUNCTION);
    manager.insert( new Symbol(d.function_name.value, SymbolType::FUNCTION, return_type->is_constant, return_type->clone(), &d) );
    for(Parameter *param : d.parameters) {
        if(manager.lookup_current(param->parameter_name.value)) {
            ss.str("");
            ss << "Error: Duplicate parameter name: '" << param->parameter_name.value << "'. Line: " << param->parameter_name.start.line << ".\n";
            throw SemanticError(ss.str());
        }

        Type *param_type = resolve(param->type_name);
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

