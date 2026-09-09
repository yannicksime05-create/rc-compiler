#ifndef TYPE_CHECKER_H
#define TYPE_CHECKER_H

#include <stdexcept>
#include <sstream>
#include "ast.h"
#include "token.h"
#include "types.h"

class TypeChecker {
    // --- Helpers
    const BuiltinType  *to_builtin(const Type *t);
    const ArrayType    *to_array(const Type *t);
    const AutoType     *to_auto(const Type *t);

    /**
    *   This is a helper function for are_compatibles and is_assignable.
    *   Special cases apart, those functions do the same work, so this helper does it for them.
    */
    bool assign_compat_helper(const Type *a, const Type *b);

public:
    /** Default constructor */
    TypeChecker() {}

    /** Default destructor */
    ~TypeChecker() = default;

    // --- Type classification
    bool is_builtin(const Type *t);
    bool is_array(const Type *t);
//    bool is_function(const Type *t);
    bool is_auto(const Type *t);
    bool is_any(const Type *t);
    bool is_unknown(const Type *t);

    // --- BuiltinType classification (all return false if t is not a BuiltinType)
    bool is_integral(const Type *t);
    bool is_floating(const Type *t);
    bool is_numeric(const Type *t);
    bool is_string(const Type *t);
    bool is_char(const Type *t);
    bool is_bool(const Type *t);
    bool is_void(const Type *t);

    // --- Type compatibility
    bool are_equals(const Type *a, const Type *b);
    bool are_compatibles(const Type *a, const Type *b);
    bool is_assignable(const Type *value, const Type *target);

    // --- Type resolution
    Type        *resolve(TypeSpecifier& t);
    Type        *resolve_type_name(Token& t);
    BuiltinType *promote(const Type *left, const Type *right);
    Type        *resolve_binary(const Type *left, const Type *right, TokenType op);
    Type        *resolve_unary(const Type *operand, TokenType op);
    Type        *resolve_assignment(const Type *target, const Type *value, TokenType op);


    // --- String conversion (for error messages)
    std::string to_string(const Type *t);
    std::string builtintype_to_string(const BuiltinType *t);
    std::string arraytype_to_string(const ArrayType *t);
    std::string autotype_to_string(const AutoType *t);

    // --- Error messages
    std::string type_mismatch(const Type *left, const Type *right, const Token& op);
    std::string invalid_conversion(const Type *from, const Type *to, const Token& where);
};

#endif // TYPECHECKER_H
