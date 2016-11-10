#pragma once
#include "instruction.h"
#include "register.h"
#include "constant.h"
#include "function.h"

class Program:public CodeEmitter{
public:
  string fname;
  FILE* file;
  InstructionFactory *ifactory;
  RegisterFactory *rfactory;
  ConstantFactory *cfactory;
  map<Instruction*, Function> functions; 
  Program(const string _fname):fname(_fname){
    // File
    file = fopen(fname.c_str(), "r");
    if(!file){
      printf("Unable to open: %s\n", fname.c_str());
      return;
    }
    ifactory = InstructionFactory::_();
    rfactory = RegisterFactory::_();
    cfactory = ConstantFactory::_();
    // Parsing Instructions
    parseInst();
    // Creating Functions
    createFunctions();
  }
  ~Program(){
    fclose(file);
  }
  void parseOp(Value& val){
    int i, j;
    char op[50];
    fscanf(file, "%s ",op);
    if(op[0]=='['){
      op[strlen(op)-1]='\0';
      i = atoi(op+1);
      val.init(ifactory->getInst(i));
    }
    else if(op[0]=='('){
      op[strlen(op)-1]='\0';
      i = atoi(op+1);
      val.init(rfactory->getReg(i));
    }
    else if(op[0]=='G')
      val.init(cfactory->getGP());
    else if(op[0]=='F')
      val.init(cfactory->getFP());
    else{
      i=0,j=0;
      while((op[j]<'0' || op[j]>'9') && op[j]!='-')j++;
      i = atoi(op+j);
      if(j==0 || strstr(op, "_base")!=NULL || strstr(op, "_offset")!=NULL){
        val.init(cfactory->getCons(i));
      }
      else{
        op[j-1]=0;
        val.init(rfactory->getVar(op));
      }
    }
  }
  void parseInst(){
    char opcode[10];
    int id;
    Instruction *prev=NULL, *curr=NULL;
    bool is_main=false;
    while(fscanf(file, " instr %d: %s ", &id, opcode)!=EOF){
      curr = ifactory->getInst(id);
      parse(opcode, curr);
      if(curr->type==Instruction::ientrypc){
        is_main = true;
        prev = NULL;
      }
      else if(curr->type==Instruction::ienter){
        functions[curr] = Function(curr, is_main);
        is_main=false;
        prev = curr;
      }
      else{
        if(prev!=NULL)
          prev->next = curr;
        prev = curr;
        if(curr->type==Instruction::iret)
          prev=NULL;
      }
    }
  }
  void createFunctions(){
    for(auto& p:functions){
      p.second.init();
    }
  }
  void parse(const char *opcode, Instruction *inst){
    if(strcmp(opcode, "add")==0){
      inst->type = Instruction::iadd;
      parseOp(inst->op1);
      parseOp(inst->op2);
    }
    else if(strcmp(opcode, "sub")==0){
      inst->type = Instruction::isub;
      parseOp(inst->op1);
      parseOp(inst->op2);
    }
    else if(strcmp(opcode, "mul")==0){
      inst->type = Instruction::imul;
      parseOp(inst->op1);
      parseOp(inst->op2);
    }
    else if(strcmp(opcode, "div")==0){
      inst->type = Instruction::idiv;
      parseOp(inst->op1);
      parseOp(inst->op2);
    }
    else if(strcmp(opcode, "mod")==0){
      inst->type = Instruction::imod;
      parseOp(inst->op1);
      parseOp(inst->op2);
    }
    else if(strcmp(opcode, "neg")==0){
      inst->type = Instruction::ineg;
      parseOp(inst->op1);
    }
    else if(strcmp(opcode, "param")==0){
      inst->type = Instruction::iparam;
      parseOp(inst->op1);
    }
    else if(strcmp(opcode, "enter")==0){
      inst->type = Instruction::ienter;
      parseOp(inst->op1);
    }
    else if(strcmp(opcode, "entrypc")==0){
      inst->type = Instruction::ientrypc;
    }
    else if(strcmp(opcode, "leave")==0){
      inst->type = Instruction::ileave;
    }
    else if(strcmp(opcode, "ret")==0){
      inst->type = Instruction::iret;
      parseOp(inst->op1);
    }
    else if(strcmp(opcode, "end")==0){
      inst->type = Instruction::iend;
    }
    else if(strcmp(opcode, "call")==0){
      inst->type = Instruction::icall;
      parseOp(inst->op1);
    }
    else if(strcmp(opcode, "br")==0){
      inst->type = Instruction::ibr;
      parseOp(inst->op1);
    }
    else if(strcmp(opcode, "blbc")==0){
      inst->type = Instruction::iblbc;
      parseOp(inst->op1);
      parseOp(inst->op2);
    }
    else if(strcmp(opcode, "blbs")==0){
      inst->type = Instruction::iblbs;
      parseOp(inst->op1);
      parseOp(inst->op2);
    }
    else if(strcmp(opcode, "cmpeq")==0){
      inst->type = Instruction::icmpeq;
      parseOp(inst->op1);
      parseOp(inst->op2);
    }
    else if(strcmp(opcode, "cmple")==0){
      inst->type = Instruction::icmple;
      parseOp(inst->op1);
      parseOp(inst->op2);
    }
    else if(strcmp(opcode, "cmplt")==0){
      inst->type = Instruction::icmplt;
      parseOp(inst->op1);
      parseOp(inst->op2);
    }
    else if(strcmp(opcode, "read")==0){
      inst->type = Instruction::iread;
    }
    else if(strcmp(opcode, "write")==0){
      inst->type = Instruction::iwrite;
      parseOp(inst->op1);
    }
    else if(strcmp(opcode, "wrl")==0){
      inst->type = Instruction::iwrl;
    }
    else if(strcmp(opcode, "load")==0){
      inst->type = Instruction::iload;
      parseOp(inst->op1);
    }
    else if(strcmp(opcode, "store")==0){
      inst->type = Instruction::istore;
      parseOp(inst->op1);
      parseOp(inst->op2);
    }
    else if(strcmp(opcode, "move")==0){
      inst->type = Instruction::imove;
      parseOp(inst->op1);
      parseOp(inst->op2);
    }
    else if(strcmp(opcode, "nop")==0){
      inst->type = Instruction::inop;
    }
  }
  void emit()override{
    for(auto p:functions){
      p.second.emit();
      cout<<endl;
    }
  }
};