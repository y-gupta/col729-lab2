#pragma once
#include "emitter.h"
#include <set>
using namespace std;

class BasicBlock:public CodeEmitter{
private:
	Instruction* leader;
	set<BasicBlock*> preds, succs;
	BasicBlock *succ_next, *succ_branch;
public:
	BasicBlock(){
		leader = NULL;
		succ_next = succ_branch = NULL;
	}
	BasicBlock(Instruction* _leader):leader(_leader){
		assert(leader!=NULL);
		succ_next = succ_branch = NULL;
	}
	int getId(){
		assert(leader!=NULL);
		return leader->id;
	}
	set<BasicBlock*> getPred(){
		return preds;
	}
	set<BasicBlock*> getSucc(){
		return succs;
	}
	void addPred(BasicBlock* block){
		assert(block!=NULL);
		preds.insert(block);
	}
	void addSuccNext(BasicBlock* block){
		assert(block!=NULL);
		succ_next = block;
		succs.insert(block);
	}
	void addSuccBranch(BasicBlock* block){
		assert(block!=NULL);
		succ_branch = block;
		succs.insert(block);
	}
	void emit() override{
		printf("Leader:\n");
		leader->emit();cout<<endl;
		// Instruction* itr = leader;
		// while(itr){
		// 	itr->emit();
		// 	cout<<endl;
		// 	itr = itr->next;
		// }
		printf("Succ:\n");
		if(succ_next){
			succ_next->leader->emit();cout<<endl;
		}
		if(succ_branch){
			succ_branch->leader->emit();cout<<endl;
		}
		printf("Pred:\n");
		for(auto i:preds){
			i->leader->emit();
			cout<<endl;
		}
	}
};