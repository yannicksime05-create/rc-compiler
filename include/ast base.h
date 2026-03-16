#ifndef AST_BASE_H
#define AST_BASE_H

enum class ASTNodeType {
    PROGRAM,

    INT_LIT_NODE, DECIMAL_LIT_NODE, STRING_LIT_NODE,

    //Expressions Nodes
    IDENTIFIER_EXPR_NODE, BINARY_EXPR_NODE, UNARY_EXP_NODE, ASSIGNMENT_EXPR_NODE,
    CONDITIONAL_EXPR_NODE, CALL_EXPR_NODE, MEMBER_ACCESS_EXPR_NODE, SUBSCRIPT_EXPR_NODE,
    SEQUENCE_EXPR_NODE,

    //Statements Nodes
    COMP_STMT_NODE, EXPR_STMT_NODE, IF_STMT_NODE, SWITCH_STMT_NODE, WHILE_STMT_NODE,
    DO_WHILE_STMT_NODE, FOR_STMT_NODE, RETURN_STMT_NODE, DECL_STMT_NODE,

    //Declarations Nodes
    VAR_DECL_NODE, FUNC_DECL_NODE

};


class Visitor;

struct ASTNode {
    ASTNodeType node_type;
    virtual void accept(Visitor& v) = 0;
    virtual ~ASTNode() = default;
};

#endif // AST_BASE_H
