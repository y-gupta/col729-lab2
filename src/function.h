#pragma once
#include "basic_block.h"
#include <map>
using namespace std;

class Function: public CodeEmitter{
public:
	Instruction* leader;
	map<Instruction*, BasicBlock> blocks;
	bool is_main;
	Function(Instruction* _leader=NULL, bool _is_main=false):leader(_leader), is_main(_is_main){
	}
	void init(){
		createBlocks();
	}
	BasicBlock* getBlock(Instruction* leader){
    if(blocks.find(leader)==blocks.end()){
      blocks[leader] = BasicBlock(leader);
    }
    return &blocks[leader];
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
					if(inst->next)getBlock(inst->next);
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
					tmp = getBlock(inst->next);
					block->addSuccNext(tmp), tmp->addPred(block);
					tmp =	getBlock(inst->op1.inst);
					block->addSuccBranch(tmp), tmp->addPred(block);
					break;
				case Instruction::icall:
					tmp = getBlock(inst->next);
					block->addSuccNext(tmp), tmp->addPred(block);
					break;
			}
			prev = inst;
			inst = inst->next;
		}
	}
	void emit() override{
		for(auto& p:blocks){
			p.second.emit();
			cout<<endl;
		}
	}
};