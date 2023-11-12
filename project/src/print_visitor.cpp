//----------------------------------------------------------------------
// FILE: print_visitor.cpp
// DATE: CPSC 326, Spring 2023
// AUTH: Santiago Calvillo
// DESC: Program that creates objects for an AST after a program is given
//----------------------------------------------------------------------

#include "print_visitor.h"

using namespace std;


PrintVisitor::PrintVisitor(ostream& output)
  : out(output)
{
}


void PrintVisitor::inc_indent()
{
  indent += INDENT_AMT;
}


void PrintVisitor::dec_indent()
{
  indent -= INDENT_AMT;
}


void PrintVisitor::print_indent()
{
  out << string(indent, ' ');
}

void PrintVisitor::visit(Program& p)
{
    for (auto struct_def : p.struct_defs)
        struct_def.accept(*this);
    for (auto fun_def : p.fun_defs)
        fun_def.accept(*this);
}


void PrintVisitor::visit(FunDef& f) {
    out << endl;
    if(f.return_type.is_array)
        out << "array ";
    out << f.return_type.type_name;
    out << " ";
    out << f.fun_name.lexeme();
    out << "(";
    int count = 1;
    for(auto varDefs : f.params)
    {
        if(varDefs.data_type.is_array)
            out << "array ";
        out << varDefs.data_type.type_name;
        out << " ";
        out << varDefs.var_name.lexeme();
        if(count < f.params.size()) {
            out << ", ";
        }
        count++;
    }
    out << ") {";
    out << endl;
    inc_indent();
    for (int i = 0; i < f.stmts.size(); i++) {
        print_indent();
        f.stmts[i]->accept(*this);
        out << endl;
    }
    dec_indent();
    out << "}" << endl;
}

void PrintVisitor::visit(StructDef& s){
    out << endl;
    out << "struct ";
    out << s.struct_name.lexeme();
    out << " {" << endl;
    int count = 1;
    inc_indent();
    for(auto varDefs : s.fields)
    {
        print_indent();
        if(varDefs.data_type.is_array)
            out << "array ";
        out << varDefs.data_type.type_name;
        out << " ";
        out << varDefs.var_name.lexeme();
        if(count < s.fields.size()) {
            out << "," << endl;
        }
        count++;
    }
    dec_indent();
    out << endl << "}" << endl;
}
void PrintVisitor::visit(ReturnStmt& s) {
    out << "return ";
    s.expr.accept(*this);
}

void PrintVisitor::visit(WhileStmt& s) {
    out << "while (";
    s.condition.accept(*this);
    out << ") {" << endl;
    inc_indent();
    for (int i = 0; i < s.stmts.size(); i++) {
        print_indent();
        s.stmts[i]->accept(*this);
        out << endl;
    }
    dec_indent();
    print_indent();
    out << "}";
}

void PrintVisitor::visit(ForStmt& s) {
    out << "for (";
    s.var_decl.accept(*this);
    out << "; ";
    s.condition.accept(*this);
    out << "; ";
    s.assign_stmt.accept(*this);
    out << ") {" << endl;
    inc_indent();
    for (int i = 0; i < s.stmts.size(); i++) {
        print_indent();
        s.stmts[i]->accept(*this);
        out << endl;
    }
    dec_indent();
    print_indent();
    out << "}";
}

void PrintVisitor::visit(IfStmt& s) {
    out << "if (";
    s.if_part.condition.accept(*this);
    out << ") {" << endl;
    inc_indent();
    for (int i = 0; i < s.if_part.stmts.size(); i++) {
        print_indent();
        s.if_part.stmts[i]->accept(*this);
        out << endl;
    }
    dec_indent();
    print_indent();
    out << "}" ;
    for (int i = 0; i < s.else_ifs.size(); i++) {
        out << endl;
        print_indent();
        out << "elseif (";
        s.else_ifs[i].condition.accept(*this);
        out << ") {" << endl;
        inc_indent();
        for (int j = 0; j < s.else_ifs[i].stmts.size(); j++) {
            print_indent();
            s.else_ifs[i].stmts[j]->accept(*this);
            out << endl;
        }
        dec_indent();
        print_indent();
        out << "}";
    }
    if(s.else_stmts.size() > 0) {
        out << endl;
        print_indent();
        out << "else {" << endl;
        inc_indent();
        for (int i = 0; i < s.else_stmts.size(); i++) {
            print_indent();
            s.else_stmts[i]->accept(*this);
            out << endl;
        }
        dec_indent();
        print_indent();
        out << "}";
    }
}

void PrintVisitor::visit(VarDeclStmt& s) {
    VarDef varDef = s.var_def;
    if(varDef.data_type.is_array)
        out << "array ";
    out << varDef.data_type.type_name;
    out << " ";
    out << varDef.var_name.lexeme();
    out << " = ";
    s.expr.accept(*this);
}

void PrintVisitor::visit(AssignStmt& s) {
    int count = 1;
    for(auto varRefs : s.lvalue) {
        out << varRefs.var_name.lexeme();
        if (varRefs.array_expr.has_value()) {
            out << "[";
            varRefs.array_expr->accept(*this);
            out << "]";
        }
        if(count < s.lvalue.size())
            out << ".";
        count++;
    }
    out << " = ";
    s.expr.accept(*this);
}

void PrintVisitor::visit(CallExpr& e) {
    out << e.fun_name.lexeme();
    out << "(";
    int count = 1;
    for(auto arg : e.args) {
        arg.accept(*this);
        if(count < e.args.size())
            out << ", ";
        count++;
    }
    out << ")";
}

void PrintVisitor::visit(Expr& e) {
    if(e.negated)
        out << "not (";
    e.first->accept(*this);
    if(e.op.has_value()) {
        out << " " << e.op->lexeme() << " ";
        e.rest->accept(*this);
    }
    if(e.negated)
        out << ")";
}

void PrintVisitor::visit(SimpleTerm& t) {
    t.rvalue->accept(*this);
}

void PrintVisitor::visit(ComplexTerm& t) {
    out << "(";
    t.expr.accept(*this);
    out << ")";
}

void PrintVisitor::visit(SimpleRValue& v) {
    if(v.first_token().type() == TokenType::STRING_VAL) {
        out << "\"";
        out << v.first_token().lexeme();
        out << "\"";
    }
    else if(v.first_token().type() == TokenType::CHAR_VAL) {
        out << "\'";
        out << v.first_token().lexeme();
        out << "\'";
    }
    else
        out << v.first_token().lexeme();
}

void PrintVisitor::visit(NewRValue& v) {
    out << "new ";
    out << v.type.lexeme();
    if (v.array_expr.has_value()) {
        out << "[";
        v.array_expr->accept(*this);
        out << "]";
    }
}

void PrintVisitor::visit(VarRValue& v) {
    int count = 1;
    for(auto varRefs : v.path) {
        out << varRefs.var_name.lexeme();
        if (varRefs.array_expr.has_value()) {
            out << "[";
            varRefs.array_expr->accept(*this);
            out << "]";
        }
        if(count < v.path.size()) {
            out << ".";
        }
        count++;
    }
}