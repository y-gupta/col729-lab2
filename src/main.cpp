#include <iostream>
#include <cstdio>

#include "program.h"

using namespace std;

int main(){
  Program program("out.3addr");
  // program.emitCFG();
  cout << "Final id:" <<program.schedule(0)<<endl;
  program.emit();
  cout << "CFG:"<<endl;
  program.emitCFG();
  return 0;
}
