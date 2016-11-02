#pragma once

#include "emitter.h"
#include <cstring>

class Register:public CodeEmitter{
  char desc[64];
public:
  int id;
  Register(int _id=0, const char* _desc="reg"):id(_id){
    desc[63] = 0;
    strncpy(desc, _desc, 63);
  }
  void emit() override{
    printf(" (%d)", id);
  }
};

class RegisterFactory{
  static RegisterFactory *factory = NULL;
  
  struct Store{ //each store holds 128 registers to avoid frequent new()
    Register reg[128];
    int used;
    Store *next;
    Store():next(0),used(0){}
    Register* alloc(){
      if(used<128)
        return &reg[used++];
      return NULL;
    }
  };

  //to ensure that previously allocated registers are not invalidated like std::vector
  Store *stores; //linked list of stores

  int id_counter;
public:
  RegisterFactory(){
    assert(factory == NULL);
    factory = this;

    id_counter = 0;
    stores = new Store();
  }
  ~RegisterFactory(){
    for(Store *s = stores;s != NULL;){
      Store *next = s->next;
      delete s;
      s = next;
    }
  }
  Register* alloc(){
    Register *res = stores->alloc();
    if(!res)
    {
      Store *s = new Store();
      s->next = stores;
      stores = s;
      res = stores->alloc();
      assert(res);
    }
    res.id = id_counter++;
    return res;
  }
  int size(){
    return id_counter;
  }
};