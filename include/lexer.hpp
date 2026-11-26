#pragma once

#include <stdexcept>
#include <iostream>
#include <string>
#include <stdint.h>
#include <vector>
#include <unordered_map>
#include <cctype>



enum class TypeToken : uint8_t
{
    Kw_program = 0,         // процедура
    Kw_var,                 // переменные
    Kw_float,               // вещественные
    Kw_int,                 // целые
    Kw_input,               // ввод
    Kw_output,              // передача
    Kw_for,                 // цикл
    Kw_from,                // от
    Kw_to,                  // до
    Kw_step,                // шаг
    Kw_do,                  // выполнить
    Kw_end,                 // конец.

    Name,                   // имя
    Int,                    // число
    Float,                  // вещественное
    Op_assing,              // :=
    Op_plus,                // +
    Op_minus,               // -
    Op_mul,                 // *
    Op_div,                 // /

    Semi,                   // ;
    Comma,                  // ,
    Dot,                    // .
    Colon,                  // :
    LParen,                 // (
    RParen,                 // )
    LBracket,               // [
    RBracket,               // ]
    Eof,                    // eof
};

class Token
{
public:
    TypeToken type;
    std::string lexem;

    int line = 1, col = 1;

    Token(TypeToken type, std::string lexem, int line, int col) : type(type), lexem(lexem), line(line), col(col) {} 
    bool is(TypeToken type) const { return this->type == type; }
};

class Lexer
{
private:
    std::string_view source;
    size_t pos = 0;
    int line = 1, col = 1;

    bool is_end() const { return pos >= source.size(); }
    char peek() const { return (pos < source.size()) ? source[pos] : '\0'; }
    char peek(size_t offset) const { return (pos+offset < source.size()) ? source[pos+offset] : '\0'; }
    Token make(TypeToken type, std::string lexem) { return Token(type, lexem, line, col); }
    Token make(TypeToken type, std::string lexem, int line, int col) { return Token(type, lexem, line, col); }
    void advance();

    bool isStartName(unsigned char c);
    bool isName(unsigned char c);
    Token parseNumber();
    Token parseName();


    bool isSpace();
    void skipCommentSpace();
    
    Token next();

public:
    Lexer(std::string_view source) : source(source) {}
    std::vector<Token> tokenize();

};
