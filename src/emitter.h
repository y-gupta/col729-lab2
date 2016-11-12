#pragma once
#include <iostream>
#include <cstdio>
using namespace std;

class CodeEmitter{
public:
  string meta;
  virtual int schedule(int _id){
    return _id;
  };
  virtual void emit() = 0;
};