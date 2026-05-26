#ifndef SYMBOL_H
#define SYMBOL_H

#include <string>
#include "ast base.h"

enum class TypeKind {
    BUILTIN,
    FUNCTION,
    AUTO,
    ANY,
    ARRAY,
    UNKNOWN
};

enum class BuiltinTypes {
    BOOL,
    INT,
    FLOAT,
    STRING,
    VOID
};

//struct FunctionTypes {
//    std::vector<DataType *> parameters_type;
//    DataType *return_type;
//};

//struct AutoTypes {
//    bool resolved;
//};

struct Type {
    TypeKind kind;
    bool is_constant = false;
//    bool is_auto_resolved = false;

    BuiltinTypes builtin;
    Type *element_type = nullptr; //When kind = ARRAY
//    FunctionTypes *function_data = nullptr;

    Type() {}
    Type(TypeKind tk, BuiltinTypes b, bool constness = true) : kind(tk), is_constant(constness), builtin(b) {}
    //Array type constructor
    Type(TypeKind tk, Type *elt_type) {
        kind = tk;
        element_type = elt_type;

        if(!element_type) {
//            element_type = new Type(TypeKind::UNKNOWN);
        }
    }

};

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
    ASTNode *declaration = nullptr;

    Symbol(const std::string& n, SymbolType t, bool c, Type *dt, ASTNode *node)
    : name(n), type(t), is_constant(c), declared_type(dt), declaration(node) {}

    ~Symbol() {
        delete declared_type;
        declared_type = nullptr;

//        std::cout << "Cleaning symbol...\n";
    }
};

#endif // SYMBOL_H
