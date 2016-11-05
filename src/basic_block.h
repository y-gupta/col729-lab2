#pragma once

#include "emitter.h"

class BasicBlock:public CodeEmitter{
private:
	Instruction* leader;
	vector<BasicBlock*> preds;
	BasicBlock *succ_next, *succ_branch;
public:
	BasicBlock(){
		leader = NULL;		
		succ_next = NULL;
		succ_branch = NULL;
	}
	BasicBlock(Instruction* _leader):leader(_leader){
		assert(leader!=NULL);
		succ_next = NULL;
		succ_branch = NULL;
	}
	void addPred(BasicBlock* block){
		assert(block!=NULL);
		preds.push_back(block);
	}
	void addSuccNext(BasicBlock* block){
		assert(block!=NULL);
		succ_next = block;
	}
	void addSuccBranch(BasicBlock* block){
		assert(block!=NULL);
		succ_branch = block;
	}
	void emit() override{
		Instruction* itr = leader;
		while(itr){
			itr->emit();
			cout<<endl;
			itr = itr->next;
		}
	}
};