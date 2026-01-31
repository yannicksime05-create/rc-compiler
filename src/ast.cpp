//#include <iostream>
//#include <exception>
#include "../include/ast.h"

void Program::accept(Visitor& v) {
    v.visit(*this);
}







//
void IntNumberExpr::accept(Visitor& v) {
    v.visit(*this);
}

void DecimalNumberExpr::accept(Visitor& v) {
    v.visit(*this);
}

void StringExpr::accept(Visitor& v) {
    v.visit(*this);
}

void IdentifierExpr::accept(Visitor& v) {
    v.visit(*this);
}

void BinaryExpr::accept(Visitor& v) {
    v.visit(*this);
}

void UnaryExpr::accept(Visitor& v) {
    v.visit(*this);
}

void AssignmentExpr::accept(Visitor& v) {
    v.visit(*this);
}

void ConditionalExpr::accept(Visitor& v) {
    v.visit(*this);
}

void CallExpr::accept(Visitor& v) {
    v.visit(*this);
}

void MemberAccessExpr::accept(Visitor& v) {
    v.visit(*this);
}

void SubscriptExpr::accept(Visitor& v) {
    v.visit(*this);
}

void SequenceExpr::accept(Visitor& v) {
    v.visit(*this);
}






//
void VariableDecl::accept(Visitor& v) {
    v.visit(*this);
}

void FunctionDecl::accept(Visitor& v) {
    v.visit(*this);
}







//
void CompoundStmt::accept(Visitor& v) {
    v.visit(*this);
}

void ExpressionStmt::accept(Visitor& v) {
    v.visit(*this);
}

void DeclarationStmt::accept(Visitor& v) {
    v.visit(*this);
}

void IfStmt::accept(Visitor& v) {
    v.visit(*this);
}

void SwitchStmt::accept(Visitor& v) {
    v.visit(*this);
}

void WhileStmt::accept(Visitor& v) {
    v.visit(*this);
}

void DoWhileStmt::accept(Visitor& v) {
    v.visit(*this);
}

void ForStmt::accept(Visitor& v) {
    v.visit(*this);
}

void ReturnStmt::accept(Visitor& v) {
    v.visit(*this);
}
