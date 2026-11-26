#include "parser.hpp"


Token Parser::advance()
{
    Token t = peek();
    if (!is_end()) pos++;
    return t;
}
Token Parser::match(TypeToken type, std::string what)
{
    if(!check(type)) 
        throw std::runtime_error(std::to_string(peek().line) + " : " + std::to_string(peek().col) + " : expected " + what + ", received '" + peek().lexem + "'");
    return advance();
}

std::unique_ptr<Program> Parser::parseProgram()
{
    const Token &start = peek();

    match(TypeToken::Kw_program, "'процедура'");
    Token nameTok = match(TypeToken::Name, "имя процедуры");
    match(TypeToken::Semi, "';'");

    auto decls = parseVarBlock();
    auto body  = parseStmtList();

    match(TypeToken::Kw_end, "'конец'");
    match(TypeToken::Dot, "'.'");
    auto prog = std::make_unique<Program>(nameTok.lexem, std::move(decls), std::move(body));
    prog->set_pos(start.line, start.col);
    return prog;
}

std::vector<std::unique_ptr<VarDecl>> Parser::parseVarBlock()
{
    std::vector<std::unique_ptr<VarDecl>> decls;
    if (!check(TypeToken::Kw_var)) return decls;

    advance();
    decls.push_back(parseVarDecl());
    while (check(TypeToken::Semi))
    {
        advance();
        if (check(TypeToken::Name) && (peek(1).type == TypeToken::Comma || peek(1).type == TypeToken::Colon))
            decls.push_back(parseVarDecl());
        else
            break;
    }
    return decls;
}



std::unique_ptr<VarDecl> Parser::parseVarDecl()
{
    Token fn = match(TypeToken::Name, "имя переменной");
    std::vector<std::string> vename;
    vename.push_back(fn.lexem);

    while (check(TypeToken::Comma))
        advance(), vename.push_back(match(TypeToken::Name, "имя переменно").lexem);
    
    match(TypeToken::Colon, "':'");

    TypeKind kind;

    if  (check(TypeToken::Kw_float)) advance(), kind = TypeKind::Float;
    else if (check(TypeToken::Kw_int)) advance(), kind = TypeKind::Int;
    else throw std::runtime_error(std::to_string(peek().line) + " : " + std::to_string(peek().col) + " : expected type ('вешуственное' or 'целое'), received " + peek().lexem + "'");
    
    auto decl = std::make_unique<VarDecl>(std::move(vename), kind);
    decl->set_pos(fn.line, fn.col);
    return decl;
}

StmtList Parser::parseStmtList()
{
    StmtList stmts;
    while (!is_end() && !check(TypeToken::Kw_end) && !check(TypeToken::RBracket))
    {
        size_t before = pos;
        stmts.push_back(parseStmt());
        if (pos == before) throw std::runtime_error(std::to_string(peek().line) + " : " + std::to_string(peek().col) + " : parser did not advnce");
    }
    return stmts;
}

StmtPtr Parser::parseStmt()
{
    switch (peek().type)
    {
    case TypeToken::Kw_input:  return parseInputStmt();
    case TypeToken::Kw_output: return parseOutputStmt();
    case TypeToken::Kw_for:    return parseForStmt();
    case TypeToken::LBracket:  return parseBlockStmt();
    case TypeToken::Name:      return parseAssingStmt();
    default:
        throw std::runtime_error(std::to_string(peek().line) + " : " + std::to_string(peek().col) + " : expected statement (assign, input, output, for or '['), received '" + peek().lexem + "'");
    }
}

StmtPtr Parser::parseAssingStmt()
{
    Token nameTok = match(TypeToken::Name, "имя переменной");
    match(TypeToken::Op_assing, ":=");
    ExprPtr value = parseExpr();
    if (check(TypeToken::Semi)) advance();
    auto stmt = std::make_unique<AssingStmt>(nameTok.lexem, std::move(value));
    stmt->set_pos(nameTok.line, nameTok.col);
    return stmt;
}

StmtPtr Parser::parseInputStmt()
{
    Token kw = advance();
    match(TypeToken::LParen, "'('");
    
    std::vector<std::string> vars;
    vars.push_back(match(TypeToken::Name, "имя переменной").lexem);
    while (check(TypeToken::Comma))
        advance(), vars.push_back(match(TypeToken::Name, "имя переменной").lexem);
    match(TypeToken::RParen, "')'");

    if (check(TypeToken::Semi)) advance();
    auto stmt = std::make_unique<InputStmt>(std::move(vars));
    stmt->set_pos(kw.line, kw.col);
    return stmt;
}

StmtPtr Parser::parseOutputStmt()
{
    Token kw = advance();
    match(TypeToken::LParen, "'('");
    
    ExprList args;
    args.push_back(parseExpr());
    while (check(TypeToken::Comma))
        advance(), args.push_back(parseExpr());
    
    match(TypeToken::RParen, "')'");
    if (check(TypeToken::Semi)) advance();
    auto stmt = std::make_unique<OutputStmt>(std::move(args));
    stmt->set_pos(kw.line, kw.col);
    return stmt;
}

StmtPtr Parser::parseForStmt()
{
    Token kw = advance();

    Token count = match(TypeToken::Name, "имя счетчика");
    match(TypeToken::Kw_from, "'от'");
    ExprPtr from = parseExpr();
    match(TypeToken::Kw_to, "'до'");
    ExprPtr to = parseExpr();
    
    ExprPtr step;
    if (check(TypeToken::Kw_step))
        advance(), step = parseExpr();
    match(TypeToken::Kw_do, "'выполнить'");
    StmtPtr body = parseStmt();
    
    auto stmt = std::make_unique<ForStmt>(count.lexem, std::move(from), std::move(to), std::move(step), std::move(body));
    stmt->set_pos(kw.line, kw.col);
    return stmt;
}

StmtPtr Parser::parseBlockStmt()
{
    Token lb = match(TypeToken::LBracket, "'['");
    StmtList body = parseStmtList();
    match(TypeToken::RBracket, "']'");

    auto stmt = std::make_unique<BlockStmt>(std::move(body));
    stmt->set_pos (lb.line, lb.col);
    return stmt;
}

ExprPtr Parser::parseExpr() { return parseAddSubExpr(); }

ExprPtr Parser::parseAddSubExpr()
{
    ExprPtr left = parseMulDivExpr();
    while (check(TypeToken::Op_plus) || check(TypeToken::Op_minus)) 
    {
        Token opTok = advance();
        OpKind op = (opTok.type == TypeToken::Op_plus) ? OpKind::Add : OpKind::Sub;
        ExprPtr right = parseMulDivExpr();
        auto node = std::make_unique<BinaryExpr>(op, std::move(left), std::move(right));
        node->set_pos(opTok.line, opTok.col);
        left= std::move(node);
    }
    return left;
}

ExprPtr Parser::parseMulDivExpr()
{
    ExprPtr left = parseUnary();
    while (check(TypeToken::Op_mul) || check(TypeToken::Op_div))
    {
        Token opTok = advance();
        OpKind op = (opTok.type == TypeToken::Op_mul) ? OpKind::Mul : OpKind::Div;
        ExprPtr right = parseUnary();
        auto node = std::make_unique<BinaryExpr>(op, std::move(left), std::move(right));
        node->set_pos(opTok.line, opTok.col);
        left = std::move(node);
    }
    return left;
}

ExprPtr Parser::parseUnary()
{
    if (check(TypeToken::Op_minus))
    {
        Token opTok = advance();
        ExprPtr operand = parseUnary();
        auto node = std::make_unique<UnaryExpr>(OpKind::Neg, std::move(operand));
        node->set_pos(opTok.line, opTok.col);
        return node;
    }
    return parsePrimary();
}

ExprPtr Parser::parsePrimary()
{
    switch (peek().type)
    {
    case TypeToken::Int: 
    {
        Token t = advance();
        auto node = std::make_unique<LiteralExpr>(std::stoll(t.lexem));
        node->set_pos(t.line, t.col);
        return node;
    }
    case TypeToken::Float:
    {
        Token t = advance();
        auto node = std::make_unique<LiteralExpr>(std::stod(t.lexem));
        node->set_pos(t.line, t.col);
        return node;
    }
    case TypeToken::Name:
    {
        Token t = advance();
        auto node = std::make_unique<VarExpr>(t.lexem);
        node->set_pos(t.line, t.col);
        return node;
    }
    case TypeToken::LParen:
    {
        advance();
        ExprPtr e = parseExpr();
        match(TypeToken::RParen, "')'");
        return e;
    }
    default:
        throw std::runtime_error(std::to_string(peek().line) + " : " + std::to_string(peek().col) + " :  expected expression (number, name or '('), received '" + peek().lexem + "'");
    }
}

