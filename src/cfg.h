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
  map<BasicBlock*, set<BasicBlock*> > doms;
  map<BasicBlock*, BasicBlock*> idoms;
  CFG(){}
  void init(BasicBlock* _root, map<Instruction*, BasicBlock*>& _blocks){
    assert(_root!=NULL);
    root = _root;
    for(auto& p:_blocks){
      blocks.insert(p.second);
    }
    n = blocks.size();
    initDominators();
    initIdominators();
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
        auto tmp = blocks;
        for(auto& p:b->getPred())
          tmp = set_intersection(tmp, doms[p]);
        tmp.insert(b);
        if(tmp!=doms[b])
          changed=true, doms[b]=tmp;
      }
    }
  }
  void initIdominators(){
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
  void printIdoms(){
    printf("\nIdoms:\n");
    for(auto& p:idoms){
      if(p.second)
        printf("%d -> %d\n", p.first->getId(), p.second->getId());
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
    printIdoms();
  }
};