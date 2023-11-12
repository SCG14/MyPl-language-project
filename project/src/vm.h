//----------------------------------------------------------------------
// FILE: vm.h
// DATE: CPSC 326, Spring 2020
// AUTH: S. Bowers
// DESC: The mypl virtual machine interface
//----------------------------------------------------------------------

#ifndef VM_H
#define VM_H

#include <memory>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>
#include "vm_instr.h"
#include "vm_frame.h"


class VM
{
public:

  // add a new frame type to the vm
  void add(const VMFrameInfo& frame);

  // run the virtual machine
  void run(bool DEBUG = false);

  // to print the instructions for each VM frame
  friend std::string to_string(const VM& vm);

  
private:

  // heap for struct objects mapping oid's to field values
  std::unordered_map<int, std::unordered_map<std::string, VMValue>> struct_heap;

  // heap for array objects
  std::unordered_map<int, std::vector<VMValue>> array_heap;

  // next available object id 
  int next_obj_id = 2023;

  // collection of frame "templates" identified by function name
  std::unordered_map<std::string, VMFrameInfo> frame_info;

  // VM function call stack
  std::stack<std::shared_ptr<VMFrame>> call_stack;

  // helper functions to report VM errors
  void error(std::string msg) const;
  void error(std::string msg, const VMFrame& f) const;

  // helper function to check for null values (throws mypl exception)
  void ensure_not_null(const VMFrame& f, const VMValue& x) const;

  // operation support helper functions
  VMValue add(const VMValue& x, const VMValue& y) const;
  VMValue sub(const VMValue& x, const VMValue& y) const;  
  VMValue mul(const VMValue& x, const VMValue& y) const;  
  VMValue div(const VMValue& x, const VMValue& y) const;    
  VMValue lt(const VMValue& x, const VMValue& y) const;  
  VMValue le(const VMValue& x, const VMValue& y) const;  
  VMValue gt(const VMValue& x, const VMValue& y) const;  
  VMValue ge(const VMValue& x, const VMValue& y) const;  
  VMValue eq(const VMValue& x, const VMValue& y) const;  

};

#endif
