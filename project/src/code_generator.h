//----------------------------------------------------------------------
// FILE: code_generator.h
// DATE: CPSC 326, Spring 2023
// AUTH: S. Bowers
// DESC: Interface for the code generator visitor.
//----------------------------------------------------------------------


#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include <string>
#include <unordered_map>
#include "ast.h"
#include "var_table.h"
#include "vm.h"


class CodeGenerator : public Visitor {
public:
  CodeGenerator(VM& vm);
  void visit(Program& p);
  void visit(FunDef& f);
  void visit(StructDef& s);
  void visit(ReturnStmt& s);
  void visit(WhileStmt& s);
  void visit(ForStmt& s);
  void visit(IfStmt& s);
  void visit(VarDeclStmt& s);
  void visit(AssignStmt& s);
  void visit(CallExpr& e);
  void visit(Expr& e);
  void visit(SimpleTerm& t); 
  void visit(ComplexTerm& t);
  void visit(SimpleRValue& v);
  void visit(NewRValue& v);
  void visit(VarRValue& v);    

private:

  VM& vm;
  VMFrameInfo curr_frame;
  int next_var_index = 0;  
  VarTable var_table;
  std::unordered_map<std::string,StructDef> struct_defs;

};

#endif
