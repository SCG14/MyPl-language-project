//----------------------------------------------------------------------
// FILE: vm_frame.h
// DATE: CPSC 326, Spring 2023
// AUTH: S. Bowers
// DESC: Representation of a VM frame and its corresponding frame
// information.
//----------------------------------------------------------------------

#ifndef VM_FRAME_H
#define VM_FRAME_H

#include <stack>
#include <string>
#include <vector>
#include "vm_instr.h"


// The following are plain-old-data classes


class VMFrameInfo
{
public:

  // the name of the function associated with the frame
  std::string function_name;

  // the number of parameters of the assocated function
  int arg_count; 

  // the program instructions
  std::vector<VMInstr> instructions;  

};


class VMFrame
{
public:

  // the type of the current frame
  VMFrameInfo info;
  
  // the program counter
  int pc = 0;

  // the internal memory of the function
  std::vector<VMValue> variables;

  // the operand stack
  std::stack<VMValue> operand_stack;

};

#endif
