#pragma once

#include "emitter.h"
#include <cassert>
#include <cstdio>

class Register;
class Instruction;
class Constant;

class Value: public CodeEmitter{
  int type;
  void *val;
public:
  enum{
    typeNone, typeReg, typeCons, typeInst
  };

  Value():type(typeNone),val(0){}
  Value(Register *reg):type(typeReg),val(reg){}
  Value(Instruction *inst):type(typeInst),val(inst){}
  Value(Constant *cons):type(typeCons),val(cons){}
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
  void emit() override;
};