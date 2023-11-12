//----------------------------------------------------------------------
// FILE: print_visitor.h
// DATE: CPSC 326, Spring 2023
// AUTH: S. Bowers
// DESC: Visitor interface for pretty printing MyPL code.
//----------------------------------------------------------------------


#ifndef PRINT_VISITOR_H
#define PRINT_VISITOR_H

#include <ostream>
#include "ast.h"
#include <fstream>

class PrintVisitor : public Visitor {
public:
  PrintVisitor(std::ostream& output);
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
  std::ostream& out;  
  int indent = 0;
  const int INDENT_AMT = 2;

  void inc_indent();
  void dec_indent();
  void print_indent();
  
};

class CSharpPrintVisitor : public Visitor {
public:
    CSharpPrintVisitor(const std::string& filename);
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
    std::ofstream out;
    int indent = 0;
    const int INDENT_AMT = 2;

    void inc_indent();
    void dec_indent();
    void print_indent();

};
#endif
