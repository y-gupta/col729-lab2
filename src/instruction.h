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
  Instruction():id(0),out(0),type(inop){
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
      case ibr: printf(" br"); (Instruction*)op1->emitAddr(); break;

      case iblbc: printf(" blbc"); op1.emit(); (Instruction*)op2->emitAddr(); break;
      case iblbs: printf(" blbs"); op1.emit(); (Instruction*)op2->emitAddr(); break;

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
};