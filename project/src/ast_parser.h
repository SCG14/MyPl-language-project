//----------------------------------------------------------------------
// FILE: ast_parser.h
// DATE: CPSC 326, Spring 2023
// AUTH: Santiago Calvillo
// DESC: ast_parser header file that contains function definitions to be implemented
//----------------------------------------------------------------------

#ifndef AST_PARSER_H
#define AST_PARSER_H

#include "mypl_exception.h"
#include "lexer.h"
#include "ast.h"


class ASTParser
{
public:

  // crate a new recursive descent parer
  ASTParser(const Lexer& lexer);

  // run the parser
  Program parse();
  
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
  void struct_def(Program& p);
  void fun_def(Program& s);
  void fields(StructDef& structDef);
  DataType data_type();
  void params(FunDef& funDef);
  bool base_type();
  void stmt(std::vector<std::shared_ptr<Stmt>>& stmts);
  void expr(Expr& expr);
  void rvalue(SimpleTerm& term);
  bool base_rvalue();
  void vdecl_stmt(VarDeclStmt& decl);
  void assign_stmt(AssignStmt& decl);
  void lvalue(std::vector<VarRef>& values);
  void if_stmt(IfStmt& ifStmt);
  void if_stmt_t(IfStmt& ifStmt);
  void while_stmt(WhileStmt& whileStmt);
  void call_expr(CallExpr& callExpr);
  void new_rvalue(NewRValue& newRValue);
  void var_rvalue(VarRValue& varRValue);
  void for_stmt(ForStmt& forStmt);
  // ...

};


#endif
