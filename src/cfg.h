#include "program.h"
#include "basic_block.h"
#include "instruction.h"
class CFG{
public:
	map<BasicBlock*, vector<BasicBlock*> > graph;
	CFG(){}
	void init(map<Instruction*, BasicBlock*>& blocks){
		for(auto& p:blocks){
			BasicBlock* block = p.second;
			graph[block] = {};
			if(block->succ_next)
				graph[block].push_back(block->succ_next);
			if(block->succ_branch)
				graph[block].push_back(block->succ_branch);
		}
	}
	void print(){
		printf("Basic blocks: ");
		for(auto& p:graph){
			printf("%d ", p.first->getId());
		}
		printf("\nCFG:\n");
		for(auto& p:graph){
			auto v = p.second;
			printf("%d -> ", p.first->getId());
			for(auto& b:v){
				printf("%d ", b->getId());
			}
			printf("\n");
		}
	}
};