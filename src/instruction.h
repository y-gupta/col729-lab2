#pragma once

#include <vector>

#include "emitter.h"
#include "value.h"
#include "register.h"

class Instruction : public CodeEmitter{
public:
  enum{
    ineg, iadd, isub, imul, idiv, imod, iparam, ienter, ileave, iend, iload,
    istore, imove, icmpeq, icmplt, icmple, iblbs, iblbc, icall, ibr, iret,
    iread, iwrite, iwrl, inop, ientrypc
  };
  int type, id;
  Register *out;  //virtual output register, may be NULL
  Value op1,op2;
  Instruction *next;
  Instruction():id(0),out(0),type(inop),next(NULL){
  }
  void emitAddr(){
    printf(" [%d]",id);
  }
  void emit() override{
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

      case icall: printf(" call"); op1.emit(); break;
      case ibr: printf(" br"); op1.emit(); break;

      case iblbc: printf(" blbc"); op1.emit(); op2.emit(); break;
      case iblbs: printf(" blbs"); op1.emit(); op2.emit(); break;

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
      default: printf(" unknown_instruction");
    }
  }
};

class InstructionFactory{
private:
  static InstructionFactory* factory;
  
  map<int, Instruction> instructions;

  InstructionFactory(){
    assert(factory == NULL);
  }
public:
  static InstructionFactory* _(){
    if(factory==NULL)
      factory = new InstructionFactory();
    return factory;
  }
  Instruction* getInst(const int id){
    if(instructions.find(id)==instructions.end())
      instructions[id] = Instruction();
    return &instructions[id];
  }
  int size(){
    return instructions.size();
  }
  void print(){
    for(auto& p:instructions){
      p.second.emit();
      cout<<endl;
    }
  }
};
InstructionFactory* InstructionFactory::factory=NULL;