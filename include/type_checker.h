#ifndef TYPE_CHECKER_H
#define TYPE_CHECKER_H

#include "ast.h"
#include "token.h"
#include "types.h"

/**
*   How good a conversion from one type to another is. Only classifies: reporting is the analyser's job.
*/
enum class Conversion {
    EXACT,          // same type
    WIDENING,       // numeric, to a type at least as big (int -> long)
    NARROWING,      // numeric, to a smaller type (double -> int): allowed, but the value may change
    INVALID
};

class TypeChecker {
    // --- Helpers
    const BuiltinType  *to_builtin(const Type *t);
    const ArrayType    *to_array(const Type *t);
    const AutoType     *to_auto(const Type *t);

    int numeric_rank(const BuiltinType *t);

    // --- String conversion (for error messages)
    std::string builtintype_to_string(const BuiltinType *t);
    std::string arraytype_to_string(const ArrayType *t);
    std::string autotype_to_string(const AutoType *t);

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
    bool is_unsigned(const Type *t);
    bool is_signed(const Type *t);
    bool is_floating(const Type *t);
    bool is_numeric(const Type *t);
    bool is_string(const Type *t);
    bool is_char(const Type *t);
    bool is_bool(const Type *t);
    bool is_void(const Type *t);

    // --- Type compatibility
    Conversion classify_conversion(const Type *from, const Type *to);
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

    //
    Type        *find_base_type(const ArrayType *t);

    // --- String conversion (for error messages)
    std::string to_string(const Type *t);

    // --- Error messages
    std::string type_mismatch(const Type *left, const Type *right, const Token& op);
    std::string invalid_conversion(const Type *from, const Type *to);
    std::string narrowing_conversion(const Type *from, const Type *to);
};

#endif // TYPECHECKER_H
