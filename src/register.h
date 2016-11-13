#pragma once

#include <cstring>
#include <cassert>
#include <vector>

#include "emitter.h"

class Register:public CodeEmitter{
  static vector<Register*> store;
public:
  char name[64];
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
};
vector<Register*> Register::store = vector<Register*>();