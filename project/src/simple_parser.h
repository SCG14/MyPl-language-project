//----------------------------------------------------------------------
// FILE: simple_parser.h
// DATE: CPSC 326, Spring 2023
// AUTH: 
// DESC: Basic interface for our Simple Parser (syntax checker)
//----------------------------------------------------------------------

#ifndef SIMPLE_PARSER_H
#define SIMPLE_PARSER_H

#include "mypl_exception.h"
#include "lexer.h"


class SimpleParser
{
public:

  // crate a new recursive descent parer
  SimpleParser(const Lexer& lexer);

  // run the parser
  void parse();
  
private:
  
  Lexer lexer;
  Token curr_token;
  
  // helper functions
  void advance();
  void eat(TokenType t, const std::string& msg);
  bool match(TokenType t);
  bool match(std::initializer_list<TokenType> types);
  void error(const std::string& msg);
  bool bin_op();

  // recursive descent functions
  void struct_def();
  void fun_def();

  // TODO:
  //   -- Add remaining recursive descent function declarations here.
  //   -- Note that you need to follow the general flow of the MyPL
  //      syntax specification, but there may be a few places you will
  //      want to simplify and/or diverge.
  //   -- Be sure to remove this comment block when you are finished.
  void fields();
  void data_type();
  bool base_type();
  void params();
  void stmt();
  void vdecl_stmt();
  void assign_stmt();
  void lvalue();
  void if_stmt();
  void if_stmt_t();
  void while_stmt();
  void for_stmt();
  void call_expr();
  void ret_stmt();
  void expr();
  void rvalue();
  void new_rvalue();
  bool base_rvalue();
  void var_rvalue();
};


#endif
