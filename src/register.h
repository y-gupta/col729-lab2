#pragma once

#include <cstring>
#include <cassert>
#include <vector>

#include "emitter.h"

class Register:public CodeEmitter{
  char name[64];
  static vector<Register*> store;
public:
  int id;
  bool is_var;
  Register(bool _is_var=false,const char* _name="reg"){
    id = -1;
    name[63] = 0;
    strncpy(name, _name, 63);
    is_var = _is_var;
  }
  void emit() override{
    if(is_var)
      printf(" %s", name);
    else
      printf(" (%d)", id);
  }
  void convert(int i){
    string s = string(name) + "$" + to_string(i);
    strncpy(name, s.c_str(), 63);
  }
  static Register* alloc(){
    auto reg = new Register(false);
    store.push_back(reg);
    return reg;
  }
  static Register* allocVar(const char* name){
    auto reg = new Register(true, name);
    store.push_back(reg);
    return reg;
  }
  static void free(){
    for(auto reg: store){
      delete reg;
    }
    store.clear();
  }
  string getName(){
    return string(name);
  }
};
vector<Register*> Register::store = vector<Register*>();