#ifndef SCOPE_H
#define SCOPE_H

#include <unordered_map>
#include <string>
#include <vector>
#include "symbol.h"

enum class ScopeType {
    GLOBAL,
    FUNCTION,
    BLOCK,
    LOOP,
    SWITCH
};

class Scope {
    ScopeType type;
    Scope *parent = nullptr;
    std::unordered_map<std::string, Symbol*> symbols;


public:
    Scope(ScopeType t, Scope *p = nullptr) : type(t), parent(p) {}

    bool insert(Symbol *s) {
        auto result = symbols.emplace(s->name, s);

        return result.second;
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

    /**
    *   True if this scope, or one of the scopes around it, has the given type.
    *   The search stops at the nearest function: currently used for ScopeType::LOOP and ScopeType::SWITCH
    */
    bool is_inside(ScopeType t) {
        for(const Scope *s = this; s != nullptr; s = s->parent) {
            if(s->type == t)                    return true;
            if(s->type == ScopeType::FUNCTION)  return false;
        }

        return false;
    }

    //This destructor does nothing because the declarations nodes on the ast will delete their symbols.
    ~Scope() {
    }
};

class ScopeManager {
    std::vector<Scope*> scopes;

public:
    ScopeManager() {}

    ~ScopeManager() {
        for(Scope *s : scopes) if(s) delete s;
    }

    Scope *current() const {
        return scopes.empty() ? nullptr : scopes.back();
    }

    void enter(ScopeType type) {
        Scope *parent = scopes.empty() ? nullptr : scopes.back();
        scopes.push_back(new Scope(type, parent));
    }

    void exit() {
        if(!scopes.empty()) {
            delete scopes.back();
            scopes.pop_back();
        }
    }

    /**
    *   @brief Gets the type of the current scope.
    */
    ScopeType get_type() { return current()->get_type(); }

    /**
    *   @brief Tells if we're inside a scope of this type (within the current function).
    */
    bool inside(ScopeType t) { return current() && current()->is_inside(t); }

    /**
    *   @brief Inserts into the current scope.
    */
    bool insert(Symbol *s) { return current()->insert(s); }

    /**
    *   @brief Looks up into the entire scope chain.
    */
    Symbol *lookup(const std::string& name) const {
        if( !current() ) return nullptr;
        return current()->global_lookup(name);
    }

    /**
    *   @brief Looks up into the current scope.
    */
    Symbol *lookup_current(const std::string& name) const {
        if(!current()) return nullptr;
        return current()->lookup(name);
    }
};

class ScopeGuard {
    ScopeManager& manager;

public:
    ScopeGuard(ScopeManager& m, ScopeType t) : manager(m) { manager.enter(t); }

    ~ScopeGuard() {
        manager.exit();
    }
};

#endif // SCOPE_H
