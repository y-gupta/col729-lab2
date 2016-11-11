#pragma once
#include "basic_block.h"
#include "cfg.h"
#include <map>
using namespace std;

class Function: public CodeEmitter{
public:
	Instruction* leader;
	map<Instruction*, BasicBlock*> blocks;
	bool is_main;
	CFG cfg;
	Function(Instruction* _leader=NULL, bool _is_main=false):leader(_leader), is_main(_is_main){
	}
	void init(){
		createBlocks();
	}
	void createCFG(){
		cfg.init(getBlock(leader), blocks);
	}
	BasicBlock* getBlock(Instruction* leader){
    if(blocks.find(leader)==blocks.end()){
      blocks[leader] = new BasicBlock(leader);
    }
    return blocks[leader];
  }
	void createBlocks(){
		// Creating leaders
		getBlock(leader);
		Instruction *inst = leader->next;
		while(inst){
			switch(inst->type){
				case Instruction::iblbc:
				case Instruction::iblbs:
					if(inst->next)getBlock(inst->next);
					getBlock(inst->op2.inst);
					break;
				case Instruction::ibr:
					getBlock(inst->op1.inst);
					break;
				case Instruction::icall:
					if(inst->next)getBlock(inst->next);
					break;
			}
			inst = inst->next;
		}
		// Creating basic block succ, pred
		Instruction *prev=NULL;
		inst = leader;
		BasicBlock *block, *tmp;
		while(inst){
			if(blocks.find(inst)!=blocks.end()){	// inst -> leader
				if(prev)prev->next=NULL;						// cut the prev link
				block = getBlock(inst);
			}
			switch(inst->type){
				case Instruction::iblbc:
				case Instruction::iblbs:
					tmp = getBlock(inst->next);
					block->addSuccNext(tmp), tmp->addPred(block);
					tmp =	getBlock(inst->op2.inst);
					block->addSuccBranch(tmp), tmp->addPred(block);
					break;
				case Instruction::ibr:
					tmp =	getBlock(inst->op1.inst);
					block->addSuccBranch(tmp), tmp->addPred(block);
					break;
				case Instruction::icall:
					tmp = getBlock(inst->next);
					block->addSuccNext(tmp), tmp->addPred(block);
					break;
				default:
					if(blocks.find(inst->next)!=blocks.end()){
						tmp = getBlock(inst->next);
						block->addSuccNext(tmp), tmp->addPred(block);
					}
					break;
			}
			prev = inst;
			inst = inst->next;
		}
		createCFG();
	}
  void emitCFG(){
    printf("Function: %d\n", leader->id);
    cfg.print();
    printf("\n");
  }
	void emit() override{
    getBlock(leader)->emit();
	}
};