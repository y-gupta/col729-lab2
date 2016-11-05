#include "program.h"
#include "basic_block.h"

class CFG{
public:
	BasicBlockFactory* bfactory;
	CFG(){
		bfactory = BasicBlockFactory::_();
	}
	void create(const vector<Instruction*>& insts){
		vector<int> is_leader(insts.size(), 0);
		is_leader[0]=1;
		bfactory->getBlock(insts[0]);
		for(int i=1;i<insts.size();i++){
			switch(insts[i]->type){
				case Instruction::ienter:
					is_leader[i]=1;
					bfactory->getBlock(insts[i]);
					break;
				case Instruction::iblbc:
				case Instruction::iblbs:
					if(i<insts.size()-1)is_leader[i+1]=1;
					is_leader[((Instruction*)(insts[i]->op2))->line]=1;
					break;
				case Instruction::icall:
				case Instruction::ibr:
					if(i<insts.size()-1)is_leader[i+1]=1;
					is_leader[((Instruction*)(insts[i]->op1))->line]=1;
					break;
			}
		}
		BasicBlock* current=bfactory->getBlock(insts[0]);
	}		
};