#include "program.h"
#include "basic_block.h"
#include "instruction.h"
#include <algorithm>

class CFG{
public:
	int n;
	BasicBlock* root;
	set<BasicBlock*> blocks;
	map<BasicBlock*, set<BasicBlock*> > doms;
	CFG(){}
	void init(BasicBlock* _root, map<Instruction*, BasicBlock*>& _blocks){
		assert(_root!=NULL);
		root = _root;
		for(auto& p:_blocks){
			blocks.insert(p.second);
		}
		n = blocks.size();
		initDominators();
	}
	void initDominators(){
		doms[root] = {root};
		for(auto& b:blocks)
			if(b!=root)
				doms[b] = blocks;
		bool changed=true; 
		while(changed){
			changed = false;
			for(auto& b:blocks){
				if(b==root)continue;
				vector<BasicBlock*> v(n);
				auto it = v.begin();
				auto tmp = blocks;
				for(auto& p:b->getPred()){
					it = set_intersection(doms[p].begin(), doms[p].end(), tmp.begin(), tmp.end(), v.begin());
					tmp = set<BasicBlock*>(v.begin(), it);
				}
				tmp.insert(b);
				if(tmp!=doms[b])changed=true, doms[b]=tmp;
			}
		}
	}
	void printDoms(){
		printf("\nDoms:\n");
		for(auto& p:doms){
			auto block = p.first;
			auto v = p.second;
			printf("%d -> ", block->getId());
			for(auto& b:v){
				printf("%d ", b->getId());
			}
			printf("\n");
		}
	}
	void print(){
		printf("Basic blocks: ");
		for(auto& block:blocks){
			printf("%d ", block->getId());
		}
		printf("\nCFG:\n");
		for(auto& block:blocks){
			auto v = block->getSucc();
			printf("%d -> ", block->getId());
			for(auto& b:v){
				printf("%d ", b->getId());
			}
			printf("\n");
		}
		printDoms();
	}
};