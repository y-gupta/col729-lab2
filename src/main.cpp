#include "ir.h"

int main(){
  IR ir("../out.3addr");
  ir.read_inst();
  return 0;
}