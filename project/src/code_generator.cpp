//----------------------------------------------------------------------
// FILE: code_generator.cpp
// DATE: CPSC 326, Spring 2023
// AUTH: Santiago Calvillo
// DESC: Program that takes an AST and converts it into machine instructions
//----------------------------------------------------------------------

#include <iostream>             // for debugging
#include "code_generator.h"

using namespace std;


// helper function to replace all occurrences of old string with new
void replace_all(string& s, const string& old_str, const string& new_str)
{
  while (s.find(old_str) != string::npos)
    s.replace(s.find(old_str), old_str.size(), new_str);
}


CodeGenerator::CodeGenerator(VM& vm)
  : vm(vm)
{
}


void CodeGenerator::visit(Program& p)
{
  for (auto& struct_def : p.struct_defs)
    struct_def.accept(*this);
  for (auto& fun_def : p.fun_defs)
    fun_def.accept(*this);
}


void CodeGenerator::visit(FunDef& f)
{
    var_table.push_environment();
    curr_frame = {f.fun_name.lexeme(), (int)f.params.size()};
    // check every param
    int count = 0;
    for(auto varDef : f.params) {
        string name = varDef.var_name.lexeme();
        curr_frame.instructions.push_back(VMInstr::STORE(count));
        count++;
        var_table.add(varDef.var_name.lexeme());
    }

    for(auto s : f.stmts)
        s->accept(*this);

    if(f.return_type.type_name == "void") {
        curr_frame.instructions.push_back(VMInstr::PUSH(nullptr));
        curr_frame.instructions.push_back(VMInstr::RET());
    }

    // add function frame
    vm.add(curr_frame);
    var_table.pop_environment();

}


void CodeGenerator::visit(StructDef& s)
{
    struct_defs[s.struct_name.lexeme()] = s;
}


void CodeGenerator::visit(ReturnStmt& s)
{
    s.expr.accept(*this);
    curr_frame.instructions.push_back(VMInstr::RET());
}


void CodeGenerator::visit(WhileStmt& s)
{
    // index it will jump to each time
    int jump_index = curr_frame.instructions.size();
    s.condition.accept(*this);
    int jump_false_index = curr_frame.instructions.size();
    curr_frame.instructions.push_back(VMInstr::JMPF(-1));
    var_table.push_environment();
    for (int i = 0; i < s.stmts.size(); i++)
        s.stmts[i]->accept(*this);
    var_table.pop_environment();
    // jump to start
    curr_frame.instructions.push_back(VMInstr::JMP(jump_index));
    curr_frame.instructions.push_back(VMInstr::NOP());
    // set jmpf index
    int index = curr_frame.instructions.size() - 1;
    curr_frame.instructions[jump_false_index].set_operand(index);
}


void CodeGenerator::visit(ForStmt& s)
{
    var_table.push_environment();
    s.var_decl.accept(*this);
    // do decl and get jmp index for later
    int jump_index = curr_frame.instructions.size();
    s.condition.accept(*this);
    int jump_false_index = curr_frame.instructions.size();
    curr_frame.instructions.push_back(VMInstr::JMPF(-1));
    var_table.push_environment();
    for (int i = 0; i < s.stmts.size(); i++)
        s.stmts[i]->accept(*this);
    var_table.pop_environment();
    s.assign_stmt.accept(*this);
    curr_frame.instructions.push_back(VMInstr::JMP(jump_index));
    curr_frame.instructions.push_back(VMInstr::NOP());
    int index = curr_frame.instructions.size()- 1;
    curr_frame.instructions[jump_false_index].set_operand(index);
    var_table.pop_environment();
}


void CodeGenerator::visit(IfStmt& s)
{
    // condition needs to be bool
    s.if_part.condition.accept(*this);
    int if_jmpf_index = curr_frame.instructions.size();
    curr_frame.instructions.push_back(VMInstr::JMPF(-1));

    // new environment within if block
    var_table.push_environment();
    for (int i = 0; i < s.if_part.stmts.size(); i++)
        s.if_part.stmts[i]->accept(*this);
    var_table.pop_environment();

    std::vector<int> jmp_indexes;
    jmp_indexes.push_back(int(curr_frame.instructions.size()));
    curr_frame.instructions.push_back(VMInstr::JMP(-1));
    curr_frame.instructions[if_jmpf_index].set_operand(int(curr_frame.instructions.size()));
    // check for every if else condition bool and new environment
    for (int i = 0; i < s.else_ifs.size(); i++) {
        s.else_ifs[i].condition.accept(*this);
        int ifelse_jmpf_index = curr_frame.instructions.size();
        curr_frame.instructions.push_back(VMInstr::JMPF(-1));
        var_table.push_environment();
        for (int j = 0; j < s.else_ifs[i].stmts.size(); j++)
            s.else_ifs[i].stmts[j]->accept(*this);
        var_table.pop_environment();
        jmp_indexes.push_back(int(curr_frame.instructions.size()));
        curr_frame.instructions.push_back(VMInstr::JMP(-1));
        curr_frame.instructions[ifelse_jmpf_index].set_operand(int(curr_frame.instructions.size()));
    }
    // else statement blocks run
    if(s.else_stmts.size() > 0) {
        var_table.push_environment();
        for (int i = 0; i < s.else_stmts.size(); i++)
            s.else_stmts[i]->accept(*this);

        var_table.pop_environment();
    }
    int final_index = curr_frame.instructions.size();
    for(int i = 0; i < jmp_indexes.size(); i++)
        curr_frame.instructions[jmp_indexes[i]].set_operand(final_index);
}


void CodeGenerator::visit(VarDeclStmt& s)
{
    VarDef varDef = s.var_def;
    string var_name = varDef.var_name.lexeme();
    var_table.add(var_name);
    s.expr.accept(*this);
    int var_index = var_table.get(var_name);
    curr_frame.instructions.push_back(VMInstr::STORE(var_index));
}


void CodeGenerator::visit(AssignStmt& s)
{
    // check all items except last one
    for (int i = 0; i < s.lvalue.size() - 1; ++i) {
        if (i == 0)
            curr_frame.instructions.push_back(VMInstr::LOAD(var_table.get(s.lvalue[0].var_name.lexeme())));
        else
            curr_frame.instructions.push_back(VMInstr::GETF((s.lvalue[i].var_name.lexeme())));

        if(s.lvalue[i].array_expr.has_value()) {
            s.lvalue[i].array_expr.value().accept(*this);
            curr_frame.instructions.push_back(VMInstr::GETI());
        }
    }

    // check if last item has expr or just needs to store or set field
    if (s.lvalue[s.lvalue.size() - 1].array_expr.has_value()) {
        if (s.lvalue.size() == 1)
            curr_frame.instructions.push_back(VMInstr::LOAD(var_table.get(s.lvalue[0].var_name.lexeme())));
        else
            curr_frame.instructions.push_back(VMInstr::GETF((s.lvalue[s.lvalue.size() - 1].var_name.lexeme())));
        s.lvalue[s.lvalue.size() - 1].array_expr.value().accept(*this);
        s.expr.accept(*this);
        curr_frame.instructions.push_back(VMInstr::SETI());
    }
    else if (s.lvalue.size() > 1) {
        s.expr.accept(*this);
        curr_frame.instructions.push_back(VMInstr::SETF(s.lvalue[s.lvalue.size() - 1].var_name.lexeme()));
    }
    else {
        s.expr.accept(*this);
        curr_frame.instructions.push_back(VMInstr::STORE(var_table.get(s.lvalue[s.lvalue.size() - 1].var_name.lexeme())));
    }
}


void CodeGenerator::visit(CallExpr& e)
{
    // do params and do for each instruction
    string fun_name = e.fun_name.lexeme();
    for (int i = 0; i < e.args.size(); i++)
        e.args[i].accept(*this);

    if (fun_name == "print")
        curr_frame.instructions.push_back(VMInstr::WRITE());
    else if (fun_name == "input")
        curr_frame.instructions.push_back(VMInstr::READ());
    else if (fun_name == "get")
        curr_frame.instructions.push_back(VMInstr::GETC());
    else if (fun_name == "concat")
        curr_frame.instructions.push_back(VMInstr::CONCAT());
    else if (fun_name == "length")
        curr_frame.instructions.push_back(VMInstr::SLEN());
    else if (fun_name == "length_array")
        curr_frame.instructions.push_back(VMInstr::ALEN());
    else if (fun_name == "to_int")
        curr_frame.instructions.push_back(VMInstr::TOINT());
    else if (fun_name == "to_double")
        curr_frame.instructions.push_back(VMInstr::TODBL());
    else if (fun_name == "to_string")
        curr_frame.instructions.push_back(VMInstr::TOSTR());
    else
        curr_frame.instructions.push_back(VMInstr::CALL(fun_name));


}


void CodeGenerator::visit(Expr& e)
{
    // get lhs type
    e.first->accept(*this);

    // if op check operator is valid with types
    if(e.op.has_value()) {
        // get rhs type
        e.rest->accept(*this);

        // check operator is compatible
        string op_val = e.op->lexeme();

        if(op_val == "+")
            curr_frame.instructions.push_back(VMInstr::ADD());
        else if(op_val == "-")
            curr_frame.instructions.push_back(VMInstr::SUB());
        else if(op_val == "*")
            curr_frame.instructions.push_back(VMInstr::MUL());
        else if(op_val == "/")
            curr_frame.instructions.push_back(VMInstr::DIV());
        else if(op_val == "==")
            curr_frame.instructions.push_back(VMInstr::CMPEQ());
        else if(op_val == "!=")
            curr_frame.instructions.push_back(VMInstr::CMPNE());
        else if(op_val == "<")
            curr_frame.instructions.push_back(VMInstr::CMPLT());
        else if(op_val == ">")
            curr_frame.instructions.push_back(VMInstr::CMPGT());
        else if(op_val == "<=")
            curr_frame.instructions.push_back(VMInstr::CMPLE());
        else if(op_val == ">=")
            curr_frame.instructions.push_back(VMInstr::CMPGE());
        else if(op_val == "and")
            curr_frame.instructions.push_back(VMInstr::AND());
        else if(op_val == "or")
            curr_frame.instructions.push_back(VMInstr::OR());
    }

    if(e.negated)
        curr_frame.instructions.push_back(VMInstr::NOT());

}


void CodeGenerator::visit(SimpleTerm& t)
{
    t.rvalue->accept(*this);
}
 

void CodeGenerator::visit(ComplexTerm& t)
{
    t.expr.accept(*this);
}


void CodeGenerator::visit(SimpleRValue& v)
{
    if (v.value.type() == TokenType::INT_VAL) {
        int val = stoi(v.value.lexeme());
        curr_frame.instructions.push_back(VMInstr::PUSH(val));
    }
    else if (v.value.type() == TokenType::DOUBLE_VAL) {
        double val = stod(v.value.lexeme());
        curr_frame.instructions.push_back(VMInstr::PUSH(val));
    }
    else if (v.value.type() == TokenType::NULL_VAL) {
        curr_frame.instructions.push_back(VMInstr::PUSH(nullptr));
    }
    else if (v.value.type() == TokenType::BOOL_VAL) {
        if (v.value.lexeme() == "true")
            curr_frame.instructions.push_back(VMInstr::PUSH(true));
        else
            curr_frame.instructions.push_back(VMInstr::PUSH(false));
    }
    else if (v.value.type() == TokenType::STRING_VAL) {
        string s = v.value.lexeme();
        replace_all(s, "\\n", "\n");
        replace_all(s, "\\t", "\t");
        // could do more here
        curr_frame.instructions.push_back(VMInstr::PUSH(s));
    }
    else if (v.value.type() == TokenType::CHAR_VAL) {
        string s = v.value.lexeme();
        replace_all(s, "\\n", "\n");
        replace_all(s, "\\t", "\t");
        // could do more here
        curr_frame.instructions.push_back(VMInstr::PUSH(s));
    }

}


void CodeGenerator::visit(NewRValue& v)
{
    if (v.array_expr.has_value()) {
        v.array_expr->accept(*this);
        curr_frame.instructions.push_back(VMInstr::PUSH(nullptr));
        curr_frame.instructions.push_back(VMInstr::ALLOCA());
    }
    else  {
        curr_frame.instructions.push_back(VMInstr::ALLOCS());
        StructDef s = struct_defs[v.type.lexeme()];
        for(auto varDef : s.fields) {
            curr_frame.instructions.push_back(VMInstr::DUP());
            string name = varDef.var_name.lexeme();
            curr_frame.instructions.push_back(VMInstr::ADDF(name));
            curr_frame.instructions.push_back(VMInstr::DUP());
            curr_frame.instructions.push_back(VMInstr::PUSH(nullptr));
            curr_frame.instructions.push_back(VMInstr::SETF(name));
        }
    }

}


void CodeGenerator::visit(VarRValue& v)
{
    VarRef ref1 = v.path[0];
    int var_index = var_table.get(ref1.var_name.lexeme());
    // evaluate array
    curr_frame.instructions.push_back(VMInstr::LOAD(var_index));
    if (ref1.array_expr.has_value()) {
        ref1.array_expr->accept(*this);
        curr_frame.instructions.push_back(VMInstr::GETI());
    }

    // go through all paths
    for(int i = 1; i < v.path.size(); ++i) {
        VarRef varRef = v.path[i];
        curr_frame.instructions.push_back(VMInstr::GETF((varRef.var_name.lexeme())));

        if (varRef.array_expr.has_value()) {
            varRef.array_expr->accept(*this);
            curr_frame.instructions.push_back(VMInstr::GETI());
        }
    }

}
    

