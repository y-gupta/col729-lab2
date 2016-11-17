#pragma once

#include <map>
#include <set>
#include <iterator>

#include "program.h"
#include "ssa.h"

using namespace std;

class ConsPropagation{
  class Lattice{
  public:
    int type;
    long val;
    enum {
      typeConst, typeNonConst, typeUnknown
    };
    Lattice(int _type=typeUnknown,long _val=0):type(_type),val(_val){}
    bool isUnknown() const{
      return type == typeUnknown;
    }
    bool isNonConst() const{
      return type == typeNonConst;
    }
    bool isConst() const{
      return type == typeConst;
    }
    bool operator == (const Lattice &b) const{
      return (type == b.type) && (val == b.val);
    }
    bool operator != (const Lattice &b) const{
      return (type != b.type) || (val != b.val);
    }
    Lattice operator & (const Lattice &b) const{
      if(isNonConst() || b.isNonConst())
        return Lattice(typeNonConst);
      if(isUnknown())
        return b;
      if(b.isUnknown())
        return *this;
      if(val == b.val)
        return *this;
      return Lattice(typeNonConst);
    }
  };
public:
  Program *prog;
  ConsPropagation(Program *_prog){
    prog = _prog;
  };
  void run(bool output_metadata=false){
    SSA(prog).run(); //transform to SSA
    for(auto &inst_fn: prog->functions){
      processFunction(&(inst_fn.second));
    }
  };
  typedef pair<Instruction*, Instruction*> Edge;
  Instruction *entry;
  map<Instruction*, vector<Instruction*> > edges;
  map<pair<Instruction*,Instruction*>, bool> edge_label;
  map<Instruction*, vector<Instruction*> > var_uses;
  map<Register*, Instruction*> var_def;
  map<Instruction*, Lattice> lattice_cell;
  set<pair<Instruction*,Instruction*>> executable_edges;
  vector<pair<Instruction*, Instruction*> > SSA_WL, flow_WL;
  map<Instruction*, pair<Edge, Edge> > phi_edges;

  int edgeCount(Instruction* inst){
    int cnt=0;
    for(auto &edge: executable_edges){
      if(edge.second == inst)
        ++cnt;
    }
    return cnt;
  }
  void populateDef(Instruction *inst){
    if(inst->type == Instruction::imove)
      var_def[inst->op2.reg] = inst;
    if(inst->hasLHS())
      var_def[inst->out] = inst;
  }
  void populateUse(Instruction *inst){
    if(inst->hasRHS()){
      if(inst->op1.getType() == Value::typeReg)
        var_uses[var_def.at(inst->op1.reg)].push_back(inst);
      if(inst->type != Instruction::imove && inst->op2.getType() == Value::typeReg)
        var_uses[var_def.at(inst->op2.reg)].push_back(inst);
    }
  }
  set<BasicBlock*> block_flags;
  void populateEdges(BasicBlock *bb){
    if(block_flags.find(bb) != block_flags.end())
      return;
    block_flags.insert(bb);
    auto i = bb->leader;
    for(; i->next != NULL;i = i->next){
      edges[i].push_back(i->next);
      populateDef(i);
    }
    populateDef(i);
    if(i->type == Instruction::iblbs){
      assert(bb->succ_next && bb->succ_next);
      edge_label[make_pair(i,bb->succ_branch->leader)] = true;
      edge_label[make_pair(i,bb->succ_next->leader)] = false;
    }
    if(i->type == Instruction::iblbc){
      assert(bb->succ_next && bb->succ_next);
      edge_label[make_pair(i,bb->succ_branch->leader)] = false;
      edge_label[make_pair(i,bb->succ_next->leader)] = true;
    }
    for(auto bb2: bb->succs){
      edges[i].push_back(bb2->leader);
      populateEdges(bb2);
    }
  }
  void setPhiEdges(BasicBlock *bb, Instruction *i, Instruction *prev_i){
    if(i->phi_parent1 == -1)
      phi_edges[i].first = make_pair(prev_i, i);
    else{
      auto it=bb->preds.begin();
      assert(bb->preds.size() <= i->phi_parent1);
      std::advance(it, i->phi_parent1);
      auto i2 = (*it)->leader;
      while(i2->next != NULL)
        i2=i2->next;
      phi_edges[i].first = make_pair(i2, bb->leader);
    }
    if(i->phi_parent2 == -1){
      phi_edges[i].second = make_pair(prev_i, i);
    }else{
      auto it=bb->preds.begin();
      assert(bb->preds.size() <= i->phi_parent2);
      std::advance(it, i->phi_parent2);
      auto i2 = (*it)->leader;
      while(i2->next != NULL)
        i2=i2->next;
      phi_edges[i].second = make_pair(i2, bb->leader);
    }
  }
  void populateUses(BasicBlock *bb){
    if(block_flags.find(bb) != block_flags.end())
      return;
    block_flags.insert(bb);
    auto i = bb->leader;
    Instruction* prev_inst = NULL;
    for(; i != NULL;i = i->next){
      populateUse(i);
      if(i->type == Instruction::iphi){
        setPhiEdges(bb,i,prev_inst);
      }
      prev_inst = i;
    }
    for(auto bb2: bb->succs){
      populateUses(bb2);
    }
  }
  void initialize(Function *fn){
    edges.clear();
    SSA_WL.clear();
    flow_WL.clear();
    executable_edges.clear();
    edge_label.clear();
    var_uses.clear();
    var_def.clear();
    lattice_cell.clear();

    entry = fn->leader;
    block_flags.clear();
    populateEdges(fn->getBlock(entry));
    block_flags.clear();
    populateUses(fn->getBlock(entry));
    block_flags.clear();

    for(auto i2: edges[entry]){
      flow_WL.push_back(make_pair(entry, i2));
    }
    // for(auto &i_nbrs: edges){
    //   lattice_cell[i_nbrs.first] = Lattice();
    // }
  }
  Lattice latEval(Instruction *inst){
    Lattice l1,l2;
    if(inst->op1.getType() == Value::typeCons)
      l1 = Lattice(Lattice::typeConst, inst->op1.cons->val);
    else if(inst->op1.getType() == Value::typeReg)
      l1 = lattice_cell[var_def.at(inst->op1.reg)];

    if(inst->op2.getType() == Value::typeCons)
      l2 = Lattice(Lattice::typeConst, inst->op2.cons->val);
    else if(inst->op2.getType() == Value::typeReg)
      l2 = lattice_cell[var_def.at(inst->op2.reg)];

    switch(inst->type){
      case Instruction::ineg:
        if(l1.isUnknown() || l1.isNonConst())
          return l1;
        return Lattice(Lattice::typeConst,-l1.val);
      case Instruction::iadd:
      case Instruction::isub:
      case Instruction::imul:
      case Instruction::idiv:
      case Instruction::imod:
      case Instruction::icmpeq:
      case Instruction::icmple:
      case Instruction::icmplt:
        if(l1.isNonConst() || l2.isNonConst())
          return Lattice(Lattice::typeNonConst);
        if(l1.isUnknown() || l2.isUnknown())
          return Lattice(Lattice::typeUnknown);
        switch(inst->type){
          case Instruction::iadd:
            return Lattice(Lattice::typeConst,l1.val+l2.val);
          case Instruction::isub:
            return Lattice(Lattice::typeConst,l1.val-l2.val);
          case Instruction::imul:
            return Lattice(Lattice::typeConst,l1.val*l2.val);
          case Instruction::idiv:
            return Lattice(Lattice::typeConst,l1.val/l2.val);
          case Instruction::imod:
            return Lattice(Lattice::typeConst,l1.val%l2.val);
          case Instruction::icmpeq:
            return Lattice(Lattice::typeConst,l1.val == l2.val);
          case Instruction::icmple:
            return Lattice(Lattice::typeConst,l1.val <= l2.val);
          case Instruction::icmplt:
            return Lattice(Lattice::typeConst,l1.val < l2.val);
        }
        break;
      case Instruction::iblbc:
      case Instruction::iblbs:
      case Instruction::imove:
        return l1;
      case Instruction::iread:
      case Instruction::iload:
        return Lattice(Lattice::typeNonConst);
    }
    return Lattice(Lattice::typeUnknown);
  }
  void visitInst(Instruction *inst){
    auto val = latEval(inst);

    if(inst->hasLHS() || inst->type == Instruction::imove){
      auto &left_val = lattice_cell[inst];
      if(left_val != val){
        left_val = left_val & val;
        for(auto &i2: var_uses[inst])
          SSA_WL.push_back(make_pair(inst,i2));
      }
    }

    if(edges[inst].size()==2){
      if(val.isNonConst()){
        flow_WL.push_back(make_pair(inst,edges[inst][0]));
        flow_WL.push_back(make_pair(inst,edges[inst][1]));
      }else if(val.isConst()){
        for(auto &i2: edges[inst]){
          if(bool(val.val) == edge_label.at(make_pair(inst,i2))){
            flow_WL.push_back(make_pair(inst, i2));
          }
        }
      }
    }
  }
  void visitPhi(Instruction *inst){
    Lattice l1,l2;
    if(executable_edges.find(phi_edges[inst].first) != executable_edges.end())
      l1 = lattice_cell[var_def.at(inst->op1)];
    if(executable_edges.find(phi_edges[inst].second) != executable_edges.end())
      l2 = lattice_cell[var_def.at(inst->op2)];
    auto val =  l1 & l2;
    if(val != lattice_cell[inst])
    {
      lattice_cell[inst] = val;
      for(auto &i2: var_uses[inst])
        SSA_WL.push_back(make_pair(inst,i2));
    }
  }
  void processFunction(Function *fn){
    initialize(fn);
    int count = 0;
    while(1){
      if(!flow_WL.empty()){
        auto edge = flow_WL.back();
        flow_WL.pop_back();

        // edge.second->meta += " f:"+std::to_string(count);

        if(executable_edges.find(edge) == executable_edges.end()){
          executable_edges.insert(edge);

          if(edge.second->type == Instruction::iphi){
            visitPhi(edge.second);
          }else if(edgeCount(edge.second) == 1)
            visitInst(edge.second);
          if(edges[edge.second].size()==1)
            flow_WL.push_back(make_pair(edge.second, edges[edge.second][0]));
        }
      }else if(!SSA_WL.empty()){
        auto edge = SSA_WL.back();
        SSA_WL.pop_back();
        // edge.second->meta += " s:"+std::to_string(count);
        if(edge.second->type == Instruction::iphi)
          visitPhi(edge.second);
        else if(edgeCount(edge.second) >= 1)
          visitInst(edge.second);
      }else
        break;
      count++;
    }

    for(auto &i_val: lattice_cell){
      auto inst = i_val.first;
      auto &val = i_val.second;
      inst->addMetaFunction([val](CodeEmitter *){
        string res;
        if(val.isConst()){
          res = " const: "+std::to_string(val.val);
        }else if(val.isNonConst())
          res = " non-const";
        else
          res = " unknown";
        return res;
      });
      if(val.isConst())
      {

        // for(auto &i2: var_uses[inst]){
        //   if(i2->op1.type ==
        // }
      }
    }
    cerr<<"Done cons!!"<<endl;
  }
};