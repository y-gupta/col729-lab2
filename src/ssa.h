#pragma once
#include "program.h"
#include "reaching_defs.h"
#include "instruction.h"
#include "register.h"
#include "value.h"
#include "cfg.h"

class SSA{
public:
	Program *program;
	CFG *cfg;
	set<string> vars;
	BasicBlock* entry;
	map<BasicBlock*, set<Instruction*> > reach_def;
	map<string, set<Instruction*> > def_insts;
	map<string, set<BasicBlock*> > def_blocks;
	SSA(Program *_program):program(_program){
	}
	void run(bool populate_meta=false){

	}
	// void initVars(){
	// 	for(auto& p:reach_def){
	// 		for(auto& inst:p.second){
	// 			assert(inst->type == Instruction::imove);
	// 			assert(inst->op2.getType() == Value::typeReg);
	// 			vars.insert(inst->op2.reg->getName());
	// 		}
	// 	}
	// }
	// void initDefInstsAndBlocks(){
	// 	for(auto var:string){
	// 		def_insts[var]={};
	// 		def_blocks[var]={};
	// 	}
	// 	for(auto& p:reach_def){
	// 		for(auto& inst:p.second){
	// 			def_insts[inst->op2.reg->getName()].insert(inst);
	// 			def_blocks[inst->op2.reg->getName()].insert(p.first);
	// 		}
	// 	}
	// 	for(auto& p:def_insts){
	// 		int i=1;
	// 		for(auto& inst:p.second){
	// 			p.op2.reg->convert(i++);
	// 		}
	// 	}
	// }
	// void init(map<BasicBlock*, set<Instruction*> > _reach_def, BasicBlock* _entry){
	// 	assert(cfg!=NULL);
	// 	reach_def = _reach_def;
	// 	entry = _entry;
	// 	initVars();
	// 	initDefInstsAndBlocks();
	// 	for(auto var:vars){
	// 		auto idf = cfg->idf(set_union(def_blocks[var], {entry}));
	// 		for(auto& block:idf){
	// 			auto reach = reach_def[block];
	// 			// vector<Instruction*>
	// 		}
	// 	}
	// }

};
