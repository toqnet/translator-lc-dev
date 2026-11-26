#include "lexer.hpp"



inline unsigned char uchar(char c) { return static_cast<unsigned char>(c); }

static const std::unordered_map<std::string, TypeToken> keywords 
{
    {"процедура", TypeToken::Kw_program },
    {"program", TypeToken::Kw_program },
    {"переменные", TypeToken::Kw_var},
    {"vars", TypeToken::Kw_var},
    {"вещественные", TypeToken::Kw_float},
    {"float", TypeToken::Kw_float},
    {"целые", TypeToken::Kw_int},
    {"int", TypeToken::Kw_int},
    {"ввод", TypeToken::Kw_input},
    {"input", TypeToken::Kw_input},
    {"передача", TypeToken::Kw_output},
    {"output", TypeToken::Kw_output},
    {"цикл", TypeToken::Kw_for},
    {"for", TypeToken::Kw_for},
    {"от", TypeToken::Kw_from},
    {"from", TypeToken::Kw_from},
    {"до", TypeToken::Kw_to},
    {"to", TypeToken::Kw_to},
    {"шаг", TypeToken::Kw_step},
    {"step", TypeToken::Kw_step},
    {"выполнить", TypeToken::Kw_do},
    {"do", TypeToken::Kw_do},
    {"конец", TypeToken::Kw_end},
    {"end", TypeToken::Kw_end}
};

void Lexer::advance()
{
    if (is_end()) return;
    if (peek() == '\n') line++, col=1;
    else col++;
    pos++;
}

bool Lexer::isStartName(unsigned char c)
{
    if (std::isalpha(c) || c == '_') return true;
    if (c >= 0x80) return true;
    return false;
}

bool Lexer::isName(unsigned char c)
{
    if (std::isalnum(c) || c == '_') return true;
    if (c >= 0x80) return true;
    return false;
}

bool Lexer::isSpace() { return (peek() == ' ') || (peek() == '\t') || (peek() == '\r') || (peek() == '\n'); }

void Lexer::skipCommentSpace()
{
    while(!is_end())
        if (isSpace()) advance();
        else if ((peek() == '/') && (peek(1) == '/')) { while (!is_end() && peek() != '\n') advance(); }
        else break;
}           

Token Lexer::parseName()
{
    int startLine = line, startCol = col;
    std::string buffer;
    while (!is_end() && isName(uchar(peek())))
        buffer += peek(), advance();
    auto it = keywords.find(buffer);
    if (it != keywords.end()) return Token(it->second, buffer, startLine, startCol);
    else  return Token(TypeToken::Name, buffer, startLine, startCol);
}

Token Lexer::parseNumber()
{
    int startLine = line, startCol = col;
    bool is_float = false;
    std::string buffer;
    if (peek() == '0') buffer += peek(), advance();
    else while (!is_end() && std::isdigit(uchar(peek()))) buffer += peek(), advance();
    if (peek() == '.' && std::isdigit(uchar(peek(1)))) 
    {   
        is_float = true;
        buffer += peek(), advance();
        while (!is_end() && std::isdigit(peek())) 
            buffer += peek(), advance();
    }

    return Token(is_float ? TypeToken::Float : TypeToken::Int, buffer, startLine, startCol);
}

Token Lexer::next()
{
    skipCommentSpace();
    if (is_end()) return make(TypeToken::Eof, "eof");
    if (isStartName(uchar(peek()))) return parseName();
    if (std::isdigit(peek())) return parseNumber();

    int startLine = line, startCol = col;
    char c = peek();
    advance();

    switch (c)
    {
    case '+' : return make(TypeToken::Op_plus, "+", startLine, startCol);
    case '-' : return make(TypeToken::Op_minus, "-", startLine, startCol);
    case '*' : return make(TypeToken::Op_mul, "*", startLine, startCol);
    case '/' : return make(TypeToken::Op_div, "/", startLine, startCol);
    case ';' : return make(TypeToken::Semi, ";", startLine, startCol);
    case ',' : return make(TypeToken::Comma, ",", startLine, startCol);
    case '.' : return make(TypeToken::Dot, ".", startLine, startCol);
    case ':' : if (peek() == '=') { advance(); return make(TypeToken::Op_assing, ":=", startLine, startCol); } else return make(TypeToken::Colon, ":", startLine, startCol);
    case '(' : return make(TypeToken::LParen, "(", startLine, startCol);
    case ')' : return make(TypeToken::RParen, ")", startLine, startCol);
    case '[' : return make(TypeToken::LBracket, "[", startLine, startCol);
    case ']' : return make(TypeToken::RBracket, "]", startLine, startCol);
    default  : throw std::runtime_error(std::string("Invalid operator : '") + peek() + "' : " + std::to_string(startLine) + " : " + std::to_string(startCol));
    }
}

std::vector<Token> Lexer::tokenize()
{
    std::vector<Token> tokens;
    while (true)
    {
        Token t = next();
        tokens.push_back(t);
        if (t.type == TypeToken::Eof) break;
    }
    return tokens;
}

