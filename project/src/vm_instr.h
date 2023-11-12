//----------------------------------------------------------------------
// FILE: vm_instr.h
// DATE: CPSC 326, Spring 2023
// AUTH: S. Bowers
// DESC: MyPL VM instructions
//----------------------------------------------------------------------


#ifndef VM_INSTR_H
#define VM_INSTR_H

#include <variant>
#include <optional>
#include <string>
#include "op_code.h"


// vm values are one of int, double, bool, string, or nullptr_t
typedef std::variant<int, double, bool, std::string, std::nullptr_t> VMValue;

// function to get a string representation of a vm_value
std::string to_string(const VMValue& val);


class VMInstr
{
public:

  // static creation functions for the various types of instructions
  static VMInstr PUSH(const VMValue& value);
  static VMInstr POP();
  static VMInstr LOAD(int mem_addr);
  static VMInstr STORE(int mem_addr);
  static VMInstr ADD();
  static VMInstr SUB();
  static VMInstr MUL();
  static VMInstr DIV();
  static VMInstr AND();
  static VMInstr OR();
  static VMInstr NOT();
  static VMInstr CMPLT();
  static VMInstr CMPLE();
  static VMInstr CMPGT();
  static VMInstr CMPGE();
  static VMInstr CMPEQ();
  static VMInstr CMPNE();
  static VMInstr JMP(int instruction_index);
  static VMInstr JMPF(int instruction_index);
  static VMInstr CALL(const std::string& function);
  static VMInstr RET();
  static VMInstr WRITE();
  static VMInstr READ();
  static VMInstr SLEN();
  static VMInstr ALEN();
  static VMInstr GETC();
  static VMInstr TOINT();
  static VMInstr TODBL();  
  static VMInstr TOSTR();
  static VMInstr CONCAT();
  static VMInstr ALLOCS();
  static VMInstr ALLOCA();
  static VMInstr ADDF(const std::string& field);
  static VMInstr SETF(const std::string& field);
  static VMInstr GETF(const std::string& field);
  static VMInstr SETI();
  static VMInstr GETI();  
  static VMInstr DUP();
  static VMInstr NOP();

  // set the instruction's comment (optional)
  void set_comment(const std::string& comment);

  // returns the comment or empty string if no comment has been set
  std::string comment() const;

  // returns the instruction's opcode
  OpCode opcode() const;

  // returns the operand for those instructions with operands
  std::optional<VMValue> operand() const;

  // set the operand value
  void set_operand(VMValue value);
  
  // pretty print the instruction
  friend std::string to_string(const VMInstr& instr);
  
private:

  // each instruction has an opcode
  OpCode instr_opcode;

  // some instructions have operands
  std::optional<VMValue> instr_operand;

  // comments can be optionally added
  std::string instr_comment;

  // no operand constructor (helper) for use by static construction methods
  VMInstr(OpCode opcode);

  // operand constructor (helper) for use by static construction methods
  VMInstr(OpCode opcode, const VMValue& value);

};


#endif
