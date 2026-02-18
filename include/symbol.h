#ifndef SYMBOL_H
#define SYMBOL_H

#include <string>

enum class TypeKind {
    BUILTIN,
    FUNCTION,
    AUTO,
    ANY,
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
//    FunctionTypes *function_data = nullptr;
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
        delete declaration;
        declaration = nullptr;
    }
};

#endif // SYMBOL_H
