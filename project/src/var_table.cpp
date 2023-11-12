//----------------------------------------------------------------------
// FILE: var_table.cpp
// DATE: CPSC 326, Spring 2023
// AUTH: S. Bowers
// DESC: Var table implementation
//----------------------------------------------------------------------

#include "var_table.h"


using namespace std;


void VarTable::push_environment()
{
  environments.push_back(unordered_map<string,int>());
}


void VarTable::pop_environment()
{
  if (!empty()) {
    next_index -= environments.back().size();
    environments.pop_back();
  }
}


bool VarTable::empty() const
{
  return environments.empty();
}


void VarTable::add(const string& name)
{
  if (!empty())
    environments.back()[name] = next_index++;
}


int VarTable::get(const string& name) const
{
  for (int i = environments.size() - 1; i >= 0; --i) 
    if (environments[i].contains(name))
      return environments[i].at(name);
  // couldn't find name, so return null option value
  return -1;
}


string to_string(const VarTable& var_table)
{
  string str = "";
  for (auto env : var_table.environments) {
    str += "environment: [";
    for(const auto& [var, index] : env)
      str += "\n  " + var + " -> " + to_string(index);
    str += "\n]\n";
  }
  return str;
}

  
