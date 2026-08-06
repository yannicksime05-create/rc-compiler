#ifndef TYPES_H
#define TYPES_H

#include <vector>

enum class TypeKind {
    BUILTIN,
    FUNCTION,
    AUTO,
    ANY,
    ARRAY,
    UNKNOWN
};

struct Type {
    TypeKind kind;
    bool is_constant = false;

    Type(TypeKind tk, bool c = false) : kind(tk), is_constant(c) {}

    virtual Type *clone() const { return new Type(*this); }

    virtual ~Type() = default;
};

struct BuiltinType : Type {
    enum class Types {
        BOOL,
        INT,
        FLOAT,
        STRING,
        VOID
    };

    Types builtin;

    BuiltinType(Types b, bool c = false) : Type(TypeKind::BUILTIN, c), builtin(b) {}

    BuiltinType *clone() const override { return new BuiltinType(*this); }


    ~BuiltinType() {}
};

struct ArrayType : Type {
    Type *element_type = nullptr;
    int size = 0;

    ArrayType(Type *elmtp, int sz, bool c = false) : Type(TypeKind::ARRAY, c), element_type(elmtp), size(sz) {}

    ArrayType(const ArrayType& t) : Type(TypeKind::ARRAY, t.is_constant) {
        size = t.size;
        element_type = t.element_type ? t.element_type->clone() : nullptr;
    }

    ArrayType& operator=(const ArrayType& t) {
        if(this != &t) {
            size = t.size;

            if(element_type) {
                delete element_type;
                element_type = nullptr;
            }

            if(t.element_type) {
                element_type = t.element_type->clone();
            }
        }

        return *this;
    }

    ArrayType *clone() const override { return new ArrayType(*this); }

    ~ArrayType() {
        delete element_type;
        element_type = nullptr;
    }
};

struct AutoType : Type {
    Type *underlying_type = nullptr;

    AutoType(Type *t, bool c = false) : Type(TypeKind::AUTO, c) {}

    AutoType(const AutoType& t) : Type(TypeKind::AUTO, t.is_constant) {
        underlying_type = t.underlying_type ? t.underlying_type->clone() : nullptr;
    }

    AutoType& operator=(const AutoType& t) {
        if(this != &t) {
            is_constant = t.is_constant;
            delete underlying_type;

            underlying_type = t.underlying_type ? t.underlying_type->clone() : nullptr;
        }

        return *this;
    }

    AutoType *clone() const override { return new AutoType(*this); }

    ~AutoType() {
        delete underlying_type;
        underlying_type = nullptr;
    }
};

struct UnknownType : Type {
    std::string why;

    UnknownType(const std::string& reason, bool c = false) : Type(TypeKind::UNKNOWN, c), why(reason) {}

    UnknownType *clone() const override { return new UnknownType(*this); }

    ~UnknownType() {}
};

#endif // TYPES_H
