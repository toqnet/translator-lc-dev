#pragma once

#include <stdexcept>
#include <string>
#include <vector>
#include <memory>

#include "lexer.hpp"
#include "ast.hpp"

class Parser
{
private:
    std::vector<Token> tokens;
    size_t pos = 0;

    const Token &peek() const { return tokens[pos]; }
    const Token &peek(size_t offset) const { return pos+offset < tokens.size() ? tokens[pos+offset] : tokens.back(); }
    bool is_end() const { return peek().type ==  TypeToken::Eof; }
    bool check(TypeToken type) const { return peek().type == type; }
    Token advance();
    Token match(TypeToken type, std::string what);

    std::vector<std::unique_ptr<VarDecl>> parseVarBlock();
    std::unique_ptr<VarDecl> parseVarDecl();

    StmtList parseStmtList();

    StmtPtr parseStmt();
    StmtPtr parseAssingStmt();
    StmtPtr parseInputStmt();
    StmtPtr parseOutputStmt();
    StmtPtr parseForStmt();
    StmtPtr parseBlockStmt();
    
    ExprPtr parseExpr();
    ExprPtr parseAddSubExpr();
    ExprPtr parseMulDivExpr();
    ExprPtr parseUnary();
    ExprPtr parsePrimary();
public:
    Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}
    std::unique_ptr<Program> parseProgram();
};