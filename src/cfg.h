#include "program.h"
#include "basic_block.h"
#include "instruction.h"
#include "util.h"
#include <algorithm>
class CFG{
public:
  int n;
  BasicBlock* root;
  set<BasicBlock*> blocks;
  map<BasicBlock*, set<BasicBlock*> > doms, inv_doms, inv_idoms;
  map<BasicBlock*, set<BasicBlock*> > df;
  map<BasicBlock*, BasicBlock*> idoms;
  CFG(){}
  void init(BasicBlock* _root, map<Instruction*, BasicBlock*>& _blocks){
    assert(_root!=NULL);
    root = _root;
    for(auto& p:_blocks){
      blocks.insert(p.second);
    }
    n = blocks.size();
    initDoms();
    initIdoms();
    initDF();
  }
  void initDoms(){
    doms[root] = {root};
    for(auto& b:blocks)
      if(b!=root)
        doms[b] = blocks;
    bool changed=true; 
    while(changed){
      changed = false;
      for(auto& b:blocks){
        if(b==root)continue;
        auto tmp = blocks;
        for(auto& p:b->getPred())
          tmp = set_intersection(tmp, doms[p]);
        tmp.insert(b);
        if(tmp!=doms[b])
          changed=true, doms[b]=tmp;
      }
    }
  }
  void initIdoms(){
    auto tmp = doms;
    for(auto& u:blocks){
      tmp[u].erase(u);
    }
    for(auto& n:blocks){
      for(auto it_s = tmp[n].begin();it_s!=tmp[n].end();it_s++){
        for(auto it_t = tmp[n].begin(); it_t!=tmp[n].end();){
          if(tmp[*it_s].find(*it_t)!=tmp[*it_s].end())
            tmp[n].erase(it_t++);
          else{
            ++it_t;
          }
        }
      }
    }
    for(auto& u:blocks){
      if(u==root)
        idoms[u] = NULL;
      else
        idoms[u] = *(tmp[u].begin());
    }
  }
  void initDF(){
    // Compute inverse dominators
    for(auto b:blocks){
      inv_doms[b] = {};
    }
    for(auto& p:doms){
      auto u = p.first;
      for(auto& v:p.second){
        inv_doms[v].insert(u);
      }
    }
    // Compute inverse struct dominators
    for(auto& p:inv_doms){
      inv_idoms[p.first] = p.second;
      inv_idoms[p.first].erase(p.first);
    }
    // Compute dominance frontier
    for(auto block:blocks){
      auto inv_dom = inv_doms[block];
      auto inv_idom = inv_idoms[block];
      set<BasicBlock*> res={};
      for(auto b:inv_dom){
        res = set_union(res, b->getSucc());
      }
      res = set_difference(res, inv_idom);
      df[block] = res;
    }
  }
  set<BasicBlock*> idf(set<BasicBlock*> s){
    auto worklist = s;
    auto idf = s;
    while(!s.empty()){
      for(auto b:df[*(s.begin())]){
        if(idf.find(b)==idf.end()){
          idf.insert(b);
          worklist.insert(b);
        }
      }
      s.erase(s.begin());
    }
    return idf;
  }
  void print(map<BasicBlock*, set<BasicBlock*> > m){
    for(auto& p:m){
      printf("%d -> ", p.first->getId());
      print(p.second);
      printf("\n");
    }
  }
  void print(map<BasicBlock*, BasicBlock*> m){
    for(auto& p:m){
      if(p.second)
        printf("%d -> %d\n", p.first->getId(), p.second->getId());
    }
  }
  void print(set<BasicBlock*> s){
    for(auto b:s){
      printf("%d ", b->getId());
    }
  }
  void print(){
    printf("Basic blocks: ");
    print(blocks);
    printf("\nCFG:\n");
    for(auto& block:blocks){
      printf("%d -> ", block->getId());
      print(block->getSucc());
      printf("\n");
    }
    printf("\nDoms:\n");
    print(doms);
    printf("\nIDoms:\n");
    print(idoms);
    printf("\nDF:\n");
    print(df);
  }
};