//----------------------------------------------------------------------
// FILE: semantic_checker.cpp
// DATE: CPSC 326, Spring 2023
// AUTH: 
// DESC: 
//----------------------------------------------------------------------

#include <unordered_set>
#include "mypl_exception.h"
#include "semantic_checker.h"
#include <iostream>

using namespace std;

// hash table of names of the base data types and built-in functions
const unordered_set<string> BASE_TYPES {"int", "double", "char", "string", "bool"};
const unordered_set<string> BUILT_INS {"print", "input", "to_string",  "to_int",
  "to_double", "length", "get", "concat"};


// helper functions

optional<VarDef> SemanticChecker::get_field(const StructDef& struct_def,
                                            const string& field_name)
{
  for (const VarDef& var_def : struct_def.fields)
    if (var_def.var_name.lexeme() == field_name)
      return var_def;
  return nullopt;
}


void SemanticChecker::error(const string& msg, const Token& token)
{
  string s = msg;
  s += " near line " + to_string(token.line()) + ", ";
  s += "column " + to_string(token.column());
  throw MyPLException::StaticError(s);
}


void SemanticChecker::error(const string& msg)
{
  throw MyPLException::StaticError(msg);
}


// visitor functions


void SemanticChecker::visit(Program& p)
{
  // record each struct def
  for (StructDef& d : p.struct_defs) {
    string name = d.struct_name.lexeme();
    if (struct_defs.contains(name))
      error("multiple definitions of '" + name + "'", d.struct_name);
    struct_defs[name] = d;
  }
  // record each function def (need a main function)
  bool found_main = false;
  for (FunDef& f : p.fun_defs) {
    string name = f.fun_name.lexeme();
    if (BUILT_INS.contains(name))
      error("redefining built-in function '" + name + "'", f.fun_name);
    if (fun_defs.contains(name))
      error("multiple definitions of '" + name + "'", f.fun_name);
    if (name == "main") {
      if (f.return_type.type_name != "void")
        error("main function must have void type", f.fun_name);
      if (f.params.size() != 0)
        error("main function cannot have parameters", f.params[0].var_name);
      found_main = true;
    }
    fun_defs[name] = f;
  }
  if (!found_main)
    error("program missing main function");
  // check each struct
  for (StructDef& d : p.struct_defs)
    d.accept(*this);
  // check each function
  for (FunDef& d : p.fun_defs)
    d.accept(*this);
}


void SemanticChecker::visit(SimpleRValue& v)
{
  if (v.value.type() == TokenType::INT_VAL)
    curr_type = DataType {false, "int"};
  else if (v.value.type() == TokenType::DOUBLE_VAL)
    curr_type = DataType {false, "double"};    
  else if (v.value.type() == TokenType::CHAR_VAL)
    curr_type = DataType {false, "char"};    
  else if (v.value.type() == TokenType::STRING_VAL)
    curr_type = DataType {false, "string"};    
  else if (v.value.type() == TokenType::BOOL_VAL)
    curr_type = DataType {false, "bool"};    
  else if (v.value.type() == TokenType::NULL_VAL)
    curr_type = DataType {false, "void"};    
}

void SemanticChecker::visit(FunDef& f)
{
    symbol_table.push_environment();
    unordered_set<string> paramNames;
    // check every param
    for(auto varDef : f.params) {
        string name = varDef.var_name.lexeme();
        // cannot be named with a reserved word, also checking if struct, that struct is defined
        if (BASE_TYPES.contains(name))
            error("using reserved word '" + name + "' as name", f.fun_name);
        if (paramNames.contains(name))
            error("multiple definitions of '" + name + "'", f.fun_name);
        if(!BASE_TYPES.contains(varDef.data_type.type_name) && !struct_defs.contains(varDef.data_type.type_name) && varDef.data_type.type_name != "void")
            error("type '" + varDef.data_type.type_name + "' not defined", f.fun_name);

        paramNames.insert(name);
        symbol_table.add(varDef.var_name.lexeme(), varDef.data_type);
    }

    // check return type is of valid type
    if (!BASE_TYPES.contains(f.return_type.type_name) && f.return_type.type_name != "void" && !struct_defs.contains(f.return_type.type_name))
        error("type " + f.return_type.type_name + " not defined ");
    symbol_table.add("return", f.return_type);

    for(auto s : f.stmts)
        s->accept(*this);

    symbol_table.pop_environment();
}


void SemanticChecker::visit(StructDef& s)
{
    unordered_set<string> fieldNames;
    for(auto varDef : s.fields) {
        string name = varDef.var_name.lexeme();
        // check param names are of valid words and types
        if (BASE_TYPES.contains(name))
            error("using reserved word '" + name + "' as name", s.struct_name);
        if (fieldNames.contains(name))
            error("multiple definitions of '" + name + "'", s.struct_name);
        fieldNames.insert(name);
        if(!BASE_TYPES.contains(varDef.data_type.type_name) && !struct_defs.contains(varDef.data_type.type_name))
            error("type '" + varDef.data_type.type_name + "' not defined", s.struct_name);
        symbol_table.add(varDef.var_name.lexeme(), varDef.data_type);
    }
}


void SemanticChecker::visit(ReturnStmt& s)
{
    DataType return_type = symbol_table.get("return").value();
    s.expr.accept(*this);
    // return can be of the defined type or void
    if(curr_type.type_name != return_type.type_name  && curr_type.type_name != "void")
        error("data type '" + curr_type.type_name + "' does not match '" + return_type.type_name + "'");
}


void SemanticChecker::visit(WhileStmt& s)
{
    s.condition.accept(*this);
    // condition type can only be void
    if(curr_type.type_name != "bool"  || curr_type.is_array)
        error("data type '" + curr_type.type_name + "' does not match ' bool'");
    symbol_table.push_environment();

    for (int i = 0; i < s.stmts.size(); i++)
        s.stmts[i]->accept(*this);

    symbol_table.pop_environment();
}


void SemanticChecker::visit(ForStmt& s)
{
    symbol_table.push_environment();
    // declaration must be int
    s.var_decl.accept(*this);
    string decl_type = curr_type.type_name;
    s.assign_stmt.accept(*this);
    // assignment needs to be int
    if(curr_type.type_name != "int")
        error("data type '" + curr_type.type_name + "' does not match ' int'");

    s.condition.accept(*this);
    //condition must be bool
    if(curr_type.type_name != "void" && curr_type.type_name != "bool")
        error("data type '" + curr_type.type_name + "' does not match 'bool' or 'void'");
    for (int i = 0; i < s.stmts.size(); i++)
        s.stmts[i]->accept(*this);

    symbol_table.pop_environment();
}


void SemanticChecker::visit(IfStmt& s)
{
    // condition needs to be bool
    s.if_part.condition.accept(*this);
    if(curr_type.type_name != "bool" || curr_type.is_array)
        error("data type '" + curr_type.type_name + "' does not match ' bool'");

    // new environment within if block
    symbol_table.push_environment();
    for (int i = 0; i < s.if_part.stmts.size(); i++)
        s.if_part.stmts[i]->accept(*this);
    symbol_table.pop_environment();

    // check for every if else condition bool and new environment
    for (int i = 0; i < s.else_ifs.size(); i++) {
        s.else_ifs[i].condition.accept(*this);
        if(curr_type.type_name != "bool")
            error("data type '" + curr_type.type_name + "' does not match ' bool'");

        symbol_table.push_environment();
        for (int j = 0; j < s.else_ifs[i].stmts.size(); j++)
            s.else_ifs[i].stmts[j]->accept(*this);
        symbol_table.pop_environment();
    }
    // else statement blocks run
    if(s.else_stmts.size() > 0) {
        symbol_table.push_environment();
        for (int i = 0; i < s.else_stmts.size(); i++)
            s.else_stmts[i]->accept(*this);
        symbol_table.pop_environment();
    }
}


void SemanticChecker::visit(VarDeclStmt& s)
{
    VarDef varDef = s.var_def;
    string name = varDef.data_type.type_name;
    string var_name = varDef.var_name.lexeme();

    // checks  type is valid and name has not been defined in environment
    if (!BASE_TYPES.contains(name) && name != "void" && !struct_defs.contains(name))
        error("type " + name + " not defined ");
    if(symbol_table.name_exists_in_curr_env(var_name))
        error("variable name already exists");

    s.expr.accept(*this);

    // checks type match and if array it matches
    if(curr_type.type_name != name && curr_type.type_name != "void" )
        error("expression " + name + " does not match " + curr_type.type_name + " type in declaration");
    if(varDef.data_type.is_array && !curr_type.is_array && curr_type.type_name != "void")
        error("array mismatch");
    symbol_table.add(var_name, varDef.data_type);
}


void SemanticChecker::visit(AssignStmt& s)
{
    // evaluate first value
    VarRef ref1 = s.lvalue[0];
    DataType final_type;
    string var_name =  ref1.var_name.lexeme();
    // check variable is defined and if it is get type
    if(!symbol_table.name_exists(var_name))
        error("error, variable not defined");
    else {
        std::optional<DataType> var_type = symbol_table.get(var_name);
        if(var_type.has_value())
            final_type = var_type.value();
    }

    // evaluate array
    if (ref1.array_expr.has_value()) {
        ref1.array_expr->accept(*this);
        if(curr_type.type_name != "int")
            error("array expression not int");
    }

    // go through the rest of the values to evaluate
    for(int i = 1; i < s.lvalue.size(); ++i) {
        VarRef varRef = s.lvalue[i];
        StructDef s = struct_defs[final_type.type_name];
        std::optional<VarDef> opt_field = get_field(s, varRef.var_name.lexeme());
        if(opt_field.has_value()) {
            VarDef field = opt_field.value();
            final_type = field.data_type;
            final_type.is_array = false;
        }
        else
            error("field does not exist");
        if (varRef.array_expr.has_value()) {
            varRef.array_expr->accept(*this);
            if(curr_type.type_name != "int")
                error("array expression not int");
        }
    }
    DataType type = final_type;
    string type_name = type.type_name;

    // execute expression and check type matches
    s.expr.accept(*this);
    if(curr_type.type_name != type_name && curr_type.type_name != "void" )
        error("expression " + type_name + " does not match " + curr_type.type_name + " type in assignment");

}


void SemanticChecker::visit(CallExpr& e)
{
    string fun_name = e.fun_name.lexeme();
    // check built - in types and the number of params and types
    if (fun_name == "print") {
        if (e.args.size() != 1)
            error("calling function with different number from args than declaration");
        e.args[0].accept(*this);
        curr_type = DataType {false, "void"};
    }
    else if (fun_name == "input") {
        if (e.args.size() != 0)
            error("calling function with different number from args than declaration");
        curr_type = DataType {false, "string"};
    }
    else if (fun_name == "get") {
        if (e.args.size() != 2)
            error("calling function with different number from args than declaration");
        e.args[0].accept(*this);
        if(curr_type.type_name != "int" || curr_type.is_array)
            error("first argument should be int");

        e.args[1].accept(*this);
        if(curr_type.type_name != "string" || curr_type.is_array)
            error("second argument should be string");
        curr_type = DataType {false, "char"};
    }
    else if (fun_name == "concat") {
        if (e.args.size() != 2)
            error("calling function with different number from args than declaration");
        e.args[0].accept(*this);
        if(curr_type.type_name != "string" && !curr_type.is_array)
            error("first argument should be string");

        e.args[1].accept(*this);
        if(curr_type.type_name != "string" && !curr_type.is_array)
            error("second argument should be string");
        curr_type = DataType {false, "string"};
    }
    else if (fun_name == "length") {
        if (e.args.size() != 1)
            error("calling function with different number from args than declaration");
        e.args[0].accept(*this);

        if((BASE_TYPES.contains(curr_type.type_name) && !curr_type.is_array) && curr_type.type_name != "string" )
            error("argument should be string or array");

        if(curr_type.is_array)
            e.fun_name = Token(e.fun_name.type(), "length_array", e.fun_name.line(), e.fun_name.column());

        curr_type = DataType {false, "int"};
    }
    else if (fun_name == "to_int") {
        if (e.args.size() != 1)
            error("calling function with different number from args than declaration");
        e.args[0].accept(*this);

        if(curr_type.is_array || !BASE_TYPES.contains(curr_type.type_name) || curr_type.type_name == "int" )
            error("argument should be a base type not int");

        curr_type = DataType {false, "int"};
    }
    else if (fun_name == "to_double") {
        if (e.args.size() != 1)
            error("calling function with different number from args than declaration");
        e.args[0].accept(*this);

        if(curr_type.is_array || !BASE_TYPES.contains(curr_type.type_name) || curr_type.type_name == "double" )
            error("argument should be a base type not double");

        curr_type = DataType {false, "double"};
    }
    else if (fun_name == "to_string") {
        if (e.args.size() != 1)
            error("calling function with different number from args than declaration");
        e.args[0].accept(*this);

        if(curr_type.is_array || (curr_type.type_name != "int"  && curr_type.type_name != "char" && curr_type.type_name != "double"))
            error("argument should be a base type not string");

        curr_type = DataType {false, "string"};
    }
    else {
        // check function call exists
        if (!fun_defs.contains(fun_name))
            error("calling '" + fun_name + "'  function that doesn't exist");

        FunDef& f = fun_defs[fun_name];

        // check number of args
        if (e.args.size() != f.params.size())
            error("calling function with different number from args than declaration");

        // check every argument matches its type
        for (int i = 0; i < e.args.size(); ++i) {
            DataType param_type = f.params[i].data_type;
            e.args[i].accept(*this);
            if(curr_type.type_name != param_type.type_name && curr_type.type_name != "void" )
                error("expression does not match type in function call");
        }
        curr_type = f.return_type;
    }

}


void SemanticChecker::visit(Expr& e)
{
    // get lhs type
    e.first->accept(*this);
    DataType lhs_type = curr_type;

    // if op check operator is valid with types
    if(e.op.has_value()) {
        // get rhs type
        e.rest->accept(*this);
        DataType rhs_type = curr_type;

        // check operator is compatible
        string op_val = e.op->lexeme();
        const unordered_set<string> ARITH_OPS {"+", "-", "*", "/"};
        const unordered_set<string> EQUAL_OPS {"==", "!="};
        const unordered_set<string> COMP_OPS {"<", ">", "<=", ">="};
        if(ARITH_OPS.contains(op_val)) {
            if(lhs_type.type_name != "int" && lhs_type.type_name != "double")
                error("value not compatible with operator arith");
            if(rhs_type.type_name != lhs_type.type_name)
                error("value not compatible with operator arith");
             curr_type = rhs_type;
        }
        else if(EQUAL_OPS.contains(op_val)) {
            if(rhs_type.type_name != lhs_type.type_name && rhs_type.type_name != "void" && lhs_type.type_name != "bool"&& lhs_type.type_name != "void")
                error("value " + rhs_type.type_name + " not compatible with operator equality " + lhs_type.type_name);
            curr_type = DataType {false, "bool"};
        }
        else if(COMP_OPS.contains(op_val)) {
            if(rhs_type.type_name != lhs_type.type_name)
                error("value not compatible with operator comparison");
            if(rhs_type.type_name != "string" && rhs_type.type_name != "int" && rhs_type.type_name != "char" && rhs_type.type_name != "double")
                error("value not compatible with operator comparison");
             curr_type = DataType {false, "bool"};
        }
        else if(op_val == "and" || op_val == "or")
        {
            if(rhs_type.type_name != lhs_type.type_name)
                error("value not compatible with operator and|or");
            if(rhs_type.type_name != "bool")
                error("value not compatible with operator and|or");
            curr_type = DataType {false, "bool"};
        }
    }
    else
        curr_type = lhs_type;

    if(e.negated)
        if(curr_type.type_name != "bool")
            error("value not compatible with operator");

}


void SemanticChecker::visit(SimpleTerm& t)
{
    t.rvalue->accept(*this);
} 


void SemanticChecker::visit(ComplexTerm& t)
{
    t.expr.accept(*this);
}


void SemanticChecker::visit(NewRValue& v)
{
    // check the type of new is defined
    if (!BASE_TYPES.contains(v.type.lexeme()) && !struct_defs.contains(v.type.lexeme()))
        error("type " + v.type.lexeme() + " not defined ");
    if (v.array_expr.has_value()) {
        v.array_expr->accept(*this);
        if(curr_type.type_name != "int")
            error("array expression not int");
        curr_type = DataType {true, v.type.lexeme()};
    }
    else {
        // if there is not an array value then it must be struct
        if(!struct_defs.contains(v.type.lexeme()))
            error("struct def not defined for new value");
        curr_type = DataType{false, v.type.lexeme()};
    }
}


void SemanticChecker::visit(VarRValue& v)
{
    // get first value to check vairable exists
    VarRef ref1 = v.path[0];
    string var_name =  ref1.var_name.lexeme();
    DataType final_type;
    if(!symbol_table.name_exists(var_name))
        error("error, variable not defined " + var_name);
    else {
        std::optional<DataType> var_type = symbol_table.get(var_name);
        if(var_type.has_value())
            final_type = var_type.value();
    }

    if (ref1.array_expr.has_value()) {
        ref1.array_expr->accept(*this);
        if(curr_type.type_name != "int")
            error("array expression not int");
    }

    // go through all paths
    for(int i = 1; i < v.path.size(); ++i) {
        VarRef varRef = v.path[i];
        StructDef& s = struct_defs[final_type.type_name];

        std::optional<VarDef> opt_field = get_field(s, varRef.var_name.lexeme());
        if(opt_field.has_value()) {
            VarDef field = opt_field.value();
            final_type = field.data_type;
            final_type.is_array = false;
        }
        else
            error("field " + varRef.var_name.lexeme() + " does not exist");
        if (varRef.array_expr.has_value()) {
            varRef.array_expr->accept(*this);
            if(curr_type.type_name != "int")
                error("array expression not int");
        }
    }
    curr_type = final_type;
}    

