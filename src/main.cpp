#include "program.h"

int main(){
  Program program("../out.3addr");
  program.emitCFG();
  return 0;
}