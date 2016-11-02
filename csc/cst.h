
#ifndef __CST_H__
#define __CST_H__

#include "stdio.h"

#include "css.h"
#include "csg.h"

/* kind */
enum {ASTtimes = CSStimes, ASTdiv = CSSdiv, ASTmod = CSSmod, ASTplus = CSSplus, ASTminus = CSSminus, ASTeql = CSSeql, ASTneq = CSSneq, ASTlss = CSSlss,
      ASTleq = CSSleq, ASTgtr = CSSgtr, ASTgeq = CSSgeq, ASTfactor = CSSeof+1, ASTterm, ASTsimpleexpression, ASTunaryminus, ASTequalityexpression,
      ASTexpression, ASTfieldlist, ASTstructtype, ASTtype, ASTrecursearray, ASTidentarray, ASTidentlist, ASTvariabledecl, ASTconstantdecl, ASTdesignator,
      ASTassignmentstmt, ASTexpressionlist, ASTprocedurecall, ASTifstmt, ASTwhilestmt, ASTstmtsequence, ASTformalparametersection, ASTformalparameter,
      ASTprocedurehead, ASTprocedurebody, ASTproceduredeclaration, ASTprogram};

typedef struct ASTNodeDesc * ASTNode;

typedef struct ASTNodeDesc {
  int kind;  // node type, see enum above
  ASTNode beg, end, alt;  // first, second, and third child
  unsigned long long val;  // value
  char name[CSSidlen+1];  // string
  int num;  // node number for printing the AST only
} ASTNodeDesc;

void PrintASTNode(ASTNode n, int ind);
ASTNode ReadASTNodes(FILE *f);
void ASTCompile(char *filename);

#endif // __CST_H__
