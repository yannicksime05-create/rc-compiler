#ifndef CPP_GENERATOR_H
#define CPP_GENERATOR_H

#include <string>
#include <sstream>
#include "../include/ast.h"

using TT = TokenType;


class CppGenerator : public Visitor {
    std::ostringstream out;
    int indent_level = 0;
    static const int TAB = 4;

    bool is_function_parameter = false;

    void indent() {
        for(int i = 0; i < indent_level; ++i) out << " ";
    }

    std::string map_type(const TypeSpecifier& t) {
        std::string base;
        for(const std::string& q : t.qualifiers) {
            base += q;
        }

        size_t depth = 0;
        bool is_vector = false;
        if(!t.dimension.empty()) {
            is_vector = true;

            for(depth = 1; depth <= t.dimension.size(); ++depth)  base += "std::vector<";
        }

        std::string s;
        switch(t.type_name.type) {
            case TT::KW_ANY:        s = "std::any";         break;
            case TT::KW_AUTO:       s = "auto";             break;
            case TT::KW_BOOL:       s = "bool";             break;
            case TT::KW_CHAR:       s = "char";             break;
            case TT::KW_DOUBLE:     s = "double";           break;
            case TT::KW_FLOAT:      s = "float";            break;
            case TT::KW_INT:        s = "int";              break;
            case TT::KW_STRING: {
                if(is_function_parameter)
                    s = "std::string&";
                else
                    s = "std::string";

                break;
            }
            case TT::KW_VOID:       s = "void";             break;
            default:                s = t.type_name.value;  break;
        }

        if(is_vector) {
            std::string tmp(depth, '>');
            s += tmp;
        }

        base += s;
        return base;
    }

public:
    std::string generate(Program& p) {
        out.str("");
        out << "#include <iostream>\n#include \"include/string_overloads.h\"\n#include <any>\n\n";
        visit(p);
        return out.str();
    }

    void visit(Program& p) override;

    void visit(BoolExpr& e) override;
    void visit(IntNumberExpr& e) override;
    void visit(DecimalNumberExpr& e) override;
    void visit(StringExpr& e) override;
    void visit(ArrayLiteralExpr& e) override;
    void visit(IdentifierExpr& e) override;
    void visit(BinaryExpr& e) override;
    void visit(UnaryExpr& e) override;
    void visit(AssignmentExpr& e) override;
    void visit(ConditionalExpr& e) override;
    void visit(CallExpr& e) override;
    void visit(MemberAccessExpr& e) override;
    void visit(SubscriptExpr& e) override;
    void visit(SequenceExpr& e) override;

    void visit(VariableDecl& d) override;
    void visit(FunctionDecl& d) override;

    void visit(CompoundStmt& s) override;
    void visit(ExpressionStmt& s) override;
    void visit(DeclarationStmt& s) override;
    void visit(IfStmt& s) override;
    void visit(SwitchStmt& s) override;
    void visit(WhileStmt& s) override;
    void visit(DoWhileStmt& s) override;
    void visit(ForStmt& s) override;
    void visit(RangeForStmt& s) override;
    void visit(ReturnStmt& s) override;
    void visit(PrintStmt& s) override;

};

#endif // CPP_GENERATOR_H
