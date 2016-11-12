#include <iostream>
#include <cstdio>

#include "program.h"

using namespace std;

int main(){
  Program program;
  // program.emitCFG();
  cout << "Final id:" <<program.schedule(1)<<endl;
  program.emit();
  cout << "CFG:"<<endl;
  program.emitCFG();
  return 0;
}
