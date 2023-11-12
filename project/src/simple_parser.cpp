//----------------------------------------------------------------------
// FILE: simple_parser.cpp
// DATE: CPSC 326, Spring 2023
// AUTH:
// DESC:
//----------------------------------------------------------------------

#include "simple_parser.h"


SimpleParser::SimpleParser(const Lexer& a_lexer)
  : lexer {a_lexer}
{}


void SimpleParser::advance()
{
  curr_token = lexer.next_token();
}


void SimpleParser::eat(TokenType t, const std::string& msg)
{
  if (!match(t))
    error(msg);
  advance();
}


bool SimpleParser::match(TokenType t)
{
  return curr_token.type() == t;
}


bool SimpleParser::match(std::initializer_list<TokenType> types)
{
  for (auto t : types)
    if (match(t))
      return true;
  return false;
}


void SimpleParser::error(const std::string& msg)
{
  std::string s = msg + " found '" + curr_token.lexeme() + "' ";
  s += "at line " + std::to_string(curr_token.line()) + ", ";
  s += "column " + std::to_string(curr_token.column());
  throw MyPLException::ParserError(s);
}


bool SimpleParser::bin_op()
{
  return match({TokenType::PLUS, TokenType::MINUS, TokenType::TIMES,
      TokenType::DIVIDE, TokenType::AND, TokenType::OR, TokenType::EQUAL,
      TokenType::LESS, TokenType::GREATER, TokenType::LESS_EQ,
      TokenType::GREATER_EQ, TokenType::NOT_EQUAL});
}


void SimpleParser::parse()
{
  advance();
  while (!match(TokenType::EOS)) {
    if (match(TokenType::STRUCT))
      struct_def();
    else
      fun_def();
  }
  eat(TokenType::EOS, "expecting end-of-file");
}


void SimpleParser::struct_def() {
  eat(TokenType::STRUCT, "error");
  eat(TokenType::ID, "error");
  eat(TokenType::LBRACE, "error");
  fields();
  eat(TokenType::RBRACE, "error");
}


void SimpleParser::fun_def() {
  if (match(TokenType::VOID_TYPE))
    eat(TokenType::VOID_TYPE, "error");
  else
    data_type();
  eat(TokenType::ID, "error");
  eat(TokenType::LPAREN, "error");
  params();
  eat(TokenType::RPAREN, "error");
  eat(TokenType::LBRACE, "error");
  if(!match(TokenType::EOS))
    while(!match({TokenType::RBRACE, TokenType::EOS}))
      stmt();
  eat(TokenType::RBRACE, "error");
}

// TODO: Implement the rest of your recursive descent functions
//       here. See simple_parser.h

void SimpleParser::fields() {
  if(!match(TokenType::RBRACE)) {
    data_type();
    eat(TokenType::ID, "error");
    while (match(TokenType::COMMA)) {
      eat(TokenType::COMMA, "error");
      data_type();
      eat(TokenType::ID, "error");
    }
  }
}

void SimpleParser::data_type() {
  if(match(TokenType::ID))
    advance();
  else if(match(TokenType::ARRAY)) {
    advance();
    if(base_type())
      advance();
    else
      eat(TokenType::ID, "error");
  }
  else
    if(base_type())
      advance();
}

bool SimpleParser::base_type() {
  return match({TokenType::INT_TYPE, TokenType::DOUBLE_TYPE, TokenType::BOOL_TYPE,
                TokenType::CHAR_TYPE, TokenType::STRING_TYPE});

}

void SimpleParser::params() {
  if(!match(TokenType::RPAREN)) {
    data_type();
    eat(TokenType::ID, "error");
    while (match(TokenType::COMMA)) {
      eat(TokenType::COMMA, "error");
      data_type();
      eat(TokenType::ID, "error");
    }
  }
}

void SimpleParser::stmt() {
  if(match(TokenType::RETURN))
    ret_stmt();
  else if(match(TokenType::IF))
    if_stmt();
  else if(match(TokenType::WHILE))
    while_stmt();
  else if(match(TokenType::FOR))
    for_stmt();
  else if(base_type() || match(TokenType::ARRAY))
    vdecl_stmt();
  else if(match(TokenType::ID)) {
    advance();
    if(match(TokenType::LPAREN))
      call_expr();
    else if(match(TokenType::ID))
      vdecl_stmt();
    else
      assign_stmt();
  }
  else
    error("error");
}

void SimpleParser::vdecl_stmt() {
  if(!match(TokenType::ID))
    data_type();
  eat(TokenType::ID, "error");
  eat(TokenType::ASSIGN, "error");
  expr();
}

void SimpleParser::assign_stmt() {
  lvalue();
  eat(TokenType::ASSIGN, "error");
  expr();
}

void SimpleParser::lvalue() {
  while (match({TokenType::DOT, TokenType::LBRACKET})) {
    if(match(TokenType::DOT)) {
      eat(TokenType::DOT, "error");
      eat(TokenType::ID, "error");
    }
    else if(match(TokenType::LBRACKET)) {
      eat(TokenType::LBRACKET, "error");
      expr();
      eat(TokenType::RBRACKET, "error");
    }
  }
}

void SimpleParser::if_stmt() {
  eat(TokenType::IF, "error");
  eat(TokenType::LPAREN, "error");
  expr();
  eat(TokenType::RPAREN, "error");
  eat(TokenType::LBRACE, "error");
  while(!match(TokenType::RBRACE))
    stmt();
  eat(TokenType::RBRACE, "error");
  if_stmt_t();
}

void SimpleParser::if_stmt_t() {

  if(match(TokenType::ELSEIF)) {
    eat(TokenType::ELSEIF, "error");
    eat(TokenType::LPAREN, "error");
    expr();
    eat(TokenType::RPAREN, "error");
    eat(TokenType::LBRACE, "error");
    while(!match(TokenType::RBRACE))
      stmt();
    eat(TokenType::RBRACE, "error");
    if_stmt_t();
  }
  else if(match(TokenType::ELSE)) {
    eat(TokenType::ELSE, "error");
    eat(TokenType::LBRACE, "error");
    while(!match(TokenType::RBRACE))
      stmt();
    eat(TokenType::RBRACE, "error");
  }


}

void SimpleParser::while_stmt() {
  eat(TokenType::WHILE, "error");
  eat(TokenType::LPAREN, "error");
  expr();
  eat(TokenType::RPAREN, "error");
  eat(TokenType::LBRACE, "error");
  while(!match(TokenType::RBRACE))
    stmt();
  eat(TokenType::RBRACE, "error");
}

void SimpleParser::for_stmt() {
  eat(TokenType::FOR, "error");
  eat(TokenType::LPAREN, "error");
  vdecl_stmt();
  eat(TokenType::SEMICOLON, "error");
  expr();
  eat(TokenType::SEMICOLON, "error");
  eat(TokenType::ID, "error");
  assign_stmt();
  eat(TokenType::RPAREN, "error");
  eat(TokenType::LBRACE, "error");
  while(!match(TokenType::RBRACE))
    stmt();
  eat(TokenType::RBRACE, "error");
}

void SimpleParser::call_expr() {
  eat(TokenType::LPAREN, "error");
  if(!match(TokenType::RPAREN)) {
    expr();
    while(!match(TokenType::RPAREN)) {
      eat(TokenType::COMMA, "error");
      expr();
    }
  }
  eat(TokenType::RPAREN, "error");
}

void SimpleParser::ret_stmt() {
  eat(TokenType::RETURN, "error");
  expr();
}

void SimpleParser::expr() {
  if(match(TokenType::NOT)) {
    eat(TokenType::NOT, "error");
    expr();
  }
  else if(match(TokenType::LPAREN)) {
    eat(TokenType::LPAREN, "error");
    expr();
    eat(TokenType::RPAREN, "error");
  }
  else
    rvalue();

  if(bin_op()) {
    advance();
    expr();
  }
}

void SimpleParser::rvalue() {
  if(match(TokenType::NEW))
    new_rvalue();
  else if(base_rvalue())
    advance();
  else if(match(TokenType::ID)){
    advance();
    if(match(TokenType::LPAREN))
      call_expr();
    else if(match({TokenType::DOT,TokenType::LBRACKET}))
      var_rvalue();
  }
  else
    error("error");
}

void SimpleParser::new_rvalue() {
  eat(TokenType::NEW, "error");
  if(base_type()) {
    advance();
    eat(TokenType::LBRACKET, "error");
    expr();
    eat(TokenType::RBRACKET, "error");
  }
  else if(match(TokenType::ID)) {
    advance();
    if(match(TokenType::LBRACKET)) {
      advance();
      expr();
      eat(TokenType::RBRACKET, "error");
    }
  }
  else
    error("error");
}

bool SimpleParser::base_rvalue() {
  return match({TokenType::INT_VAL,TokenType::DOUBLE_VAL,TokenType::BOOL_VAL,
                TokenType::CHAR_VAL,TokenType::STRING_VAL,TokenType::NULL_VAL});
}

void SimpleParser::var_rvalue() {
  while (match({TokenType::DOT, TokenType::LBRACKET})) {
    if(match(TokenType::DOT)) {
      eat(TokenType::DOT, "error");
      eat(TokenType::ID, "error");
    }
    else if(match(TokenType::LBRACKET)) {
      eat(TokenType::LBRACKET, "error");
      expr();
      eat(TokenType::RBRACKET, "error");
    }
  }
}
