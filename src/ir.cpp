#include <iostream>
#include <vector>
#include <cstring>
#include <cstdio>
using namespace std;
#include "instruction.h"


class IR{
public:
  string fname;
  vector<Instruction*> instructions;
  IR(const string _fname):fname(_fname){
    // instructions[0] = NULL
    instructions.push_back(NULL);

    // instructions[1...n]
    FILE* file = fopen(fname.c_str(), "r");
    char c;
    while((c=fgetc(file))!=EOF){
      if(c=='\n')
        instructions.push_back(new Instruction());
    }
    fclose(file);
  }
  void read_op(FILE* file, Value& val){
    int i, j;
    char op[50];
    fscanf(file, "%s ",op);
    if(op[0]=='['){
      op[strlen(op)-1]='\0';
      i = atoi(op+1);
      cout<<"inst "<<i<<" ";
    }
    else if(op[0]=='('){
      op[strlen(op)-1]='\0';
      i = atoi(op+1);
      cout<<"reg "<<i<<" ";
    }
    else if(op[0]=='G'){
      cout<<"GP ";
    }
    else if(op[0]=='F'){
      cout<<"FP ";
    }
    else{
      i=0,j=0;
      while((op[j]<'0' || op[j]>'9') && op[j]!='-')
        j++;
      i = atoi(op+j);
      if(strstr(op, "_base")!=NULL){
        cout<<"base "<<i<<" ";
      }
      else if(strstr(op, "_offset")!=NULL){
        cout<<"offset "<<i<<" ";
      }
      else if(j>0){
        cout<<"virtual "<<i<<" ";
      }
      else{
        cout<<i<<" ";
      }
    }
  }
  void read_inst(){
    FILE* file = fopen(fname.c_str(), "r");
    if(!file){
      printf("Unable to open: %s\n", fname.c_str());
      return;
    }
    char opcode[10];
    int id;
    while(fscanf(file, " instr %d: %s ", &id, opcode)!=EOF){
      printf(" instr %d: %s ", id, opcode);
      Instruction* inst = instructions[id];
      if(strcmp(opcode, "add")==0){
        inst->type = Instruction::iadd;
        read_op(file, inst->op1);
        read_op(file, inst->op2);
      }
      else if(strcmp(opcode, "sub")==0){
        inst->type = Instruction::isub;
        read_op(file, inst->op1);
        read_op(file, inst->op2);
      }
      else if(strcmp(opcode, "mul")==0){
        inst->type = Instruction::imul;
        read_op(file, inst->op1);
        read_op(file, inst->op2);
      }
      else if(strcmp(opcode, "div")==0){
        inst->type = Instruction::idiv;
        read_op(file, inst->op1);
        read_op(file, inst->op2);
      }
      else if(strcmp(opcode, "mod")==0){
        inst->type = Instruction::imod;
        read_op(file, inst->op1);
        read_op(file, inst->op2);
      }
      else if(strcmp(opcode, "neg")==0){
        inst->type = Instruction::ineg;
        read_op(file, inst->op1);
      }
      else if(strcmp(opcode, "param")==0){
        inst->type = Instruction::iparam;
        read_op(file, inst->op1);
      }
      else if(strcmp(opcode, "enter")==0){
        inst->type = Instruction::ienter;
        read_op(file, inst->op1);
      }
      else if(strcmp(opcode, "entrypc")==0){
        inst->type = Instruction::ientrypc;
      }
      else if(strcmp(opcode, "leave")==0){
        inst->type = Instruction::ileave;
      }
      else if(strcmp(opcode, "ret")==0){
        inst->type = Instruction::iret;
        read_op(file, inst->op1);
      }
      else if(strcmp(opcode, "end")==0){
        inst->type = Instruction::iend;
      }
      else if(strcmp(opcode, "call")==0){
        inst->type = Instruction::icall;
        read_op(file, inst->op1);
      }
      else if(strcmp(opcode, "br")==0){
        inst->type = Instruction::ibr;
        read_op(file, inst->op1);
      }
      else if(strcmp(opcode, "blbc")==0){
        inst->type = Instruction::iblbc;
        read_op(file, inst->op1);
        read_op(file, inst->op2);
      }
      else if(strcmp(opcode, "blbs")==0){
        inst->type = Instruction::iblbs;
        read_op(file, inst->op1);
        read_op(file, inst->op2);
      }
      else if(strcmp(opcode, "cmpeq")==0){
        inst->type = Instruction::icmpeq;
        read_op(file, inst->op1);
        read_op(file, inst->op2);
      }
      else if(strcmp(opcode, "cmple")==0){
        inst->type = Instruction::icmple;
        read_op(file, inst->op1);
        read_op(file, inst->op2);
      }
      else if(strcmp(opcode, "cmplt")==0){
        inst->type = Instruction::icmplt;
        read_op(file, inst->op1);
        read_op(file, inst->op2);
      }
      else if(strcmp(opcode, "read")==0){
        inst->type = Instruction::iread;
      }
      else if(strcmp(opcode, "write")==0){
        inst->type = Instruction::iwrite;
        read_op(file, inst->op1);
      }
      else if(strcmp(opcode, "wrl")==0){
        inst->type = Instruction::iwrl;
      }
      else if(strcmp(opcode, "load")==0){
        inst->type = Instruction::iload;
        read_op(file, inst->op1);
      }
      else if(strcmp(opcode, "store")==0){
        inst->type = Instruction::istore;
        read_op(file, inst->op1);
        read_op(file, inst->op2);
      }
      else if(strcmp(opcode, "move")==0){
        inst->type = Instruction::imove;
        read_op(file, inst->op1);
        read_op(file, inst->op2);
      }
      else if(strcmp(opcode, "nop")==0){
        inst->type = Instruction::inop;
      }
      else{
        assert(false);
      }
      printf("\n");
    }
  }
};

int main(){
  IR ir("../out.3addr");
  ir.read_inst();
  return 0;
}