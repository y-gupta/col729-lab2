#include "value.h"

void Value::emit(){
  if(type!=typeNone)
    (CodeEmitter*)reg->emit();
  else
    printf(" None");
}