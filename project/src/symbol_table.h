//----------------------------------------------------------------------
// FILE: symbol_table.h
// DATE: CPSC 326, Spring 2023
// AUTH: S. Bowers
// DESC: Symbol table interface
//----------------------------------------------------------------------

#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <vector>
#include <unordered_map>
#include "ast.h"


class SymbolTable
{

public:

  // add a new environment to the environment stack
  void push_environment();
  // remove the last added environment
  void pop_environment();
  // returns true if the symbol table has no environments
  bool empty() const;
  // add the name, with given type info, to the current environment
  void add(const std::string& name, const DataType& info);
  // true if the name exists in any environment
  bool name_exists(const std::string& name) const;
  // true if the name exists in the last pushed environment
  bool name_exists_in_curr_env(const std::string& name) const;
  // return the type info for the given name (if the name exists),
  // searching from most recent to least recent environment (returning
  // first such match)
  std::optional<DataType> get(const std::string& name) const;

  // pretty print the table for debugging
  friend std::string to_string(const SymbolTable& symbol_table);
  
private:

  // an environment is a mapping from names to type info
  std::vector<std::unordered_map<std::string,DataType>> environments;

};

#endif
