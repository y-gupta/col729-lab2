#pragma once
#include "emitter.h"
#include <cstring>
#include <map>

class Register:public CodeEmitter{
  char desc[64];
public:
  int id;
  bool is_var;
  Register(int _id=0, const char* _desc="reg"):id(_id){
    desc[63] = 0;
    strncpy(desc, _desc, 63);
    if(_id==-1)
      is_var = true;
    else
      is_var = false;
  }
  void emit() override{
    if(is_var)
      printf(" (%s) ", desc);
    else
      printf(" (%d)", id);
  }
};

class RegisterFactory{
private:
  static RegisterFactory* factory;
  
  map<int, Register> virtual_regs;
  map<string, Register> local_vars;

  RegisterFactory(){
    assert(factory == NULL);
  }
public:
  static RegisterFactory* _(){
    if(factory==NULL)
      factory = new RegisterFactory();
    return factory;
  }
  Register* getReg(const int _id){
    if(virtual_regs.find(_id)==virtual_regs.end())
      virtual_regs[_id] = Register(_id);
    return &virtual_regs[_id];
  }
  Register* getVar(const char* _id){
    string id(_id);
    if(local_vars.find(id)==local_vars.end())
      local_vars[id] = Register(-1, _id);
    return &local_vars[id];
  }
  int size(){
    return virtual_regs.size() + local_vars.size();
  }
};
RegisterFactory* RegisterFactory::factory=NULL;