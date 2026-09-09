#include "type_checker.h"

using TT = TokenType;
using TK = TypeKind;
using BT = BuiltinType::Types;

const BuiltinType *TypeChecker::to_builtin(const Type *t) {
    if(!t || t->kind != TK::BUILTIN) return nullptr;

    return static_cast<const BuiltinType*>(t);
}

const ArrayType *TypeChecker::to_array(const Type *t) {
    if(!t || t->kind != TK::ARRAY) return nullptr;

    return static_cast<const ArrayType*>(t);
}

const AutoType *TypeChecker::to_auto(const Type *t) {
    if(!t || t->kind != TK::AUTO) return nullptr;

    return static_cast<const AutoType*>(t);
}

bool TypeChecker::assign_compat_helper(const Type *a, const Type *b) {
    switch(a->kind) {
        case TK::ANY:       return true;
        case TK::ARRAY:     return are_equals(a, b);
        case TK::BUILTIN: {
            if(is_numeric(a))  return is_numeric(b);

            // char, string, bool are only assignable from themselves
            return are_equals(a, b);
        }
        default:            return false;
    }
}

bool TypeChecker::is_builtin(const Type *t)        {   return t && t->kind == TK::BUILTIN;     }
bool TypeChecker::is_array(const Type *t)          {   return t && t->kind == TK::ARRAY;       }
bool TypeChecker::is_auto(const Type *t)           {   return t && t->kind == TK::AUTO;        }
bool TypeChecker::is_any(const Type *t)            {   return t && t->kind == TK::ANY;         }
bool TypeChecker::is_unknown(const Type *t)        {   return t && t->kind == TK::UNKNOWN;     }


//For now it checks only int, but later, we'll add short, long, ...
bool TypeChecker::is_integral(const Type *t) {
    const BuiltinType *bt = to_builtin(t);
    return bt && bt->builtin == BT::INT;
}

//Same as is_integral_type, but for float, double and long double.
bool TypeChecker::is_floating(const Type *t) {
    const BuiltinType *bt = to_builtin(t);
    return bt && bt->builtin == BT::FLOAT;
}

bool TypeChecker::is_numeric(const Type *t) {
    return is_integral(t) || is_floating(t);
}

bool TypeChecker::is_string(const Type *t) {
    const BuiltinType *bt = to_builtin(t);
    return bt && bt->builtin == BT::STRING;
}

bool TypeChecker::is_char(const Type *t) {
    const BuiltinType *bt = to_builtin(t);
    return bt && bt->builtin == BT::CHAR;
}

bool TypeChecker::is_bool(const Type *t) {
    const BuiltinType *bt = to_builtin(t);
    return bt && bt->builtin == BT::BOOL;
}

bool TypeChecker::is_void(const Type *t) {
    const BuiltinType *bt = to_builtin(t);
    return bt && bt->builtin == BT::VOID;
}


bool TypeChecker::are_equals(const Type *a, const Type *b) {
    if(!a || !b)            return false;
    if(a->kind != b->kind)  return false;

    switch(a->kind) {
        case TK::ARRAY: {
            const ArrayType *ata = to_array(a), *atb = to_array(b);
            return ata->size == atb->size && are_equals(ata->element_type, atb->element_type);
        }
        case TK::AUTO: {
            const AutoType *ata = to_auto(a), *atb = to_auto(b);
            return are_equals(ata->resolved, atb->resolved);
        }
        case TK::BUILTIN: {
            const BuiltinType *bta = to_builtin(a), *btb = to_builtin(b);
            return bta->builtin == btb->builtin;
        }
        default: return false;
    }
}

bool TypeChecker::are_compatibles(const Type *a, const Type *b) {
    if(!a || !b)            return false;

    if(a->kind == TK::AUTO) {
        const AutoType *aa = to_auto(a);
        return aa->resolved ? are_compatibles(aa->resolved, b) : true;
    }
    if(b->kind == TK::AUTO) {
        const AutoType *ab = to_auto(b);
        return ab->resolved ? are_compatibles(ab->resolved, a) : true;
    }

    if(a->kind != b->kind)  return false;

    return assign_compat_helper(a, b);
}

bool TypeChecker::is_assignable(const Type *value, const Type *target) {
    if(!target || !value)   return false;

    if(target->kind == TK::AUTO) {
        const AutoType *at = to_auto(target);
        return is_assignable(value, at->resolved);
    }

    return assign_compat_helper(target, value);
}


Type *TypeChecker::resolve(TypeSpecifier& t) {
    Type *tmp = resolve_type_name(t.type_name);
    //for now it only constains 'const'
    tmp->is_constant = !t.qualifiers.empty();

    //int[3]            (kind: ARRAY, size: 3, elem_type: (kind: BUILTIN, builtin: INT))
    //int[3][4]         (kind: ARRAY, size: 3, elem_type: (kind: ARRAY, size: 4, elem_type: (kind: BUILTIN, builtin: INT)))
    size_t dimension = t.dimension.size();
    while( dimension ) {
        Type *elem_type = tmp;

        tmp = new ArrayType(elem_type, t.dimension.back());
        --dimension;
    }

    return tmp;
}

Type *TypeChecker::resolve_type_name(Token& t) {
    switch(t.type) {
//        case TT::KW_ANY:    return new Type(TK::ANY);
        case TT::KW_AUTO:   return new AutoType();

        case TT::KW_BOOL:   return new BuiltinType(BT::BOOL);
        case TT::KW_INT:    return new BuiltinType(BT::INT);

        case TT::KW_FLOAT:
        case TT::KW_DOUBLE: return new BuiltinType(BT::FLOAT);

        case TT::KW_CHAR:   return new BuiltinType(BT::CHAR);
        case TT::KW_STRING: return new BuiltinType(BT::STRING);

        case TT::KW_VOID:   return new BuiltinType(BT::VOID);
        default: {
            std::stringstream ss;
            ss << "undefined type '" << t.value << "'";
            return new UnknownType(ss.str());
        }
    }
}

BuiltinType *TypeChecker::promote(const Type *left, const Type *right) {
    if(!left || !right) return nullptr;

    if(!is_numeric(left) || !is_numeric(right)) return nullptr;

    if(is_floating(left) || is_floating(right)) return new BuiltinType(BT::FLOAT);

    return new BuiltinType(BT::INT);
}

Type *TypeChecker::resolve_binary(const Type *left, const Type *right, TokenType op) {
    if(!left || !right) return nullptr;

    switch(op) {
        case TT::PLUS: {
            // string + (string | char | numeric)  → string
            if( is_string(left) && (is_string(right) || is_numeric(right) || is_char(right)) ) return left->clone();

            return promote(left, right);
        }
        case TT::MINUS: {
            // string - string → string (trim)
            if( is_string(left) && is_string(right) ) return left->clone();

            return promote(left, right);
        }
        case TT::STAR: {
            // string * integral → string (repeat)
            if( is_string(left) && is_integral(right) ) return left->clone();

            return promote(left, right);
        }
        case TT::SLASH: {
            // string / (string | integral) → array of strings (split)
            if( is_string(left) && (is_string(right) || is_integral(right)) ) return new ArrayType(new BuiltinType(BT::STRING), -1);

            return promote(left, right);
        }
        case TT::MOD: {
            if( !is_integral(left) || !is_integral(right) ) return nullptr;

            return left->clone();
        }
        // --- Comparison: < <= > >=
        case TT::LESS:
        case TT::LESS_EQUAL:
        case TT::GREATER:
        case TT::GREATER_EQUAL: {
            bool valid = (is_numeric(left) && is_numeric(right)) ||
                         (is_string(left)  && is_string(right))  ||
                         (is_char(left)    && is_char(right));

            if(!valid) return nullptr;
            return new BuiltinType(BT::BOOL);
        }
        case TT::EQUAL:
        case TT::NOT_EQUAL: {
            bool valid = (is_numeric(left) && is_numeric(right)) ||
                         (is_string(left)  && is_string(right))  ||
                         (is_char(left)    && is_char(right))    ||
                         (is_bool(left)    && is_bool(right));

            if(!valid) return nullptr;

            return new BuiltinType(BT::BOOL);
        }
        // --- Logical: && ||
        case TT::AND:
        case TT::OR: {
            // accept bool and numeric (non-zero as truthy)
            bool valid = ( is_bool(left) || is_numeric(left) ) && ( is_bool(right) || is_numeric(right) );
            if(!valid) return nullptr;

            return new BuiltinType(BT::BOOL);
        }
        // --- Bitwise: & | ^ << >>
        case TT::BIT_AND:
        case TT::BIT_OR:
        case TT::BIT_XOR:
        case TT::LEFT_SHIFT:
        case TT::RIGHT_SHIFT: {
            if( !is_integral(left) || !is_integral(right) ) return nullptr;

            return promote(left, right);
        }

        default: return nullptr;
    }
}

Type *TypeChecker::resolve_unary(const Type *operand, TokenType op) {
    if(!operand) return nullptr;

    switch(op) {
        // --- - +
        case TT::PLUS:
        case TT::MINUS: {
            if( !is_numeric(operand) ) return nullptr;

            return operand->clone();
        }
        // --- !
        case TT::NOT: {
            if( !is_bool(operand) && !is_numeric(operand) ) return nullptr;

            return new BuiltinType(BT::BOOL);
        }
        // --- ++ --
        case TT::INCREMENT:
        case TT::DECREMENT: {
            if( !is_numeric(operand) ) return nullptr;

            return operand->clone();
        }
        // --- ~
        case TT::BIT_NOT: {
            if( !is_integral(operand) ) return nullptr;

            return operand->clone();
        }
        default: return nullptr;
    }
}

Type *TypeChecker::resolve_assignment(const Type *target, const Type *value, TokenType op) {
    if(!target || !value) return nullptr;

    switch(op) {
        case TT::ASSIGN: {
            if( !is_assignable(value, target) ) return nullptr;

            return target->clone();
        }
        // numeric += numeric
        // string += (string | char)
        case TT::PLUS_ASSIGN: {
            bool valid = ( is_numeric(target) && is_numeric(value) ) || ( is_string(target) && (is_string(value) || is_numeric(value) || is_char(value)) );
            if(!valid) return nullptr;

            return target->clone();
        }
        case TT::MINUS_ASSIGN: {
            bool valid = ( is_numeric(target) && is_numeric(value) ) || ( is_string(target) && is_string(value) );
            if(!valid) return nullptr;

            return target->clone();
        }
        case TT::STAR_ASSIGN: {
            bool valid = ( is_numeric(target) && is_numeric(value) ) || ( is_string(target) && is_integral(value) );
            if(!valid) return nullptr;

            return target->clone();
        }
        // string /= (string | integral) → array of strings
        case TT::SLASH_ASSIGN: {
            if( is_string(target) && (is_string(value) || is_integral(value)) ) return new ArrayType(new BuiltinType(BT::STRING), -1);

            if( is_numeric(target) && is_numeric(value) ) return target->clone();

            return nullptr;
        }
        case TT::MOD_ASSIGN:
        case TT::BIT_OR_ASSIGN:
        case TT::BIT_AND_ASSIGN:
        case TT::BIT_XOR_ASSIGN:
        case TT::LEFT_SHIFT_ASSIGN:
        case TT::RIGHT_SHIFT_ASSIGN: {
            bool valid = is_integral(target) && is_integral(value);
            if(!valid) return nullptr;

            return target->clone();
        }
        default: return nullptr;
    }
}

std::string TypeChecker::to_string(const Type *t) {
    if(!t) return "null";

    std::stringstream ss;
    if(t->is_constant) ss << "const ";

    switch(t->kind) {
        case TK::ARRAY: {
            const ArrayType *at = to_array(t);
            ss << arraytype_to_string(at);
            break;
        }
        case TK::BUILTIN: {
            const BuiltinType *bt = to_builtin(t);
            ss << builtintype_to_string(bt);
            break;
        }
        case TK::AUTO: {
            const AutoType *at = to_auto(t);
            ss << autotype_to_string(at);
            break;
        }
        default: ss << "unknown";
    }

    return ss.str();
}

std::string TypeChecker::builtintype_to_string(const BuiltinType *t) {
    switch(t->builtin) {
        case BT::BOOL:    return "bool";
        case BT::FLOAT:   return "float";
        case BT::INT:     return "int";
        case BT::CHAR:    return "char";
        case BT::STRING:  return "string";
        case BT::VOID:    return "void";
    }

    return "unknown";
}

std::string TypeChecker::arraytype_to_string(const ArrayType *t) {
    const ArrayType *tmp = t;
    while(tmp->element_type->kind == TK::ARRAY) tmp = to_array(tmp->element_type);

    std::stringstream ss;
    ss << to_string(tmp->element_type);

    tmp = t;
    ss << "[" << tmp->size << "]";
    while(tmp->element_type->kind == TK::ARRAY) {
        tmp = to_array(tmp->element_type);
        ss << "[";
        if(tmp->size > 0) ss << tmp->size;
        ss << "]";
    }
    tmp = nullptr;

    return ss.str();
}

std::string TypeChecker::autotype_to_string(const AutoType *t) {
    return to_string(t->resolved);
}

std::string TypeChecker::type_mismatch(const Type *left, const Type *right, const Token& op) {
    std::stringstream ss;
    ss << "Error: Operands' types mismatch for operator '" << op.value << "' at line: " << op.start.line
    << ".\n Left is: '" << to_string(left) << "', right is: '" << to_string(right) << "'.\n";

    return ss.str();
}

std::string TypeChecker::invalid_conversion(const Type *from, const Type *to, const Token& where) {
    std::stringstream ss;
    ss << "Error: Invalid conversion from '" << to_string(from) << "' to '" << to_string(to) << "'. Line: " << where.start.line << "\n";

    return ss.str();
}
