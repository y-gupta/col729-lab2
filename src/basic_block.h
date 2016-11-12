#pragma once

#include <cassert>
#include <set>

#include "emitter.h"
#include "instruction.h"

using namespace std;

class BasicBlock:public CodeEmitter{
private:
	set<BasicBlock*> preds, succs;
  bool emitted;
public:
  Instruction *leader;
  BasicBlock *succ_next, *succ_branch;
	BasicBlock(Instruction* _leader=NULL):leader(_leader){
		assert(leader!=NULL);
    emitted = false;
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
  void emitCFG(){
    printf("Leader:\n");
    leader->emit();cout<<endl;
    // Instruction* itr = leader;
    // while(itr){
    //  itr->emit();
    //  cout<<endl;
    //  itr = itr->next;
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
  int schedule(int id) override{
    if(leader->id != -1) // already scheduled!
      return id;
    for(auto i = leader; i != NULL; i = i->next){
      id = i->schedule(id);
      i->emit();
    }
    return id;
  }
  bool scheduled(){
    return leader->id != -1;
  }
	void emit() override{
    if(emitted)
      return;
    emitted = true;
    for(auto i = leader; i != NULL; i = i->next){
      i->emit();
    }
    if(succ_next){
      assert(succ_next->emitted == false);
      succ_next->emit();
    }
    // if(succ_branch)
    //   succ_branch->emit();
	}
};