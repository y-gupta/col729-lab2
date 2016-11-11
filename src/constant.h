#pragma once

#include <vector>
#include <cassert>
#include <cstdio>

#include "emitter.h"

class Constant : public CodeEmitter{
  static vector<Constant*> store;
public:
  enum{
    typeLong, typeGP, typeFP
  };
  int type, val;
  Constant(int _val=0, int _type=typeLong):val(_val),type(_type){
  }
  void emit() override{
    switch(type){
      case typeLong:  printf(" %d",val); break;
      case typeGP:    printf(" GP"); break;
      case typeFP:    printf(" LP"); break;
      default: printf(" unknown_constant");
    }
  }
  static Constant* alloc(int val=0,int type=typeLong){
    auto cons = new Constant(val, type);
    store.push_back(cons);
    return cons;
  }
  static void free(){
    for(auto cons: store){
      delete cons;
    }
    store.clear();
  }
};

vector<Constant*> Constant::store = vector<Constant*>();