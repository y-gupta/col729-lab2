#pragma once

#include <set>
#include <map>

#include "util.h"
#include "program.h"
#include "function.h"

using namespace std;
class CopyPropagation{
public:
  Program *prog;
  map<BasicBlock*, set<pair<Register*, Register*> > > copy_in, copy_out;
  map<BasicBlock*, set<pair<Register*, Register*> > > gen;
  map<BasicBlock*, set<Register*> > kill;
  CopyPropagation(Program *_prog){
    prog = _prog;
  }
  void run(bool populate_meta=false){
    for(auto &inst_fn: prog->functions){
      processFunction(&(inst_fn.second));
    }
    if(populate_meta)
    for(auto &bb_defs: copy_in){
      populate_inst_meta(bb_defs.first, bb_defs.second);
    }
  };
  void populate_inst_meta(BasicBlock *bb, set<pair<Register*, Register*> > copies){
    for(auto i=bb->leader;i!=NULL;i=i->next){

      i->addMetaFunction([copies](CodeEmitter *ce){
        char tmp[1024];string meta;
        for(auto& reg_reg: copies){
          sprintf(tmp," <%s,%s>",reg_reg.first->name, reg_reg.second->name);
          meta += tmp;
        }
        return meta;
      });

      if(i->type != Instruction::imove)
        continue;
      assert(i->op2.getType() == Value::typeReg);

      if(!i->op2.reg->is_var)
        continue;
       for(auto it = copies.begin(); it != copies.end();){
        if(it->first == i->op2.reg || it->second == i->op2.reg)
          it = copies.erase(it);
        else
          ++it;
      }

      if(!i->op1.reg->is_var)
        continue;
      copies.emplace(i->op2.reg, i->op1.reg);
    }
  }
  void processFunction(Function *fn, bool first=true){
    bool changed = false;
    for(auto &inst_bb: fn->blocks){
      BasicBlock *bb = inst_bb.second;
      if(first){
        calcGenKill(bb);
      }
      auto &my_copy_in = copy_in[bb];
      size_t prev_sz = my_copy_in.size();
      for(auto parent: bb->preds){
        my_copy_in = util::set_intersection(my_copy_in,copy_out[parent]);
      }

      if(prev_sz != my_copy_in.size())
        changed = true;

      auto &my_copy_out = copy_out[bb];
      my_copy_out = my_copy_in;
      for(auto &reg: kill[bb]){
        for(auto it = my_copy_out.begin(); it != my_copy_out.end();){
          if(it->first == reg || it->second == reg)
            it = my_copy_out.erase(it);
          else
            ++it;
        }
      }
      for(auto &reg_inst: gen[bb]){
        my_copy_out.insert(reg_inst);
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

      for(auto it = my_gen.begin(); it != my_gen.end();){
        if(it->first == i->op2.reg || it->second == i->op2.reg)
          it = my_gen.erase(it);
        else
          ++it;
      }

      if(!i->op1.reg->is_var)
        continue;
      my_gen.emplace(i->op2.reg, i->op1.reg);
    }
  }
};