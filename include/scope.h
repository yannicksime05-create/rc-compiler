#ifndef SCOPE_H
#define SCOPE_H

#include <unordered_map>
#include <string>
#include <vector>
#include "symbol.h"

enum class ScopeType {
    GLOBAL,
    FUNCTION,
    BLOCK
};

class Scope {
    ScopeType type;
    Scope *parent = nullptr;
    std::unordered_map<std::string, Symbol*> symbols;


public:
    Scope(ScopeType t, Scope *p = nullptr) : type(t), parent(p) {}

    bool insert(Symbol *s) {
        auto [it, inserted] = symbols.emplace(s->name, s);

        return inserted;
    }

    ScopeType get_type() const {
        return type;
    }

    Symbol *lookup(const std::string& name) const {
        auto it = symbols.find(name);

        return it != symbols.end() ? it->second : nullptr;
    }

    Symbol *global_lookup(const std::string& name) const {
        for(const Scope *s = this; s != nullptr; s = s->parent) {
            auto it = s->symbols.find(name);
            if(it != s->symbols.end())  return it->second;
        }

        return nullptr;
    }
};

class ScopeManager {
    std::vector<Scope*> scopes;

public:
    ScopeManager() {}

    Scope *current() const {
        if(!scopes.empty()) return scopes.back();
        return nullptr;
    }

    void enter(ScopeType type) {
        Scope *parent = scopes.empty() ? nullptr : scopes.back();
        scopes.push_back(new Scope(type, parent));
    }

    void exit() {
        if(!scopes.empty()) scopes.pop_back();
    }

    Symbol *lookup(const std::string& name) const {
        return current()->lookup(name);
    }
};

#endif // SCOPE_H
