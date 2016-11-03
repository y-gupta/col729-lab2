#include "value.h"

void Value::emit(){
  if(type!=typeNone)
    ((CodeEmitter*)val)->emit();
  else
    printf(" None");
}