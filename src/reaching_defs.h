#pragma once

#include <set>
#include <map>

#include "util.h"
#include "program.h"
#include "function.h"

using namespace std;
class ReachingDefs{
public:
  Program *prog;
  map<BasicBlock*, multimap<Register*, Instruction*> > reach_in, reach_out;
  map<BasicBlock*, map<Register*, Instruction*> > gen;
  map<BasicBlock*, set<Register*> > kill;
  ReachingDefs(Program *_prog){
    prog = _prog;
  }
  void run(bool populate_meta=false){
    for(auto &inst_fn: prog->functions){
      processFunction(&(inst_fn.second));
    }
    if(populate_meta)
    for(auto &bb_defs: reach_in){
      populate_inst_meta(bb_defs.first, bb_defs.second);
    }
  };
  void populate_inst_meta(BasicBlock *bb, multimap<Register*, Instruction*> defs){
    for(auto i=bb->leader;i!=NULL;i=i->next){

      i->addMetaFunction([defs](CodeEmitter *ce){
        char tmp[1024];string meta;
        for(auto& reg_inst: defs){
          sprintf(tmp," <%s,[%d]>",reg_inst.first->name, reg_inst.second->id);
          meta += tmp;
        }
        return meta;
      });

      if(i->type != Instruction::imove)
        continue;
      assert(i->op2.getType() == Value::typeReg);
      if(!i->op2.reg->is_var)
        continue;
      defs.erase(i->op2.reg);
      defs.emplace(i->op2.reg, i);
    }
  }
  void processFunction(Function *fn, bool first=true){
    bool changed = false;
    if(first){
      auto entry_reach_in = map<Register*, Instruction*>();
      for(auto &inst_bb: fn->blocks){
        BasicBlock *bb = inst_bb.second;
        for(auto i=bb->leader;i!=NULL;i=i->next){
          if(i->op1.getType() == Value::typeReg && i->op1.reg->is_var)
            entry_reach_in[i->op1.reg] = fn->leader;
          if(i->op2.getType() == Value::typeReg && i->op2.reg->is_var)
            entry_reach_in[i->op2.reg] = fn->leader;
        }
      }
      for(auto &reg_inst: entry_reach_in)
        reach_in[fn->getBlock(fn->leader)].insert(reg_inst);
    }
    for(auto &inst_bb: fn->blocks){
      BasicBlock *bb = inst_bb.second;
      if(first){
        calcGenKill(bb);
      }
      auto &my_reach_in = reach_in[bb];
      size_t prev_sz = my_reach_in.size();
      for(auto parent: bb->preds){
        my_reach_in = util::set_union(my_reach_in,reach_out[parent]);
      }

      if(prev_sz != my_reach_in.size())
        changed = true;

      auto &my_reach_out = reach_out[bb];
      my_reach_out = my_reach_in;
      for(auto reg: kill[bb]){
        my_reach_out.erase(reg);
      }
      for(auto reg_inst: gen[bb]){
        my_reach_out.insert(reg_inst);
      }
    }
    if(changed)
      processFunction(fn,false);
  }
  void calcGenKill(BasicBlock *bb){
    gen[bb] = {};
    kill[bb] = {};
    auto &my_gen = gen[bb];
    auto &my_kill = kill[bb];
    for(auto i=bb->leader;i!=NULL;i=i->next){
      if(i->type != Instruction::imove)
        continue;
      assert(i->op2.getType() == Value::typeReg);
      if(!i->op2.reg->is_var)
        continue;
      my_kill.insert(i->op2.reg);
      my_gen.erase(i->op2.reg);
      my_gen.emplace(i->op2.reg, i);
    }
  }
};