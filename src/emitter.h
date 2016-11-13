#pragma once
#include <iostream>
#include <cstdio>
#include <functional>
#include <vector>
using namespace std;

class CodeEmitter{
  vector< function<string(CodeEmitter*)> > meta_fns;
public:
  string meta;
  virtual int schedule(int _id){
    return _id;
  };
  virtual void emit() = 0;
  virtual void calcMeta(){
    for(auto &fn: meta_fns){
      meta += fn(this);
    }
  }
  virtual void addMetaFunction(function<string(CodeEmitter*)> fn){
    meta_fns.push_back(fn);
  }
};