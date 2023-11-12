//----------------------------------------------------------------------
// FILE: ast.h
// DATE: Spring 2023
// AUTH: S. Bowers
// DESC: Interface for MyPL abstract syntax tree (plain-old data) types
//----------------------------------------------------------------------


// NOTE: Guiding principle is to use heap as little as possible and
// only use pointers when necessary


#ifndef AST_H
#define AST_H

#include <vector>
#include <memory>
#include <optional>
#include "token.h"


// forward declarations
class Program;
class FunDef;
class StructDef;
class ReturnStmt;
class WhileStmt;
class ForStmt;
class IfStmt;
class VarDeclStmt;
class AssignStmt;
class CallExpr;
class Expr;
class SimpleTerm;
class ComplexTerm;
class SimpleRValue;
class NewRValue;
class VarRValue;

//----------------------------------------------------------------------
// Visitor interface
//----------------------------------------------------------------------

class Visitor {
public:
  // top-level
  virtual void visit(Program& p) = 0;
  virtual void visit(FunDef& f) = 0;
  virtual void visit(StructDef& s) = 0;
  // statements
  virtual void visit(ReturnStmt& s) = 0;
  virtual void visit(WhileStmt& s) = 0;  
  virtual void visit(ForStmt& s) = 0;  
  virtual void visit(IfStmt& s) = 0;  
  virtual void visit(VarDeclStmt& s) = 0;  
  virtual void visit(AssignStmt& s) = 0;  
  virtual void visit(CallExpr& e) = 0;  
  virtual void visit(Expr& e) = 0;  
  virtual void visit(SimpleTerm& t) = 0;  
  virtual void visit(ComplexTerm& t) = 0;  
  virtual void visit(SimpleRValue& v) = 0;  
  virtual void visit(NewRValue& v) = 0;  
  virtual void visit(VarRValue& v) = 0;  
};


//----------------------------------------------------------------------
// Top-level Abstract AST Nodes
//----------------------------------------------------------------------

class ASTNode
{
public:
  virtual ~ASTNode() {};
  virtual void accept(Visitor& v) = 0;
};

class Stmt : public ASTNode
{
};

class ExprTerm : public ASTNode
{
public:
  // helper to return first token in the term
  virtual Token first_token() = 0;
};

class RValue : public ASTNode
{
public:
  // helper to return first token in the rvalue
  virtual Token first_token() = 0;
};


//----------------------------------------------------------------------
// Program-related types
//----------------------------------------------------------------------


class Program : public ASTNode
{
public:
  std::vector<StructDef> struct_defs;
  std::vector<FunDef> fun_defs;
  void accept(Visitor& v) { v.visit(*this); }
};


class DataType
{
public:
  bool is_array = false;
  std::string type_name;
};


class VarDef
{
public:
  DataType data_type;
  Token var_name;
  Token first_token() {return var_name;}
};

class StructDef : public ASTNode
{
public:
  Token struct_name;
  std::vector<VarDef> fields;
  void accept(Visitor& v) { v.visit(*this); }  
};


class FunDef : public ASTNode
{
public:
  DataType return_type;
  Token fun_name;
  std::vector<VarDef> params;
  std::vector<std::shared_ptr<Stmt>> stmts;
  void accept(Visitor& v) { v.visit(*this); }  
};


//----------------------------------------------------------------------
// Expression-related types
//----------------------------------------------------------------------


class Expr : public ASTNode
{
public:
  bool negated = false;
  std::shared_ptr<ExprTerm> first = nullptr;
  std::optional<Token> op = std::nullopt;
  std::shared_ptr<Expr> rest = nullptr;
  void accept(Visitor& v) { v.visit(*this); }  
  Token first_token() {return first->first_token();}
};

class SimpleTerm : public ExprTerm
{
public:
  std::shared_ptr<RValue> rvalue = nullptr;
  void accept(Visitor& v) { v.visit(*this); }
  Token first_token() {return rvalue->first_token();}
};


class ComplexTerm : public ExprTerm
{
public:
  Expr expr;
  void accept(Visitor& v) { v.visit(*this); }      
  Token first_token() {return expr.first_token();}
};


class SimpleRValue : public RValue
{
public:
  Token value;
  void accept(Visitor& v) { v.visit(*this); }        
  Token first_token() {return value;}
};


class NewRValue : public RValue
{
public:
  Token type;
  std::optional<Expr> array_expr;
  void accept(Visitor& v) { v.visit(*this); }        
  Token first_token() {return type;}
};


class VarRef
{
public:
  Token var_name;
  std::optional<Expr> array_expr = std::nullopt; 
};


class VarRValue : public RValue
{
public:
  std::vector<VarRef> path;
  void accept(Visitor& v) { v.visit(*this); }        
  Token first_token() {return path[0].var_name;}
};


//----------------------------------------------------------------------
// Statement-related types
//----------------------------------------------------------------------


class ReturnStmt : public Stmt
{
public:
  Expr expr;
  void accept(Visitor& v) { v.visit(*this); }  
};


class WhileStmt : public Stmt
{
public:
  Expr condition;
  std::vector<std::shared_ptr<Stmt>> stmts;
  void accept(Visitor& v) { v.visit(*this); }  
};


class VarDeclStmt : public Stmt
{
public:
  VarDef var_def;
  Expr expr;
  void accept(Visitor& v) { v.visit(*this); }  
};


class AssignStmt : public Stmt
{
public:
  std::vector<VarRef> lvalue;
  Expr expr;
  void accept(Visitor& v) { v.visit(*this); }  
};


class ForStmt : public Stmt
{
public:
  VarDeclStmt var_decl;
  Expr condition;
  AssignStmt assign_stmt;
  std::vector<std::shared_ptr<Stmt>> stmts;
  void accept(Visitor& v) { v.visit(*this); }  
};


class BasicIf
{
public:
  Expr condition;
  std::vector<std::shared_ptr<Stmt>> stmts;
};


class IfStmt : public Stmt
{
public:
  BasicIf if_part;
  std::vector<BasicIf> else_ifs;
  std::vector<std::shared_ptr<Stmt>> else_stmts;
  void accept(Visitor& v) { v.visit(*this); }  
};


class CallExpr : public Stmt, public RValue
{
public:
  Token fun_name;
  std::vector<Expr> args;
  void accept(Visitor& v) { v.visit(*this); }  
  Token first_token() {return fun_name;}
};


#endif
