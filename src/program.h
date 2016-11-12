#pragma once

#include <string>

#include "instruction.h"
#include "register.h"
#include "constant.h"
#include "function.h"

class Program:public CodeEmitter{
public:
  string fname;
  FILE* file;
  map<int, Instruction*> inst_map;
  map<int, Register*> reg_map;
  map<string, Register*> var_map;
  Constant *FP, *GP;
  map<Instruction*, Function> functions;
  Program(const string _fname):fname(_fname){
    // File
    file = fopen(fname.c_str(), "r");
    if(!file){
      printf("Unable to open: %s\n", fname.c_str());
      return;
    }
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

      if(inst_map[i] == NULL){
        inst_map[i] = Instruction::alloc();
        reg_map[i] = inst_map[i]->out;
      }

      val.init(inst_map[i]);
    }
    else if(op[0]=='('){
      op[strlen(op)-1]='\0';
      i = atoi(op+1);
      if(reg_map[i] == NULL)
        reg_map[i] = Register::alloc();
      val.init(reg_map[i]);
    }
    else if(op[0]=='G')
      val.init(GP);
    else if(op[0]=='F')
      val.init(FP);
    else{
      i=0,j=0;
      while((op[j]<'0' || op[j]>'9') && op[j]!='-')j++;
      i = atoi(op+j);
      if(j==0 || strstr(op, "_base")!=NULL || strstr(op, "_offset")!=NULL){
        val.init(Constant::alloc(i));
      }
      else{
        op[j-1]=0;
        string name(op);
        if(var_map[name] == NULL)
          var_map[name] = Register::allocVar(name.c_str());
        val.init(var_map[name]);
      }
    }
  }
  void parseInst(){
    char opcode[10];
    int id;
    Instruction *prev=NULL, *curr=NULL;
    bool is_main=false;
    while(fscanf(file, " instr %d: %s ", &id, opcode) != EOF){
      curr = inst_map[id];
      if(curr == NULL){
        curr = Instruction::alloc();
        reg_map[id] = curr->out;
        inst_map[id] = curr;
      }
      // curr->id = id;    // Debug purposes (CFG)
      parse(opcode, curr);
      if(curr->type == Instruction::ientrypc){
        is_main = true;
        prev = NULL;
      }else if(curr->type == Instruction::ienter){
        functions[curr] = Function(curr, is_main);
        is_main=false;
        prev = curr;
      }else{
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
  void emitCFG(){
    for(auto p:functions){
      p.second.emitCFG();
    }
  }
  void emit(){
    for(auto p:functions){
      p.second.emit();
    }
  }
};