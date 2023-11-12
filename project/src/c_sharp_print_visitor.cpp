//----------------------------------------------------------------------
// FILE: print_visitor.cpp
// DATE: CPSC 326, Spring 2023
// AUTH: Santiago Calvillo
// DESC: Program that creates objects for an AST after a program is given
//----------------------------------------------------------------------

#include "print_visitor.h"

using namespace std;


CSharpPrintVisitor::CSharpPrintVisitor(const std::string& filename)
  : out(filename)
{
}


void CSharpPrintVisitor::inc_indent()
{
  indent += INDENT_AMT;
}


void CSharpPrintVisitor::dec_indent()
{
  indent -= INDENT_AMT;
}


void CSharpPrintVisitor::print_indent()
{
  out << string(indent, ' ');
}

void CSharpPrintVisitor::visit(Program& p)
{
    out << "using System;" << endl<<endl;
    out << "class CSharp {" << endl;
    inc_indent();
    for (auto struct_def : p.struct_defs)
        struct_def.accept(*this);
    for (auto fun_def : p.fun_defs)
        fun_def.accept(*this);
    dec_indent();
    out << "}" << endl;
}


void CSharpPrintVisitor::visit(FunDef& f) {
    out << endl;
    print_indent();
    if(f.fun_name.lexeme() == "main") {
        out << "static void Main(string[] args) {";
    }
    else {
        out << "static ";
        out << f.return_type.type_name;
        if(f.return_type.is_array)
            out << "[] ";
        out << " ";
        out << f.fun_name.lexeme();
        out << "(";
        int count = 1;
        for(auto varDefs : f.params)
        {
            out << varDefs.data_type.type_name;
            if(varDefs.data_type.is_array)
                out << "[] ";
            out << " ";
            out << varDefs.var_name.lexeme();
            if(count < f.params.size()) {
                out << ", ";
            }
            count++;
        }
        out << ") {";
    }

    out << endl;
    inc_indent();
    for (int i = 0; i < f.stmts.size(); i++) {
        print_indent();
        f.stmts[i]->accept(*this);
        out << ";" <<endl;
    }
    dec_indent();
    print_indent();
    out << "}" << endl;
}

void CSharpPrintVisitor::visit(StructDef& s){
    out << endl;
    print_indent();
    out << "public struct ";
    out << s.struct_name.lexeme();
    out << " {" << endl;
    int count = 1;
    inc_indent();
    for(auto varDefs : s.fields)
    {
        print_indent();
        out << "public ";
        out << varDefs.data_type.type_name;
        if(varDefs.data_type.is_array)
            out << "[]";
        out << " ";
        out << varDefs.var_name.lexeme();
        out << ";" << endl;
        count++;
    }
    dec_indent();
    print_indent();
    out << "}" << endl;
}
void CSharpPrintVisitor::visit(ReturnStmt& s) {
    out << "return ";
    s.expr.accept(*this);
}

void CSharpPrintVisitor::visit(WhileStmt& s) {
    out << "while (";
    s.condition.accept(*this);
    out << ") {" << endl;
    inc_indent();
    for (int i = 0; i < s.stmts.size(); i++) {
        print_indent();
        s.stmts[i]->accept(*this);
        out << ";" <<  endl;
    }
    dec_indent();
    print_indent();
    out << "}";
}

void CSharpPrintVisitor::visit(ForStmt& s) {
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
        out << ";" <<  endl;
    }
    dec_indent();
    print_indent();
    out << "}";
}

void CSharpPrintVisitor::visit(IfStmt& s) {
    out << "if (";
    s.if_part.condition.accept(*this);
    out << ") {" << endl;
    inc_indent();
    for (int i = 0; i < s.if_part.stmts.size(); i++) {
        print_indent();
        s.if_part.stmts[i]->accept(*this);
        out << ";" <<  endl;
    }
    dec_indent();
    print_indent();
    out << "}" ;
    for (int i = 0; i < s.else_ifs.size(); i++) {
        out << endl;
        print_indent();
        out << "else if (";
        s.else_ifs[i].condition.accept(*this);
        out << ") {" << endl;
        inc_indent();
        for (int j = 0; j < s.else_ifs[i].stmts.size(); j++) {
            print_indent();
            s.else_ifs[i].stmts[j]->accept(*this);
            out << ";" <<  endl;
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
            out << ";" <<  endl;
        }
        dec_indent();
        print_indent();
        out << "}";
    }
}

void CSharpPrintVisitor::visit(VarDeclStmt& s) {
    VarDef varDef = s.var_def;

    out << varDef.data_type.type_name;
    if(varDef.data_type.is_array)
        out << "[]";
    out << " ";
    out << varDef.var_name.lexeme();
    out << " = ";
    s.expr.accept(*this);
}

void CSharpPrintVisitor::visit(AssignStmt& s) {
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

void CSharpPrintVisitor::visit(CallExpr& e) {
    if(e.fun_name.lexeme() == "length") {
        e.args[0].accept(*this);
        out << ".Length";
    }
    else if(e.fun_name.lexeme() == "get") {
        e.args[1].accept(*this);
        out << "[";
        e.args[0].accept(*this);
        out << "]";
    }
    else if (e.fun_name.lexeme() == "to_string") {
        e.args[0].accept(*this);
        out << ".ToString()";
    }
    else {

        if (e.fun_name.lexeme() == "print") {
            out << "Console.Write";
        } else if (e.fun_name.lexeme() == "input") {
            out << "Console.ReadLine";
        } else if (e.fun_name.lexeme() == "concat") {
            out << "String.Concat";
        } else if (e.fun_name.lexeme() == "to_double") {
            out << "Convert.ToDouble";
        } else if (e.fun_name.lexeme() == "to_int") {
            out << "Convert.ToInt32";
        } else out << e.fun_name.lexeme();

        out << "(";
        int count = 1;
        for (auto arg: e.args) {
            arg.accept(*this);
            if (count < e.args.size())
                out << ", ";
            count++;
        }
        out << ")";
    }

}

void CSharpPrintVisitor::visit(Expr& e) {
    if(e.negated)
        out << "!(";
    e.first->accept(*this);
    if(e.op.has_value()) {
        string op = e.op->lexeme();
        string new_op = "";
        if(op == "and")
            new_op = "&&";
        else if(op == "or")
            new_op = "||";
        else
            new_op = e.op->lexeme();
        out << " " << new_op << " ";
        e.rest->accept(*this);
    }
    if(e.negated)
        out << ")";
}

void CSharpPrintVisitor::visit(SimpleTerm& t) {
    t.rvalue->accept(*this);
}

void CSharpPrintVisitor::visit(ComplexTerm& t) {
    out << "(";
    t.expr.accept(*this);
    out << ")";
}

void CSharpPrintVisitor::visit(SimpleRValue& v) {
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

void CSharpPrintVisitor::visit(NewRValue& v) {
    out << "new ";
    out << v.type.lexeme();
    if (v.array_expr.has_value()) {
        out << "[";
        v.array_expr->accept(*this);
        out << "]";
    }
    else
        out << "()";
}

void CSharpPrintVisitor::visit(VarRValue& v) {
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