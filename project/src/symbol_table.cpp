//----------------------------------------------------------------------
// FILE: symbol_table.cpp
// DATE: CPSC 326, Spring 2023
// AUTH: S. Bowers
// DESC: Symbol table implementation
//----------------------------------------------------------------------

#include "symbol_table.h"


using namespace std;


void SymbolTable::push_environment()
{
  environments.push_back(unordered_map<string,DataType>());
}


void SymbolTable::pop_environment()
{
  if (!empty())
    environments.pop_back();
}


bool SymbolTable::empty() const
{
  return environments.empty();
}


void SymbolTable::add(const string& name, const DataType& info)
{
  if (!empty())
    environments.back()[name] = info;
}

bool SymbolTable::name_exists(const string& name) const
{
  for (int i = environments.size() - 1; i >= 0; --i)
    if (environments[i].contains(name))
      return true;
  return false;
}


bool SymbolTable::name_exists_in_curr_env(const string& name) const
{
  return !empty() and environments.back().contains(name);
}


optional<DataType> SymbolTable::get(const string& name) const
{
  for (int i = environments.size() - 1; i >= 0; --i) 
    if (environments[i].contains(name))
      return environments[i].at(name);
  // couldn't find name, so return null option value
  return nullopt;
}


string to_string(const SymbolTable& symbol_table)
{
  string str = "";
  for (auto env : symbol_table.environments) {
    str += "environment: [";
    for(const auto& [var, type] : env) {
      str += "\n  " + var + " -> " + type.type_name;
      if (type.is_array)
        str += " (is_array = true)";
      else
        str += " (is_array = false)";
    }
    str += "\n]\n";
  }
  return str;
}

  
