#ifndef SYMBOL_H
#define SYMBOL_H

#include <string>
#include <iostream>
#include "ast_base.h"
#include "types.h"

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
