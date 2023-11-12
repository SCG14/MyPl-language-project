//----------------------------------------------------------------------
// FILE: token.h
// DATE: CPSC 326, Spring 2023
// AUTH: S. Bowers
// DESC: Interface for MyPL token objects
//----------------------------------------------------------------------

#ifndef TOKEN_H
#define TOKEN_H

#include <string>


enum class TokenType {
  // end-of-stream and identifiers
  EOS, ID, 
  // punctuation
  DOT, COMMA, LPAREN, RPAREN, LBRACKET, RBRACKET, SEMICOLON, LBRACE, RBRACE,
  // operators
  PLUS, MINUS, TIMES, DIVIDE, ASSIGN, 
  // comparators
  LESS, GREATER, LESS_EQ, GREATER_EQ, EQUAL, NOT_EQUAL,
  // values
  INT_VAL, DOUBLE_VAL, CHAR_VAL, STRING_VAL, BOOL_VAL, NULL_VAL,
  // primitive data types
  INT_TYPE, DOUBLE_TYPE, BOOL_TYPE, STRING_TYPE, CHAR_TYPE, VOID_TYPE, 
  // reserved words
  STRUCT, ARRAY, FOR, WHILE, IF, ELSEIF, ELSE, AND, OR, NOT, NEW, RETURN
};


class Token
{
public:

  // default constructor
  Token();
  // constructor
  Token(TokenType type, const std::string& lexeme, int line, int colum);
  // returns the type of the token
  TokenType type() const;
  // returns the lexeme of the token
  std::string lexeme() const;
  // returns the line of the token
  int line() const;
  // returns the column of the token
  int column() const;
  // returns the token as a printable string
  friend std::string to_string(const Token& token);

private:

  // the type of the token
  TokenType token_type;
  // the token's lexeme
  std::string token_lexeme;
  // line the token occurs on
  int token_line;
  // starting column of the token
  int token_column;

};


#endif
