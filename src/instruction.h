#pragma once

#include <vector>
#include <map>
#include <cstdio>
#include <string>

#include "emitter.h"
#include "value.h"
#include "register.h"

class Instruction : public CodeEmitter{
  static vector<Instruction*> store;
public:
  enum{
    ineg, iadd, isub, imul, idiv, imod, iparam, ienter, ileave, iend, iload,
    istore, imove, icmpeq, icmplt, icmple, iblbs, iblbc, icall, ibr, iret,
    iread, iwrite, iwrl, inop, ientrypc, iphi
  };
  int type, id;
  Register *out;  //virtual output register
  Value op1,op2;
  Instruction *next;

  Instruction(){
    id = -1;
    next = NULL;
    type = inop;
    out = Register::alloc();
  }
  static Instruction* alloc(){
    auto inst = new Instruction();
    store.push_back(inst);
    return inst;
  }
  static void free(){
    for(auto inst: store){
      delete inst;
    }
    store.clear();
  }
  void emitAddr(){
    printf(" [%d]",id);
  }
  int schedule(int _id) override{
    id = _id;
    out->id = _id;
    return _id+1;
  }
  void omit(){
    type = inop;
  }
  void emit() override{
    printf("    inst %d:",id);
    switch(type){
      case iadd: printf(" add"); op1.emit(); op2.emit(); break;
      case isub: printf(" sub"); op1.emit(); op2.emit(); break;
      case imul: printf(" mul"); op1.emit(); op2.emit(); break;
      case idiv: printf(" div"); op1.emit(); op2.emit(); break;
      case imod: printf(" mod"); op1.emit(); op2.emit(); break;
      case ineg: printf(" neg"); op1.emit(); break;

      case iparam: printf(" param"); op1.emit(); break;
      case ienter: printf(" enter"); op1.emit(); break;
      case ientrypc: printf(" entrypc"); break;
      case ileave: printf(" leave"); break;
      case iret: printf(" ret"); op1.emit(); break;
      case iend: printf(" end"); break;

      case icall: printf(" call"); op1.inst->emitAddr(); break;
      case ibr: printf(" br"); op1.inst->emitAddr(); break;

      case iblbc: printf(" blbc"); op1.emit(); op2.inst->emitAddr(); break;
      case iblbs: printf(" blbs"); op1.emit(); op2.inst->emitAddr(); break;

      case icmpeq: printf(" cmpeq"); op1.emit(); op2.emit(); break;
      case icmple: printf(" cmple"); op1.emit(); op2.emit(); break;
      case icmplt: printf(" cmplt"); op1.emit(); op2.emit(); break;

      case iread: printf(" read"); break;
      case iwrite: printf(" write"); op1.emit(); break;
      case iwrl: printf(" wrl"); break;

      case iload: printf(" load"); op1.emit(); break;
      case istore: printf(" store"); op1.emit(); op2.emit(); break;
      case imove: printf(" move"); op1.emit(); op2.emit(); break;

      case inop: printf(" nop"); break;
      case iphi: printf(" phi"); op1.emit(); op2.emit(); break;
      default: printf(" unknown_instruction");
    }
    calcMeta();
    if(meta.size())
      printf(" ;%s", meta.c_str());
    printf("\n");
  }
};

vector<Instruction*>  Instruction::store = vector<Instruction*>();