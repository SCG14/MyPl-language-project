//----------------------------------------------------------------------
// FILE: token.cpp
// DATE: CPSC 326, Spring 2023
// AUTH: S. Bowers
// DESC: Token implementation
//----------------------------------------------------------------------

#include <unordered_map>
#include "token.h"


Token::Token()
  : token_type {TokenType::EOS}, token_lexeme {""}, token_line {0},
    token_column {0}
{}

Token::Token(TokenType type, const std::string& lexeme, int line, int column)
  : token_type {type}, token_lexeme {lexeme}, token_line {line},
    token_column {column}
{}

TokenType Token::type() const
{
  return token_type;
}

std::string Token::lexeme() const
{
  return token_lexeme;
}

int Token::line() const
{
  return token_line;
}

int Token::column() const
{
  return token_column;
}

std::string to_string(const Token& token)
{
  std::unordered_map<TokenType,std::string> ts = {
    // end-of-stream
    {TokenType::EOS, "EOS"}, {TokenType::ID, "ID"},
    // punctuation
    {TokenType::DOT, "DOT"}, {TokenType::COMMA, "COMMA"},
    {TokenType::LPAREN, "LPAREN"}, {TokenType::RPAREN, "RPAREN"},
    {TokenType::LBRACKET, "LBRACKET"}, {TokenType::RBRACKET, "RBRACKET"},
    {TokenType::SEMICOLON, "SEMICOLON"}, {TokenType::RBRACE, "RBRACE"},
    {TokenType::LBRACE, "LBRACE"},
    // operators
    {TokenType::PLUS, "PLUS"}, {TokenType::MINUS, "MINUS"},
    {TokenType::TIMES, "TIMES"}, {TokenType::DIVIDE, "DIVIDE"},
    {TokenType::ASSIGN, "ASSIGN"}, 
    // comparators
    {TokenType::LESS, "LESS"}, {TokenType::GREATER, "GREATER"},    
    {TokenType::LESS_EQ, "LESS_EQ"}, {TokenType::GREATER_EQ, "GREATER_EQ"},
    {TokenType::EQUAL, "EQUAL"}, {TokenType::NOT_EQUAL, "NOT_EQUAL"},    
    // values
    {TokenType::INT_VAL, "INT_VAL"}, {TokenType::DOUBLE_VAL, "DOUBLE_VAL"},        
    {TokenType::CHAR_VAL, "CHAR_VAL"}, {TokenType::STRING_VAL, "STRING_VAL"},        
    {TokenType::BOOL_VAL, "BOOL_VAL"}, {TokenType::NULL_VAL, "NULL_VAL"},
    // primitive types
    {TokenType::INT_TYPE, "INT_TYPE"}, {TokenType::DOUBLE_TYPE, "DOUBLE_TYPE"},
    {TokenType::BOOL_TYPE, "BOOL_TYPE"}, {TokenType::STRING_TYPE, "STRING_TYPE"},
    {TokenType::CHAR_TYPE, "CHAR_TYPE"}, {TokenType::VOID_TYPE, "VOID_TYPE"},
    // reserved words
    {TokenType::STRUCT, "STRUCT"}, {TokenType::ARRAY, "ARRAY"},
    {TokenType::FOR, "FOR"}, {TokenType::WHILE, "WHILE"},
    {TokenType::IF, "IF"}, {TokenType::ELSEIF, "ELSEIF"},
    {TokenType::ELSE, "ELSE"}, {TokenType::AND, "AND"},
    {TokenType::OR, "OR"}, {TokenType::NOT, "NOT"},
    {TokenType::NEW, "NEW"}, {TokenType::RETURN, "RETURN"}
  };
  return std::to_string(token.line()) + ", "
    + std::to_string(token.column()) + ": "
    + ts[token.type()] + " '" +  token.lexeme() + "'";
}
