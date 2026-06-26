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

#endif // TYPES_H
