//----------------------------------------------------------------------
// FILE: var_table.h
// DATE: CPSC 326, Spring 2023
// AUTH: S. Bowers
// DESC: Interface for var table
//----------------------------------------------------------------------

#ifndef VAR_TABLE_H
#define VAR_TABLE_H

#include <string>
#include <vector>
#include <unordered_map>


class VarTable
{
public:

  // add a new environment to the environment stack
  void push_environment();

  // remove the last added environment
  void pop_environment();

  // returns true if the symbol table has no environments
  bool empty() const;

  // add the var name to the current environment
  void add(const std::string& name);

  // return index for most recent name (or -1 if the name doesn't exist)
  int get(const std::string& name) const;

  // pretty print the table for debugging
  friend std::string to_string(const VarTable& var_table);

private:

  // an environment is a mapping from names to type info
  std::vector<std::unordered_map<std::string,int>> environments;

  int next_index = 0;
  
};

#endif
