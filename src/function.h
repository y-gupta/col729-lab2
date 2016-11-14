#pragma once
#include "basic_block.h"
#include "cfg.h"

#include <map>
#include <deque>

using namespace std;

class Function: public CodeEmitter{
public:
	Instruction *leader, *entrypc;
	map<Instruction*, BasicBlock*> blocks;
	bool is_main;
	CFG cfg;
	Function(Instruction* _leader=NULL, bool _is_main=false):leader(_leader), is_main(_is_main),entrypc(0){
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
					block->addSuccNext(tmp);
          tmp->addPred(block);
					tmp =	getBlock(inst->op2.inst);
					block->addSuccBranch(tmp);
          tmp->addPred(block);
					break;
				case Instruction::ibr:
					tmp =	getBlock(inst->op1.inst);
					block->addSuccBranch(tmp);
          tmp->addPred(block);
					break;
				case Instruction::icall:
					tmp = getBlock(inst->next);
					block->addSuccNext(tmp);
          tmp->addPred(block);
					break;
				default:
					if(blocks.find(inst->next)!=blocks.end()){
						tmp = getBlock(inst->next);
						block->addSuccNext(tmp);
            tmp->addPred(block);
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
    if(is_main){
      assert(entrypc);
      entrypc->emit();
    }
    std::deque<BasicBlock*> list;
    list.push_back(getBlock(leader));

    while(!list.empty()){
      auto b = list.back();
      list.pop_back();
      if(b->emitted)
        continue;
      b->emit();
      if(b->succ_next){
        if(!b->succ_next->emitted)
          list.push_back(b->succ_next);
      }
      if(b->succ_branch){
        list.push_front(b->succ_branch);
      }
    }
	}
  int schedule(int id) override{
    assert(leader->id == -1);

    if(is_main) //allocate one place for entrypc
    {
      entrypc = Instruction::alloc();
      entrypc->type = Instruction::ientrypc;
      id = entrypc->schedule(id);
    }

    std::deque<BasicBlock*> list;
    list.push_back(getBlock(leader));

    while(!list.empty()){
      auto b = list.back();
      list.pop_back();
      if(b->scheduled())
        continue;
      id = b->schedule(id);
      if(b->succ_next){
        // printf("succ_next(%d):",b->leader->id);
        // b->succ_next->leader->emit();

        if(b->succ_next->scheduled()){
          // Add branch to already scheduled fall through code.
          auto br = Instruction::alloc();
          br->type = Instruction::ibr;
          br->op1.init(b->succ_next->leader);
          auto i=b->leader;
          while(i->next != NULL)
            i = i->next;
          i->next = br;
          id = br->schedule(id);
        }else{
          list.push_back(b->succ_next);
        }
      }
      if(b->succ_branch){
        // printf("succ_branch(%d):",b->leader->id);
        // b->succ_branch->leader->emit();

        list.push_front(b->succ_branch);
      }
    }
    return id;
  }
};