#pragma once

#include <vector>
#include <string>
#include <memory>
#include <stdexcept>
#include <stdint.h>

enum class TypeKind : uint8_t
{
    Float = 0,
    Int
};

enum class OpKind : uint8_t
{
    Add = 0,
    Sub,
    Mul,
    Div,
    Neg
};

struct ASTNode
{
    int line = 0, col = 0;
    void set_pos(int l, int c) { line = l, col = c; }
    virtual ~ASTNode() = default;
};

using NodePtr = std::unique_ptr<ASTNode>;
using NodeList = std::vector<NodePtr>;

struct Expr : ASTNode {};

using ExprPtr = std::unique_ptr<Expr>;
using ExprList = std::vector<ExprPtr>;

struct LiteralExpr : Expr
{
    union { long long i; double f; } value;
    TypeKind type;

    LiteralExpr(long long v) : value{ .i = v }, type(TypeKind::Int) {}
    LiteralExpr(double v) : value{ .f = v }, type(TypeKind::Float) {}
};

struct VarExpr : Expr
{
    std::string name;

    VarExpr(std::string n) : name(std::move(n)) {}
};

struct BinaryExpr : Expr
{
    OpKind op;
    ExprPtr left;
    ExprPtr right;

    BinaryExpr(OpKind op, ExprPtr left, ExprPtr right) : op(op), left(std::move(left)), right(std::move(right)) {}
};

struct UnaryExpr : Expr
{
    OpKind op;
    ExprPtr operand;

    UnaryExpr(OpKind op, ExprPtr operand) : op(op), operand(std::move(operand)) {}
};

struct VarDecl: ASTNode
{
    std::vector<std::string> vename;
    TypeKind type;

    VarDecl(std::vector<std::string> vename, TypeKind type) : vename(std::move(vename)), type(type) {}
};

struct Stmt : ASTNode {};

using StmtPtr = std::unique_ptr<Stmt>;
using StmtList = std::vector<StmtPtr>;

struct AssingStmt : Stmt
{
    std::string target;
    ExprPtr value;

    AssingStmt(std::string target, ExprPtr value) : target(target), value(std::move(value)) {}
};

struct InputStmt : Stmt
{
    std::vector<std::string> vars;
    InputStmt(std::vector<std::string> vars) : vars(std::move(vars)) {}
};


struct OutputStmt : Stmt
{
    ExprList args;
    OutputStmt(ExprList args) : args(std::move(args)) {}
};

struct ForStmt : Stmt
{
    std::string count_name;
    ExprPtr from;
    ExprPtr to;
    ExprPtr step;
    StmtPtr body;

    ForStmt(std::string count_name, ExprPtr from, ExprPtr to, ExprPtr step, StmtPtr body) :
        count_name(std::move(count_name)), from(std::move(from)), to(std::move(to)), step(std::move(step)), body(std::move(body)) {}
};

struct BlockStmt : Stmt
{
    StmtList body;

    BlockStmt(StmtList body) : body(std::move(body)) {}
};

struct Program : ASTNode
{
    std::string name;
    std::vector<std::unique_ptr<VarDecl>> decls;
    StmtList body;

    Program(std::string name, std::vector<std::unique_ptr<VarDecl>> decls, StmtList body) :
        name(std::move((name))), decls(std::move(decls)), body(std::move(body)) {}
};




