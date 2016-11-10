#pragma once
#include "emitter.h"

class Constant : public CodeEmitter{
public:
  enum{
    typeLong, typeGP, typeFP 
  };
  int type, id;
  Constant(int _id=0, int _type=typeLong):id(_id),type(_type){
  }
  void emit() override{
    switch(type){
      case typeLong:  printf(" %d ",id); break;
      case typeGP:    printf(" GP "); break;
      case typeFP:    printf(" LP "); break;
      default: printf(" unknown constant");
    }
  }
};

class ConstantFactory{
private:
  static ConstantFactory* factory;
  
  map<int, Constant> constants;
  Constant gp;
  Constant fp;

  ConstantFactory():fp(0, Constant::typeFP), gp(0, Constant::typeGP){
    assert(factory == NULL);
  }
public:
  static ConstantFactory* _(){
    if(factory==NULL)
      factory = new ConstantFactory();
    return factory;
  }
  Constant* getCons(const int id){
    if(constants.find(id)==constants.end())
      constants[id] = Constant(id);
    return &constants[id];
  }
  Constant* getFP(){
    return &fp;
  }
  Constant* getGP(){
    return &gp;
  }
  int size(){
    return constants.size();
  }
  void print(){
    for(auto& p:constants){
      p.second.emit();
      cout<<endl;
    }
  }
};
ConstantFactory* ConstantFactory::factory=NULL;