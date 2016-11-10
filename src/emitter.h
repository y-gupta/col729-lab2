#pragma once
#include <iostream>
#include <cstdio>
using namespace std;

class CodeEmitter{
public:
  string meta;
  virtual void emit() = 0;
};