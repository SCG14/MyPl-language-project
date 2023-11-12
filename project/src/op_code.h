//----------------------------------------------------------------------
// FILE: op_code.h
// DATE: CPSC 326, Spring 2023
// AUTH: S. Bowers
// DESC: Virtual machine instruction types
//----------------------------------------------------------------------

#ifndef OP_CODE_H
#define OP_CODE_H

enum class OpCode {

  // consts/vars
  PUSH,         // [operand] push v onto stack
  POP,          // pop value off of stack
  LOAD,         // [operand] push value at memory address v onto stack
  STORE,        // [operand] pop x, store x at memory address v

  // arithmetic ops
  ADD,          // pop x and y off stack, push (y + x) onto stack
  SUB,          // pop x and y off stack, push (y - x) onto stack
  MUL,          // pop x and y off stack, push (y * x) onto stack
  DIV,          // pop x and y off stack, push (y / x) onto stack

  // logical operators
  AND,          // pop bools x and y, push (y and x)
  OR,           // pop bools x and y, push (y or x)
  NOT,          // pop bool x, push (not x)

  // comparators
  CMPLT,        // pop x and y off stack, push (y < x)
  CMPLE,        // pop x and y off stack, push (y <= x)
  CMPGT,        // pop x and y off stack, push (y > x)
  CMPGE,        // pop x and y off stack, push (y >= x)
  CMPEQ,        // pop x and y off stack, push (y == x)  
  CMPNE,        // pop x and y off stack, push (y != x)

  // jump
  JMP,          // [operand] jump to given instruction v
  JMPF,         // [operand] pop x, if x is false jump to instruction v

  // functions
  CALL,         // [operand] call function v (pop and push args)
  RET,          // return from current function

  // built-ins
  WRITE,        // pop x, write to stdout
  READ,         // read stdin, push on stack
  SLEN,         // pop string x, push x.size()
  ALEN,         // pop array (vector) x, push x.size()
  GETC,         // pop string x, pop int y, push x[y]
  TOINT,        // pop x, push x as an integer
  TODBL,        // pop x, push x as a double
  TOSTR,        // pop x, push x as string
  CONCAT,       // pop x, pop y, push y + x (string concat)
    
  // heap
  ALLOCS,       // allocate struct obj, push oid x
  ALLOCA,       // pop x, pop y, allocate array obj with y x values, push oid
  ADDF,         // [operand] pop x, add field named v to obj(x)
  SETF,         // [operand] pop x and y, set obj(y).v = x
  GETF,         // [operand] pop x, push value of obj(x).v 
  SETI,         // pop x, y, and z, set array obj(z)[y] = x
  GETI,         // pop x and y, push array obj(y)[x] value
    
  // special
  DUP,          // pop x, push x, push x
  NOP           // has no effect (for jumping over code segments)

};

#endif
