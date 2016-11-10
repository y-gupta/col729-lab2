#pragma once

#include "emitter.h"
#include <cassert>
#include <cstdio>

class Register;
class Instruction;
class Constant;

class Value: public CodeEmitter{
  int type;
public:
  union{
    void *val;
    Register *reg;
    Constant *cons;
    Instruction *inst;
  };
  enum{
    typeNone, typeReg, typeCons, typeInst
  };
  Value():type(typeNone),val(0){}
  void init(Register *_reg){
    type = typeReg;
    reg = _reg;
  }
  void init(Instruction *_inst){
    type = typeInst;
    inst = _inst;
  }
  void init(Constant *_cons){
    type = typeCons;
    cons = _cons;
  }
  int getType(){
    return type;
  }
  bool isNone(){
    return type == typeNone;
  }
  operator Register*(){
    assert(type == typeReg);
    return (Register*)val;
  }
  operator Instruction*(){
    assert(type == typeInst);
    return (Instruction*)val;
  }
  operator Constant*(){
    assert(type == typeCons);
    return (Constant*)val;
  }
  void emit() override{
    if(type!=typeNone)
      ((CodeEmitter*)val)->emit();
    else
      printf(" None");
  }
};