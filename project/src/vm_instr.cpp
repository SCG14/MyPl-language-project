//----------------------------------------------------------------------
// FILE: vm_instr.cpp
// DATE: CPSC 326, Spring 2023
// AUTH: S. Bowers
// DESC: Virtual machine instructions
//----------------------------------------------------------------------


#include <unordered_map>
#include "vm_instr.h"

using namespace std;


VMInstr::VMInstr(OpCode opcode)
  : instr_opcode(opcode)
{}


VMInstr::VMInstr(OpCode opcode, const VMValue& operand)
  : instr_opcode(opcode), instr_operand(operand)
{}


void VMInstr::set_comment(const std::string& comment)
{
  instr_comment = comment;
}


std::string VMInstr::comment() const
{
  return instr_comment;
}


OpCode VMInstr::opcode() const
{
  return instr_opcode;
}


std::optional<VMValue> VMInstr::operand() const
{
  return instr_operand;
}


void VMInstr::set_operand(VMValue value)
{
  instr_operand = value;
}


VMInstr VMInstr::PUSH(const VMValue& value)
{
  return VMInstr(OpCode::PUSH, value);
}


VMInstr VMInstr::POP()
{
  return VMInstr(OpCode::POP);
}


VMInstr VMInstr::LOAD(int mem_addr)
{
  return VMInstr(OpCode::LOAD, mem_addr);
}


VMInstr VMInstr::STORE(int mem_addr)
{
  return VMInstr(OpCode::STORE, mem_addr);
}


VMInstr VMInstr::ADD()
{
  return VMInstr(OpCode::ADD);
}


VMInstr VMInstr::SUB()
{
  return VMInstr(OpCode::SUB);
}


VMInstr VMInstr::MUL()
{
  return VMInstr(OpCode::MUL);
}


VMInstr VMInstr::DIV()
{
  return VMInstr(OpCode::DIV);
}


VMInstr VMInstr::AND()
{
  return VMInstr(OpCode::AND);
}


VMInstr VMInstr::OR()
{
  return VMInstr(OpCode::OR);
}


VMInstr VMInstr::NOT()
{
  return VMInstr(OpCode::NOT);
}


VMInstr VMInstr::CMPLT()
{
  return VMInstr(OpCode::CMPLT);
}


VMInstr VMInstr::CMPLE()
{
  return VMInstr(OpCode::CMPLE);
}


VMInstr VMInstr::CMPGT()
{
  return VMInstr(OpCode::CMPGT);
}


VMInstr VMInstr::CMPGE()
{
  return VMInstr(OpCode::CMPGE);
}


VMInstr VMInstr::CMPEQ()
{
  return VMInstr(OpCode::CMPEQ);
}


VMInstr VMInstr::CMPNE()
{
  return VMInstr(OpCode::CMPNE);
}


VMInstr VMInstr::JMP(int instruction_index)
{
  return VMInstr(OpCode::JMP, instruction_index);
}


VMInstr VMInstr::JMPF(int instruction_index)
{
  return VMInstr(OpCode::JMPF, instruction_index);
}


VMInstr VMInstr::CALL(const std::string& function)
{
  return VMInstr(OpCode::CALL, function);
}


VMInstr VMInstr::RET()
{
  return VMInstr(OpCode::RET);  
}


VMInstr VMInstr::WRITE()
{
  return VMInstr(OpCode::WRITE);
}


VMInstr VMInstr::READ()
{
  return VMInstr(OpCode::READ);
}


VMInstr VMInstr::SLEN()
{
  return VMInstr(OpCode::SLEN);
}


VMInstr VMInstr::ALEN()
{
  return VMInstr(OpCode::ALEN);
}


VMInstr VMInstr::GETC()
{
  return VMInstr(OpCode::GETC);
}


VMInstr VMInstr::TOINT()
{
  return VMInstr(OpCode::TOINT);  
}


VMInstr VMInstr::TODBL()
{
  return VMInstr(OpCode::TODBL);
}  


VMInstr VMInstr::TOSTR()
{
  return VMInstr(OpCode::TOSTR);  
}


VMInstr VMInstr::CONCAT()
{
  return VMInstr(OpCode::CONCAT);  
}


VMInstr VMInstr::ALLOCS()
{
  return VMInstr(OpCode::ALLOCS);  
}


VMInstr VMInstr::ALLOCA()
{
  return VMInstr(OpCode::ALLOCA);    
}


VMInstr VMInstr::ADDF(const string& field)
{
  return VMInstr(OpCode::ADDF, field);
}


VMInstr VMInstr::SETF(const string& field)
{
  return VMInstr(OpCode::SETF, field);      
}


VMInstr VMInstr::GETF(const string& field)
{
  return VMInstr(OpCode::GETF, field);
}


VMInstr VMInstr::SETI()
{
  return VMInstr(OpCode::SETI);      
}


VMInstr VMInstr::GETI()
{
  return VMInstr(OpCode::GETI);      
}  


VMInstr VMInstr::DUP()
{
  return VMInstr(OpCode::DUP);      
}


VMInstr VMInstr::NOP()
{
  return VMInstr(OpCode::NOP);      
}


string to_string(const VMValue& val) {
  if (holds_alternative<int>(val))
    return to_string(get<int>(val));
  else if (holds_alternative<double>(val))
    return to_string(get<double>(val));
  else if (holds_alternative<bool>(val) and get<bool>(val))
    return "true";
  else if (holds_alternative<bool>(val) and !get<bool>(val))
    return "false";
  else if (holds_alternative<string>(val))
    return get<string>(val);
  else
    return "null";
}


std::string to_string(const VMInstr& instr)
{
  std::unordered_map<OpCode, string> os = {
    {OpCode::PUSH, "PUSH"}, {OpCode::POP, "POP"},
    {OpCode::LOAD, "LOAD"}, {OpCode::STORE, "STORE"},
    {OpCode::ADD, "ADD"}, {OpCode::SUB, "SUB"},
    {OpCode::MUL, "MUL"}, {OpCode::DIV, "DIV"},
    {OpCode::AND, "AND"}, {OpCode::OR, "OR"},
    {OpCode::NOT, "NOT"}, {OpCode::CMPLT, "CMPLT"},
    {OpCode::CMPLE, "CMPLE"}, {OpCode::CMPGT, "CMPGT"},
    {OpCode::CMPGE, "CMPGE"}, {OpCode::CMPEQ, "CMPEQ"}, 
    {OpCode::CMPNE, "CMPNE"}, {OpCode::JMP, "JMP"},
    {OpCode::JMPF, "JMPF"}, {OpCode::CALL, "CALL"},
    {OpCode::RET, "RET"}, {OpCode::WRITE, "WRITE"},
    {OpCode::READ, "READ"}, {OpCode::SLEN, "SLEN"},
    {OpCode::ALEN, "ALEN"}, {OpCode::GETC, "GETC"},
    {OpCode::TOINT, "TOINT"}, {OpCode::TODBL, "TODBL"},
    {OpCode::TOSTR, "TOSTR"}, {OpCode::CONCAT, "CONCAT"},
    {OpCode::ALLOCS, "ALLOCS"}, {OpCode::ALLOCA, "ALLOCA"},
    {OpCode::ADDF, "ADDF"}, {OpCode::GETF, "GETF"},
    {OpCode::SETF, "SETF"}, {OpCode::GETI, "GETI"},
    {OpCode::SETI, "SETI"}, {OpCode::DUP, "DUP"},
    {OpCode::NOP, "NOP"}
  };
  string vstr = "";
  if (instr.operand().has_value()) {
    vstr = to_string(instr.operand().value());
  }
  string s = os[instr.opcode()] + "(" + vstr + ")";
  if (instr.instr_comment != "")
    s += "  // " + instr.instr_comment;
  return s;
}

  

