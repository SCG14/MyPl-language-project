//----------------------------------------------------------------------
// FILE: vm.cpp
// DATE: CPSC 326, Spring 2023
// AUTH: 
// DESC: 
//----------------------------------------------------------------------

#include <iostream>
#include "vm.h"
#include "mypl_exception.h"


using namespace std;


void VM::error(string msg) const
{
  throw MyPLException::VMError(msg);
}


void VM::error(string msg, const VMFrame& frame) const
{
  int pc = frame.pc - 1;
  VMInstr instr = frame.info.instructions[pc];
  string name = frame.info.function_name;
  msg += " (in " + name + " at " + to_string(pc) + ": " +
    to_string(instr) + ")";
  throw MyPLException::VMError(msg);
}


string to_string(const VM& vm)
{
  string s = "";
  for (const auto& entry : vm.frame_info) {
    const string& name = entry.first;
    s += "\nFrame '" + name + "'\n";
    const VMFrameInfo& frame = entry.second;
    for (int i = 0; i < frame.instructions.size(); ++i) {
      VMInstr instr = frame.instructions[i];
      s += "  " + to_string(i) + ": " + to_string(instr) + "\n"; 
    }
  }
  return s;
}


void VM::add(const VMFrameInfo& frame)
{
  frame_info[frame.function_name] = frame;
}


void VM::run(bool DEBUG)
{
  // grab the "main" frame if it exists
  if (!frame_info.contains("main"))
    error("No 'main' function");
  shared_ptr<VMFrame> frame = make_shared<VMFrame>();
  frame->info = frame_info["main"];
  call_stack.push(frame);

  // run loop (keep going until we run out of instructions)
  while (!call_stack.empty() and frame->pc < frame->info.instructions.size()) {

    // get the next instruction
    VMInstr& instr = frame->info.instructions[frame->pc];

    // increment the program counter
    ++frame->pc;

    // for debugging
    if (DEBUG) {
      cerr << endl << endl;
      cerr << "\t FRAME.........: " << frame->info.function_name << endl;
      cerr << "\t PC............: " << (frame->pc - 1) << endl;
      cerr << "\t INSTR.........: " << to_string(instr) << endl;
      cerr << "\t NEXT OPERAND..: ";
      if (!frame->operand_stack.empty())
        cerr << to_string(frame->operand_stack.top()) << endl;
      else
        cerr << "empty" << endl;
      cerr << "\t NEXT FUNCTION.: ";
      if (!call_stack.empty())
        cerr << call_stack.top()->info.function_name << endl;
      else
        cerr << "empty" << endl;
    }

    //----------------------------------------------------------------------
    // Literals and Variables
    //----------------------------------------------------------------------

    if (instr.opcode() == OpCode::PUSH) {
      frame->operand_stack.push(instr.operand().value());
    }

    else if (instr.opcode() == OpCode::POP) {
      frame->operand_stack.pop();
    }

    else if (instr.opcode() == OpCode::LOAD) {
       int x = get<int>(instr.operand().value());
       VMValue y = frame->variables.at(x);
        frame->operand_stack.push(y);
    }

    else if (instr.opcode() == OpCode::STORE) {
        VMValue x = frame->operand_stack.top();
        frame->operand_stack.pop();
        int index = get<int>(instr.operand().value());
        if(index >= frame->variables.size())
            frame->variables.push_back(x);
        else
            frame->variables[index] = x;
    }
    
    //----------------------------------------------------------------------
    // Operations
    //----------------------------------------------------------------------

    else if (instr.opcode() == OpCode::ADD) {
      VMValue x = frame->operand_stack.top();
      ensure_not_null(*frame, x);
      frame->operand_stack.pop();
      VMValue y = frame->operand_stack.top();
      ensure_not_null(*frame, y);
      frame->operand_stack.pop();
      frame->operand_stack.push(add(y, x));
    }

    else if (instr.opcode() == OpCode::SUB) {
        VMValue x = frame->operand_stack.top();
        ensure_not_null(*frame, x);
        frame->operand_stack.pop();
        VMValue y = frame->operand_stack.top();
        ensure_not_null(*frame, y);
        frame->operand_stack.pop();
        frame->operand_stack.push(sub(y, x));
    }

    else if (instr.opcode() == OpCode::MUL) {
        VMValue x = frame->operand_stack.top();
        ensure_not_null(*frame, x);
        frame->operand_stack.pop();
        VMValue y = frame->operand_stack.top();
        ensure_not_null(*frame, y);
        frame->operand_stack.pop();
        frame->operand_stack.push(mul(y, x));
    }

    else if (instr.opcode() == OpCode::DIV) {
        VMValue x = frame->operand_stack.top();
        ensure_not_null(*frame, x);
        frame->operand_stack.pop();
        VMValue y = frame->operand_stack.top();
        ensure_not_null(*frame, y);
        frame->operand_stack.pop();
        frame->operand_stack.push(div(y, x));
    }

    else if (instr.opcode() == OpCode::AND) {
        VMValue x = frame->operand_stack.top();
        ensure_not_null(*frame, x);
        frame->operand_stack.pop();
        bool bx = get<bool>(x);
        VMValue y = frame->operand_stack.top();
        ensure_not_null(*frame, y);
        frame->operand_stack.pop();
        bool by = get<bool>(y);
        VMValue result = bx && by;
        frame->operand_stack.push(result);
    }

    else if (instr.opcode() == OpCode::OR) {
        VMValue x = frame->operand_stack.top();
        ensure_not_null(*frame, x);
        frame->operand_stack.pop();
        bool bx = get<bool>(x);
        VMValue y = frame->operand_stack.top();
        ensure_not_null(*frame, y);
        frame->operand_stack.pop();
        bool by = get<bool>(y);
        VMValue result = bx || by;
        frame->operand_stack.push(result);
    }

    else if (instr.opcode() == OpCode::NOT) {
        VMValue x = frame->operand_stack.top();
        ensure_not_null(*frame, x);
        frame->operand_stack.pop();
        bool bx = get<bool>(x);
        VMValue result = !bx;
        frame->operand_stack.push(result);
    }

    else if (instr.opcode() == OpCode::CMPLT) {
        VMValue x = frame->operand_stack.top();
        ensure_not_null(*frame, x);
        frame->operand_stack.pop();
        VMValue y = frame->operand_stack.top();
        ensure_not_null(*frame, y);
        frame->operand_stack.pop();
        frame->operand_stack.push(lt(y, x));
    }

    else if (instr.opcode() == OpCode::CMPLE) {
        VMValue x = frame->operand_stack.top();
        ensure_not_null(*frame, x);
        frame->operand_stack.pop();
        VMValue y = frame->operand_stack.top();
        ensure_not_null(*frame, y);
        frame->operand_stack.pop();
        frame->operand_stack.push(le(y, x));
    }

    else if (instr.opcode() == OpCode::CMPGT) {
        VMValue x = frame->operand_stack.top();
        ensure_not_null(*frame, x);
        frame->operand_stack.pop();
        VMValue y = frame->operand_stack.top();
        ensure_not_null(*frame, y);
        frame->operand_stack.pop();
        frame->operand_stack.push(gt(y, x));
    }

    else if (instr.opcode() == OpCode::CMPGE) {
        VMValue x = frame->operand_stack.top();
        ensure_not_null(*frame, x);
        frame->operand_stack.pop();
        VMValue y = frame->operand_stack.top();
        ensure_not_null(*frame, y);
        frame->operand_stack.pop();
        frame->operand_stack.push(ge(y, x));
    }

    else if (instr.opcode() == OpCode::CMPEQ) {
        VMValue x = frame->operand_stack.top();
        frame->operand_stack.pop();
        VMValue y = frame->operand_stack.top();
        frame->operand_stack.pop();
        bool xy = get<bool>(eq(y, x));
        VMValue result = xy;
        frame->operand_stack.push(result);
    }

    else if (instr.opcode() == OpCode::CMPNE) {
        VMValue x = frame->operand_stack.top();
        frame->operand_stack.pop();
        VMValue y = frame->operand_stack.top();
        frame->operand_stack.pop();
        bool xy = get<bool>(eq(y, x));
        VMValue result = !xy;
        frame->operand_stack.push(result);
    }

        //----------------------------------------------------------------------
    // Branching
    //----------------------------------------------------------------------

    else if (instr.opcode() == OpCode::JMP) {
        int x = get<int>(instr.operand().value());
        //instr = frame->info.instructions[x];
        frame->pc = x;
    }

    else if (instr.opcode() == OpCode::JMPF) {
        int line = get<int>(instr.operand().value());
        VMValue x = frame->operand_stack.top();
        frame->operand_stack.pop();
        bool bx = get<bool>(x);
        if(!bx) {
            //instr = frame->info.instructions[line];
            frame->pc = line;
        }
    }
    
    //----------------------------------------------------------------------
    // Functions
    //----------------------------------------------------------------------

    else if (instr.opcode() == OpCode::CALL) {
        string name = get<string>(instr.operand().value());
        shared_ptr<VMFrame> new_frame = make_shared<VMFrame>();
        new_frame->info = frame_info[name];
        int count = new_frame->info.arg_count;
        call_stack.push(new_frame);
        for(int i = 0; i < count; i++) {
            VMValue x = frame->operand_stack.top();
            new_frame->operand_stack.push(x);
            frame->operand_stack.pop();
        }
        frame = new_frame;

    }
//
    else if (instr.opcode() == OpCode::RET) {
        VMValue x = frame->operand_stack.top();
        call_stack.pop();
        if(!call_stack.empty()) {
            frame = call_stack.top();
            frame->operand_stack.push(x);
        }
    }
    
    //----------------------------------------------------------------------
    // Built in functions
    //----------------------------------------------------------------------

    else if (instr.opcode() == OpCode::WRITE) {
      VMValue x = frame->operand_stack.top();
      frame->operand_stack.pop();
      cout << to_string(x);
    }

    else if (instr.opcode() == OpCode::READ) {
      string val = "";
      getline(cin, val);
      frame->operand_stack.push(val);
    }

    else if (instr.opcode() == OpCode::SLEN) {
        VMValue x = frame->operand_stack.top();
        frame->operand_stack.pop();
        ensure_not_null(*frame, x);
        string xy = get<string>(x);
        int z = xy.length();
        VMValue result = z;
        frame->operand_stack.push(result);
    }
    //----------------------------------------------------------------------
    // heap
    //----------------------------------------------------------------------

    else if (instr.opcode() == OpCode::ALLOCS) {
        struct_heap[next_obj_id] = {};
        frame->operand_stack.push(next_obj_id);
        ++next_obj_id;
    }

    else if (instr.opcode() == OpCode::ALLOCA) {
        VMValue val = frame->operand_stack.top();
        frame->operand_stack.pop();
        int size = get<int>(frame->operand_stack.top());
        frame->operand_stack.pop();
        array_heap[next_obj_id] = vector<VMValue>(size, val);
        frame->operand_stack.push(next_obj_id);
        ++next_obj_id;
    }

    else if (instr.opcode() == OpCode::ADDF) {
        // [operand] pop x, add field named v to obj(x)
        string name = get<string>(instr.operand().value());
        VMValue vm = frame->operand_stack.top();
        ensure_not_null(*frame, vm);
        int x = get<int>(vm);
        frame->operand_stack.pop();
        struct_heap[x][name] = nullptr;
    }

    else if (instr.opcode() == OpCode::SETF) {
        // [operand] pop x and y, set obj(y).v = x
        string name = get<string>(instr.operand().value());
        VMValue x = frame->operand_stack.top();
        frame->operand_stack.pop();
        VMValue vm = frame->operand_stack.top();
        ensure_not_null(*frame, vm);
        int y = get<int>(vm);
        frame->operand_stack.pop();
        struct_heap[y][name] = x;
    }

    else if (instr.opcode() == OpCode::GETF) {
        // [operand] pop x, push value of obj(x).v
        string name = get<string>(instr.operand().value());
        VMValue vm = frame->operand_stack.top();
        ensure_not_null(*frame, vm);
        int x = get<int>(vm);
        frame->operand_stack.pop();
        VMValue v = struct_heap[x][name];
        frame->operand_stack.push(v);
    }

    else if (instr.opcode() == OpCode::SETI) {
        // pop x, y, and z, set array obj(z)[y] = x
        VMValue vmx = frame->operand_stack.top();
        frame->operand_stack.pop();
        VMValue vmy = frame->operand_stack.top();
        frame->operand_stack.pop();
        VMValue vmz = frame->operand_stack.top();
        frame->operand_stack.pop();

        ensure_not_null(*frame, vmx);
        ensure_not_null(*frame, vmy);
        ensure_not_null(*frame, vmz);
        int y = get<int>(vmy);
        int z = get<int>(vmz);

        if(y >= array_heap[z].size())
            error("out-of-bounds array index", *frame);
        array_heap[z][y] = vmx;
    }

    else if (instr.opcode() == OpCode::GETI) {
        // pop x and y, push array obj(y)[x] value
        VMValue vmx = frame->operand_stack.top();
        frame->operand_stack.pop();
        VMValue vmy = frame->operand_stack.top();
        frame->operand_stack.pop();

        ensure_not_null(*frame, vmx);
        ensure_not_null(*frame, vmy);

        int x = get<int>(vmx);
        int y = get<int>(vmy);

        if(x >= array_heap[y].size())
            error("out-of-bounds array index", *frame);
        VMValue value = array_heap[y][x];
        frame->operand_stack.push(value);
    }


    else if (instr.opcode() == OpCode::ALEN) {
        // pop array (vector) x, push x.size()
        VMValue vmx = frame->operand_stack.top();
        frame->operand_stack.pop();
        ensure_not_null(*frame, vmx);

        int x = get<int>(vmx);
        std::vector<VMValue> vector = array_heap.at(x);
        int length = vector.size();
        frame->operand_stack.push(length);
    }

    else if (instr.opcode() == OpCode::GETC) {
        // pop string x, pop int y, push x[y]
        VMValue vmx = frame->operand_stack.top();
        frame->operand_stack.pop();
        VMValue vmy = frame->operand_stack.top();
        frame->operand_stack.pop();

        ensure_not_null(*frame, vmx);
        ensure_not_null(*frame, vmy);

        string x = get<string>(vmx);
        int y = get<int>(vmy);
        if(y >= x.size())
            error("out-of-bounds string index", *frame);
        string ch = "";
        ch += x.at(y);
        VMValue value = ch;
        frame->operand_stack.push(value);
    }

    else if (instr.opcode() == OpCode::TOSTR) {
        VMValue x = frame->operand_stack.top();
        frame->operand_stack.pop();
        ensure_not_null(*frame, x);
        string xy = to_string(x);
        VMValue result = xy;
        frame->operand_stack.push(result);
    }

    else if (instr.opcode() == OpCode::TOINT) {
        VMValue x = frame->operand_stack.top();
        frame->operand_stack.pop();
        ensure_not_null(*frame, x);
        if (holds_alternative<double>(x)) {
            int xy = (int) get<double>(x);
            frame->operand_stack.push(xy);
        } else if (holds_alternative<bool>(x)) {
            int xy = (int) get<bool>(x);
            frame->operand_stack.push(xy);
        }
        else {
            try {
                int str = stoi(get<string>(x));
                frame->operand_stack.push(str);
            }
            catch (...) {
                error("cannot convert string to int", (VMFrame &) *frame);
            }
        }
    }

    else if (instr.opcode() == OpCode::TODBL) {
        VMValue x = frame->operand_stack.top();
        frame->operand_stack.pop();
        ensure_not_null(*frame, x);
        if (holds_alternative<int>(x)) {
            double xy = (double) get<int>(x);
            frame->operand_stack.push(xy);
        } else if (holds_alternative<bool>(x)) {
            double xy = (double) get<bool>(x);
            frame->operand_stack.push(xy);
        } else
            try {
                double xy = stod(get<string>(x));
                frame->operand_stack.push(xy);
            }
            catch (...){
                error("cannot convert string to double", (VMFrame &) *frame);
            }
    }

    else if (instr.opcode() == OpCode::CONCAT) {
        VMValue x = frame->operand_stack.top();
        ensure_not_null(*frame, x);
        frame->operand_stack.pop();
        VMValue y = frame->operand_stack.top();
        ensure_not_null(*frame, y);
        frame->operand_stack.pop();
        string sx = get<string>(x);
        string sy = get<string>(y);
        string concat = sy + sx;
        frame->operand_stack.push(concat);
    }
    
    //----------------------------------------------------------------------
    // special
    //----------------------------------------------------------------------

    
    else if (instr.opcode() == OpCode::DUP) {
      VMValue x = frame->operand_stack.top();
      frame->operand_stack.pop();
      frame->operand_stack.push(x);
      frame->operand_stack.push(x);      
    }

    else if (instr.opcode() == OpCode::NOP) {
      // do nothing
    }
    
    else {
      error("unsupported operation " + to_string(instr));
    }
  }
}


void VM::ensure_not_null(const VMFrame& f, const VMValue& x) const
{
  if (holds_alternative<nullptr_t>(x))
    error("null reference", f);
}


VMValue VM::add(const VMValue& x, const VMValue& y) const
{
  if (holds_alternative<int>(x)) 
    return get<int>(x) + get<int>(y);
  else
    return get<double>(x) + get<double>(y);
}

// TODO: Finish the rest of the following arithmetic operators

VMValue VM::sub(const VMValue& x, const VMValue& y) const
{
    if (holds_alternative<int>(x))
        return get<int>(x) - get<int>(y);
    else
        return get<double>(x) - get<double>(y);
}

VMValue VM::mul(const VMValue& x, const VMValue& y) const
{
    if (holds_alternative<int>(x))
        return get<int>(x) * get<int>(y);
    else
        return get<double>(x) * get<double>(y);
}

VMValue VM::div(const VMValue& x, const VMValue& y) const
{
    if (holds_alternative<int>(x))
        return get<int>(x) / get<int>(y);
    else
        return get<double>(x) / get<double>(y);
}


VMValue VM::eq(const VMValue& x, const VMValue& y) const
{
    if (holds_alternative<nullptr_t>(x) and not holds_alternative<nullptr_t>(y))
        return false;
    else if (not holds_alternative<nullptr_t>(x) and holds_alternative<nullptr_t>(y))
        return false;
    else if (holds_alternative<nullptr_t>(x) and holds_alternative<nullptr_t>(y))
        return true;
    else if (holds_alternative<int>(x))
        return get<int>(x) == get<int>(y);
    else if (holds_alternative<double>(x))
        return get<double>(x) == get<double>(y);
    else if (holds_alternative<string>(x))
        return get<string>(x) == get<string>(y);
    else
        return get<bool>(x) == get<bool>(y);
}

// TODO: Finish the rest of the comparison operators

VMValue VM::lt(const VMValue& x, const VMValue& y) const
{
    if (holds_alternative<nullptr_t>(x) and not holds_alternative<nullptr_t>(y))
        return false;
    else if (not holds_alternative<nullptr_t>(x) and holds_alternative<nullptr_t>(y))
        return false;
    else if (holds_alternative<nullptr_t>(x) and holds_alternative<nullptr_t>(y))
        return true;
    else if (holds_alternative<int>(x))
        return get<int>(x) < get<int>(y);
    else if (holds_alternative<double>(x))
        return get<double>(x) < get<double>(y);
    else if (holds_alternative<string>(x))
        return get<string>(x) < get<string>(y);
    else
        return get<bool>(x) < get<bool>(y);
}

VMValue VM::le(const VMValue& x, const VMValue& y) const
{
    if (holds_alternative<nullptr_t>(x) and not holds_alternative<nullptr_t>(y))
        return false;
    else if (not holds_alternative<nullptr_t>(x) and holds_alternative<nullptr_t>(y))
        return false;
    else if (holds_alternative<nullptr_t>(x) and holds_alternative<nullptr_t>(y))
        return true;
    else if (holds_alternative<int>(x))
        return get<int>(x) <= get<int>(y);
    else if (holds_alternative<double>(x))
        return get<double>(x) <= get<double>(y);
    else if (holds_alternative<string>(x))
        return get<string>(x) <= get<string>(y);
    else
        return get<bool>(x) <= get<bool>(y);
}

VMValue VM::gt(const VMValue& x, const VMValue& y) const
{
    if (holds_alternative<nullptr_t>(x) and not holds_alternative<nullptr_t>(y))
        return false;
    else if (not holds_alternative<nullptr_t>(x) and holds_alternative<nullptr_t>(y))
        return false;
    else if (holds_alternative<nullptr_t>(x) and holds_alternative<nullptr_t>(y))
        return true;
    else if (holds_alternative<int>(x))
        return get<int>(x) > get<int>(y);
    else if (holds_alternative<double>(x))
        return get<double>(x) > get<double>(y);
    else if (holds_alternative<string>(x))
        return get<string>(x) > get<string>(y);
    else
        return get<bool>(x) > get<bool>(y);
}

VMValue VM::ge(const VMValue& x, const VMValue& y) const
{
    if (holds_alternative<nullptr_t>(x) and not holds_alternative<nullptr_t>(y))
        return false;
    else if (not holds_alternative<nullptr_t>(x) and holds_alternative<nullptr_t>(y))
        return false;
    else if (holds_alternative<nullptr_t>(x) and holds_alternative<nullptr_t>(y))
        return true;
    else if (holds_alternative<int>(x))
        return get<int>(x) >= get<int>(y);
    else if (holds_alternative<double>(x))
        return get<double>(x) >= get<double>(y);
    else if (holds_alternative<string>(x))
        return get<string>(x) >= get<string>(y);
    else
        return get<bool>(x) >= get<bool>(y);
}

