#pragma once

#include <ostream>
#include <string>
#include <unordered_map>

#include "ast.hpp"


class Codegen
{
private:
    std::ostream& out;
    std::unordered_map<std::string, TypeKind> types;

    void genStmt(const Stmt& s);
    void genExpr(const Expr& e);

    TypeKind typeOf(const Expr& e) const;   

public:
    Codegen(std::ostream& out) : out(out) {}
    void generate(const Program& prog);
};
