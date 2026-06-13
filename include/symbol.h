#ifndef SYMBOL_H
#define SYMBOL_H

#include <string>
#include <iostream>
#include "ast_base.h"

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

    //Use this when you can clone the Type by its copy construtor.
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

    ArrayType(const ArrayType& t) : Type(TypeKind::ARRAY) {
        size = t.size;
        if(t.element_type) {
            element_type = t.element_type->clone();
        }
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

//struct FunctionType : Type {
//    std::string name;
//    Type *return_type = nullptr;
//    std::vector<Type*> parameters_types;
//
//    FunctionType(Type *ret_type, const std::vector<Type*>& param_types, bool c = false) : Type(TypeKind::FUNCTION, c), return_type(ret_type), parameters_types(param_types) {}
//
//    FunctionType *clone() const override { return new FunctionType(*this); }
//
//    ~FunctionType() {
//        delete return_type;
//        return_type = nullptr;
//
//        for(Type *t : parameters_types) {
//            delete t;
//            t = nullptr;
//        }
//    }
//};

enum class SymbolType {
    VARIABLE,
    FUNCTION,
    PARAMETER,
};

struct Symbol {
    std::string name;
    SymbolType type;
    bool is_constant = false;
    Type *declared_type = nullptr;
    //Non-owning, so never call delete on it.
    ASTNode *declaration = nullptr;

    Symbol(const std::string& n, SymbolType t, bool c, Type *dt, ASTNode *node)
    : name(n), type(t), is_constant(c), declared_type(dt), declaration(node) {}

    ~Symbol() {
        if(declared_type) {
            delete declared_type;
            declared_type = nullptr;
        }

        std::cout << "Cleaning Symbol...\n";
    }
};

#endif // SYMBOL_H
