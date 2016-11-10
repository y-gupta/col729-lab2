#pragma once
#include <iostream>
#include <cstdio>
using namespace std;

class CodeEmitter{
public:
  virtual void emit() = 0;
};