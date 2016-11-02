#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "css.h"
#include "csg.h"
#include "cst.h"


static int sym;
static int instruct;
static int tos;
static CSGNode globscope;

static int astcnt = 1;

static ASTNode NewASTNode(int kind)
{
  register ASTNode astnode;

  astnode = malloc(sizeof(ASTNodeDesc));
  assert(astnode != NULL);
  astnode->kind = kind;
  astnode->beg = NULL;
  astnode->end = NULL;
  astnode->alt = NULL;
  astnode->val = 0;
  astnode->name[0] = 0;
  astnode->num = astcnt++;

  return astnode;
}


static void Indent(int ind)
{
  int i;
  for (i = 0; i < ind; i++) {
    printf("  ");
  }
}


void PrintASTNode(ASTNode n, int ind)
{
  register ASTNode curr;

  if (n != NULL) {
    switch (n->kind) {
      case ASTeql:
        PrintASTNode(n->beg, ind);
        printf(" == ");
        PrintASTNode(n->end, ind);
        break;
      case ASTneq:
        PrintASTNode(n->beg, ind);
        printf(" != ");
        PrintASTNode(n->end, ind);
        break;
      case ASTlss:
        PrintASTNode(n->beg, ind);
        printf(" < ");
        PrintASTNode(n->end, ind);
        break;
      case ASTleq:
        PrintASTNode(n->beg, ind);
        printf(" <= ");
        PrintASTNode(n->end, ind);
        break;
      case ASTgtr:
        PrintASTNode(n->beg, ind);
        printf(" > ");
        PrintASTNode(n->end, ind);
        break;
      case ASTgeq:
        PrintASTNode(n->beg, ind);
        printf(" >= ");
        PrintASTNode(n->end, ind);
        break;
      case ASTfactor:
        if (n->beg != NULL) {
          if (n->beg->kind != ASTexpression) {
            PrintASTNode(n->beg, ind);
          } else {
            printf("(");
            PrintASTNode(n->beg, ind);
            printf(")");
          }
        } else {
          printf("%lld", n->val);
        }
        break;
      case ASTterm:
        PrintASTNode(n->beg, ind);
        curr = n->end;
        while (curr != NULL) {
          if (curr->kind == ASTtimes) {
            printf("*");
          } else if (curr->kind == ASTdiv) {
            printf("/");
          } else {
            printf("%%");
          }
          PrintASTNode(curr->beg, ind);
          curr = curr->end;
        }
        break;
      case ASTunaryminus:
        printf("-");
      case ASTsimpleexpression:
        PrintASTNode(n->beg, ind);
        curr = n->end;
        while (curr != NULL) {
          if (curr->kind == ASTplus) {
            printf("+");
          } else {
            printf("-");
          }
          PrintASTNode(curr->beg, ind);
          curr = curr->end;
        }
        break;
      case ASTequalityexpression:
      case ASTexpression:
        PrintASTNode(n->beg, ind);
        break;
      case ASTfieldlist:
        PrintASTNode(n->beg, ind);
        curr = n->end;
        while (curr != NULL) {
          PrintASTNode(curr->beg, ind);
          curr = curr->end;
        }
        break;
      case ASTstructtype:
        printf("struct %s", n->name);
        if (n->beg != NULL) {
          printf(" {\n");
          PrintASTNode(n->beg, ind+1);
          Indent(ind);
          printf("}");
        }
        break;
      case ASTtype:
        printf("%s", n->name);
        break;
      case ASTrecursearray:
        printf("[");
        PrintASTNode(n->beg, ind);
        printf("]");
        PrintASTNode(n->end, ind);
        break;
      case ASTidentarray:
        printf("%s", n->name);
        PrintASTNode(n->beg, ind);
        break;
      case ASTformalparameter:
      case ASTexpressionlist:
      case ASTidentlist:
        PrintASTNode(n->beg, ind);
        curr = n->end;
        while (curr != NULL) {
          printf(", ");
          PrintASTNode(curr->beg, ind);
          curr = curr->end;
        }
        break;
      case ASTvariabledecl:
        Indent(ind);
        PrintASTNode(n->beg, ind);
        printf(" ");
        PrintASTNode(n->end, ind);
        printf(";\n");
        break;
      case ASTconstantdecl:
        Indent(ind);
        printf("const ");
        PrintASTNode(n->beg, ind);
        printf(" %s = ", n->name);
        PrintASTNode(n->end, ind);
        printf(";\n");
        break;
      case ASTdesignator:
        printf("%s", n->name);
        curr = n->end;
        while (curr != NULL) {
          if (curr->beg == NULL) {
            printf(".%s", curr->name);
          } else {
            printf("[");
            PrintASTNode(curr->beg, ind);
            printf("]");
          }
          curr = curr->end;
        }
        break;
      case ASTassignmentstmt:
        Indent(ind);
        PrintASTNode(n->beg, ind);
        printf(" = ");
        PrintASTNode(n->end, ind);
        printf(";\n");
        break;
      case ASTprocedurecall:
        Indent(ind);
        printf("%s(", n->name);
        PrintASTNode(n->beg, ind);
        printf(");\n");
        break;
      case ASTifstmt:
        Indent(ind);
        printf("if (");
        PrintASTNode(n->beg, ind);
        printf(") {\n");
        PrintASTNode(n->end, ind+1);
        if (n->alt != NULL) {
          Indent(ind);
          printf("} else {\n");
          PrintASTNode(n->alt, ind+1);
        }
        Indent(ind);
        printf("}\n");
        break;
      case ASTwhilestmt:
        Indent(ind);
        printf("while (");
        PrintASTNode(n->beg, ind);
        printf(") {\n");
        PrintASTNode(n->end, ind+1);
        Indent(ind);
        printf("}\n");
        break;
      case ASTstmtsequence:
        PrintASTNode(n->beg, ind);
        PrintASTNode(n->end, ind);
        break;
      case ASTformalparametersection:
        PrintASTNode(n->beg, ind);
        printf(" %s", n->name);
        break;
      case ASTprocedurehead:
        printf("%s(", n->name);
        PrintASTNode(n->beg, ind);
        printf(")\n");
        break;
      case ASTprogram:
      case ASTprocedurebody:
        PrintASTNode(n->beg, ind);
        curr = n->end;
        while (curr != NULL) {
          PrintASTNode(curr->beg, ind);
          curr = curr->end;
        }
        break;
      case ASTproceduredeclaration:
        Indent(ind);
        printf("\nvoid ");
        PrintASTNode(n->beg, ind);
        Indent(ind);
        printf("{\n");
        PrintASTNode(n->end, ind+1);
        Indent(ind);
        printf("}\n");
        break;
    }
  }
}


ASTNode ReadASTNodes(FILE *f)
{
  int i, j, cnt, ptr;
  ASTNode *nodes;

  fscanf(f, "%d\n", &cnt);
  assert(cnt >= 1);

  cnt++;
  nodes = malloc(sizeof(ASTNode) * cnt);
  assert(nodes != NULL);
  nodes[0] = NULL;
  astcnt = 1;
  for (i = 1; i < cnt; i++) {
    nodes[i] = NewASTNode(0);
  }

  for (i = 1; i < cnt; i++) {
    fscanf(f, "%d %d ", &(nodes[i]->num), &(nodes[i]->kind));
    fscanf(f, "%d ", &ptr);
    nodes[i]->beg = nodes[ptr];
    fscanf(f, "%d ", &ptr);
    nodes[i]->end = nodes[ptr];
    fscanf(f, "%d ", &ptr);
    nodes[i]->alt = nodes[ptr];
    fscanf(f, "%lld \"%s\n", &(nodes[i]->val), &(nodes[i]->name));
    j = 0;
    while (nodes[i]->name[j] != '\"') j++;
    nodes[i]->name[j] = 0;
  }
  fclose(f);

  return nodes[1];
}


static void EmitASTNode(ASTNode n, FILE *f)
{
  if (n != NULL) {
    fprintf(f, "%d %d ", n->num, n->kind);
    if (n->beg == NULL) {
      fprintf(f, "0 ");
    } else {
      fprintf(f, "%d ", n->beg->num);
    }
    if (n->end == NULL) {
      fprintf(f, "0 ");
    } else {
      fprintf(f, "%d ", n->end->num);
    }
    if (n->alt == NULL) {
      fprintf(f, "0 ");
    } else {
      fprintf(f, "%d ", n->alt->num);
    }
    fprintf(f, "%lld \"%s\"\n", n->val, n->name);

    EmitASTNode(n->beg, f);
    EmitASTNode(n->end, f);
    EmitASTNode(n->alt, f);
  }
}


static void EmitASTNodes(ASTNode n)
{
  fprintf(stdout, "%d\n", astcnt-1);
  EmitASTNode(n, stdout);
}


// This function searches for an object named id in the root scope.  If
// found, a pointer to the object is returned.  Otherwise, NULL is returned.
static CSGNode FindObj(CSGNode *root, CSSIdent *id)
{
  register int maxlev;
  register CSGNode curr;
  register CSGNode obj;

  maxlev = -1;
  curr = *root;
  obj = NULL;
  while (curr != NULL) {
    while ((curr != NULL) && ((strcmp(curr->name, *id) != 0) || (curr->lev <= maxlev))) {
      curr = curr->next;
    }
    if (curr != NULL) {
      obj = curr;
      maxlev = curr->lev;
      curr = curr->next;
    }
  }
  if (obj != NULL) {
    if (((obj->class == CSGVar) || (obj->class == CSGFld)) && ((obj->lev != 0) && (obj->lev != CSGcurlev))) {
      CSSError("object cannot be accessed");
    }
  }
  return obj;
}


// This function adds a new object at the end of the object list pointed to
// by root and returns a pointer to the new node.
static CSGNode AddToList(CSGNode *root, CSSIdent *id)
{
  register CSGNode curr;

  curr = NULL;
  if (*root == NULL) {  // first object
    curr = malloc(sizeof(CSGNodeDesc));
    assert(curr != NULL);
    *root = curr;
    if (curr == NULL) CSSError("out of memory");
    curr->class = -1;
    curr->lev = CSGcurlev;
    curr->next = NULL;
    curr->dsc = NULL;
    curr->type = NULL;
    strcpy(curr->name, *id);
    curr->val = 0;
  } else {  // linked list is not empty, add to the end of the list
    curr = *root;
    while (((curr->lev != CSGcurlev) || (strcmp(curr->name, *id) != 0)) && (curr->next != NULL)) {
      curr = curr->next;
    }
    if ((strcmp(curr->name, *id) == 0) && (curr->lev == CSGcurlev)) {
      CSSError("duplicate identifier");
    } else {
      curr->next = malloc(sizeof(CSGNodeDesc));
      assert(curr->next != NULL);
      curr = curr->next;
      if (curr == NULL) CSSError("out of memory");
      curr->class = -1;
      curr->lev = CSGcurlev;
      curr->next = NULL;
      curr->dsc = NULL;
      curr->type = NULL;
      strcpy(curr->name, *id);
      curr->val = 0;
    }
  }
  return curr;
}


// This function initializes the fields of an object.
static void InitObj(CSGNode obj, signed char class, CSGNode dsc, CSGType type, long long val)
{
  obj->class = class;
  obj->next = NULL;
  obj->dsc = dsc;
  obj->type = type;
  obj->val = val;
}


// Similar to InitObj(), but also initalizes the ENTRY POINT of a procedure.
static void InitProcObj(CSGNode obj, signed char class, CSGNode dsc, CSGType type, CSGNode entrypt)
{
  obj->class = class;
  obj->next = NULL;
  obj->dsc = dsc;
  obj->type = type;
  obj->true = entrypt;
}


/*************************************************************************/


static ASTNode Expression(CSGNode *x);
static ASTNode DesignatorM(CSGNode *x);


static ASTNode Factor(CSGNode *x)
{
  register ASTNode factor;
  register CSGNode obj;

  factor = NewASTNode(ASTfactor);
  switch (sym) {
    case CSSident:
      obj = FindObj(&globscope, &CSSid);
      if (obj == NULL) CSSError("unknown identifier");
      CSGMakeNodeDesc(x, obj);
      sym = CSSGet();  // consume ident before calling Designator
      factor->beg = DesignatorM(x);
      break;
    case CSSnumber:
      CSGMakeConstNodeDesc(x, CSGlongType, CSSval);
      factor->val = CSSval;
      sym = CSSGet();
      break;
    case CSSlparen:
      sym = CSSGet();
      factor->beg = Expression(x);
      if (sym != CSSrparen) CSSError("')' expected");
      sym = CSSGet();
      break;
    default: CSSError("factor expected"); break;
  }

  return factor;
}


static ASTNode Term(CSGNode *x)
{
  register ASTNode term, new, curr;
  register int op;
  CSGNode y;

  term = NewASTNode(ASTterm);
  term->beg = Factor(x);
  curr = term;
  while ((sym == CSStimes) || (sym == CSSdiv) || (sym == CSSmod)) {
    op = sym;
    new = NewASTNode(sym);
    curr->end = new;
    curr = new;
    sym = CSSGet();
    y = malloc(sizeof(CSGNodeDesc));
    assert(y != NULL);
    new->beg = Factor(&y);
    CSGOp2(op, x, y);
  }

  return term;
}


static ASTNode SimpleExpression(CSGNode *x)
{
  register ASTNode simex, new, curr;
  register int op;
  CSGNode y;

  simex = NewASTNode(ASTsimpleexpression);
  curr = simex;
  if ((sym == CSSplus) || (sym == CSSminus)) {
    op = sym; 
    if (sym == CSSminus) {
      simex->kind = ASTunaryminus;
    }
    sym = CSSGet();
    simex->beg = Term(x);
    CSGOp1(op, x);
  } else {
    simex->beg = Term(x);
  }
  while ((sym == CSSplus) || (sym == CSSminus)) {
    new = NewASTNode(sym);
    op = sym; 
    curr->end = new;
    curr = new;
    sym = CSSGet();
    y = malloc(sizeof(CSGNodeDesc));
    assert(y != NULL);
    new->beg = Term(&y);
    CSGOp2(op, x, y);
  }

  return simex;
}


static ASTNode EqualityExpr(CSGNode *x)
{
  register ASTNode eqexpr;
  register int op;
  CSGNode y;

  eqexpr = NewASTNode(ASTequalityexpression);
  eqexpr->beg = SimpleExpression(x);
  if ((sym == CSSlss) || (sym == CSSleq) || (sym == CSSgtr) || (sym == CSSgeq)) {
    y = malloc(sizeof(CSGNodeDesc));
    assert(y != NULL);
    op = sym; 
    eqexpr->kind = sym;
    sym = CSSGet();
    eqexpr->end = SimpleExpression(&y);
    CSGRelation(op, x, y);
  }

  return eqexpr;
}


static ASTNode Expression(CSGNode *x)
{
  register ASTNode expr;
  register int op;
  CSGNode y;

  expr = NewASTNode(ASTexpression);
  expr->beg = EqualityExpr(x);
  if ((sym == CSSeql) || (sym == CSSneq)) {
    op = sym; 
    expr->kind = sym;
    sym = CSSGet();
    y = malloc(sizeof(CSGNodeDesc));
    assert(y != NULL);
    expr->end = EqualityExpr(&y);
    CSGRelation(op, x, y);
  }

  return expr;
}


static ASTNode ConstExpression(CSGNode *expr)
{
  register ASTNode cexpr;

  cexpr = Expression(expr);
  if ((*expr)->class != CSGConst) CSSError("constant expression expected");

  return cexpr;
}


/*************************************************************************/


static ASTNode VariableDeclaration(CSGNode *root);


static ASTNode FieldList(CSGType type)
{
  register ASTNode fieldlist, tail, new;
  register CSGNode curr;

  fieldlist = NewASTNode(ASTfieldlist);
  fieldlist->beg = VariableDeclaration(&(type->fields));
  tail = fieldlist;
  while (sym != CSSrbrace) {
    new = NewASTNode(ASTfieldlist);
    new->beg = VariableDeclaration(&(type->fields));
    tail->end = new;
    tail = new;
  }
  curr = type->fields;
  if (curr == NULL) CSSError("empty structs are not allowed");
  while (curr != NULL) {
    curr->class = CSGFld;
    curr->val = type->size;
    type->size += curr->type->size;
    if (type->size > 0x7fffffff) CSSError("struct too large");
    curr = curr->next;
  }

  return fieldlist;
}


static ASTNode StructType(CSGType *type)
{
  register ASTNode st;
  register CSGNode obj;
  register int oldinstruct;
  CSSIdent id;

  st = NewASTNode(ASTstructtype);
  assert(sym == CSSstruct);
  sym = CSSGet();
  if (sym != CSSident) CSSError("identifier expected");
  strcpy(id, CSSid);
  strcpy(st->name, CSSid);
  sym = CSSGet();
  if (sym != CSSlbrace) {
    obj = FindObj(&globscope, &id);
    if (obj == NULL) CSSError("unknown struct type");
    if ((obj->class != CSGTyp) || (obj->type->form != CSGStruct)) CSSError("struct type expected");
    *type = obj->type;
  } else {
    sym = CSSGet();
    *type = malloc(sizeof(CSGTypeDesc));
    if ((*type) == NULL) CSSError("out of memory");
    (*type)->form = CSGStruct;
    (*type)->fields = NULL;
    (*type)->size = 0;
    oldinstruct = instruct;
    instruct = 1;
    st->beg = FieldList(*type);
    instruct = oldinstruct;
    if (sym != CSSrbrace) CSSError("'}' expected");
    sym = CSSGet();
    obj = AddToList(&globscope, &id);
    InitObj(obj, CSGTyp, NULL, *type, (*type)->size);
  }

  return st;
}


static ASTNode Type(CSGType *type)
{
  register ASTNode tp = NULL;
  register CSGNode obj;

  if (sym == CSSstruct) {
    tp = StructType(type);
  } else {
    if (sym != CSSident) CSSError("identifier expected");
    obj = FindObj(&globscope, &CSSid);

    tp = NewASTNode(ASTtype);
    strcpy(tp->name, CSSid);
    sym = CSSGet();
    if (obj == NULL) CSSError("unknown type");
    if (obj->class != CSGTyp) CSSError("type expected");
    *type = obj->type;
  }

  return tp;
}


static ASTNode RecurseArray(CSGType *type)
{
  register ASTNode recarray;
  register CSGType typ;
  CSGNode expr;

  recarray = NewASTNode(ASTrecursearray);
  expr = malloc(sizeof(CSGNodeDesc));
  assert(expr != NULL);
  assert(sym == CSSlbrak);
  sym = CSSGet();
  recarray->beg = ConstExpression(&expr);
  if (expr->type != CSGlongType) CSSError("constant long expression required");
  if (sym != CSSrbrak) CSSError("']' expected");
  sym = CSSGet();
  if (sym == CSSlbrak) {
    recarray->end = RecurseArray(type);
  }
  typ = malloc(sizeof(CSGTypeDesc));
  if (typ == NULL) CSSError("out of memory");
  typ->form = CSGArray;
  typ->len = expr->val;
  typ->base = *type;
  if (0x7fffffff / typ->len < typ->base->size) {
    CSSError("array size too large");
  }
  typ->size = typ->len * typ->base->size;
  *type = typ;

  return recarray;
}


static ASTNode IdentArray(CSGNode *root, CSGType type)
{
  register ASTNode identarray;
  register CSGNode obj;

  identarray = NewASTNode(ASTidentarray);
  if (sym != CSSident) CSSError("identifier expected");
  obj = AddToList(root, &CSSid);
  strcpy(identarray->name, CSSid);
  sym = CSSGet();
  if (sym == CSSlbrak) {
    identarray->beg = RecurseArray(&type);
  }
  if (instruct == 0) tos -= type->size;
  InitObj(obj, CSGVar, NULL, type, tos);

  return identarray;
}


static ASTNode IdentList(CSGNode *root, CSGType type)
{
  register ASTNode identlist, new, tail;

  identlist = NewASTNode(ASTidentlist);
  identlist->beg = IdentArray(root, type);
  tail = identlist;
  while (sym == CSScomma) {
    sym = CSSGet();
    new = NewASTNode(ASTidentlist);
    new->beg = IdentArray(root, type);
    tail->end = new;
    tail = new;
  }

  return identlist;
}


static ASTNode VariableDeclaration(CSGNode *root)
{
  register ASTNode vardecl;
  CSGType type;

  vardecl = NewASTNode(ASTvariabledecl);
  vardecl->beg = Type(&type);
  vardecl->end = IdentList(root, type);
  if (sym != CSSsemicolon) CSSError("';' expected");
  sym = CSSGet();

  return vardecl;
}


static ASTNode ConstantDeclaration(CSGNode *root)
{
  register ASTNode constdecl;
  register CSGNode obj;
  CSGType type;
  CSGNode expr;
  CSSIdent id;

  constdecl = NewASTNode(ASTconstantdecl);
  expr = malloc(sizeof(CSGNodeDesc));
  assert(expr != NULL);
  assert(sym == CSSconst);
  sym = CSSGet();
  constdecl->beg = Type(&type);
  if (type != CSGlongType) CSSError("only long supported");
  if (sym != CSSident) CSSError("identifier expected");
  strcpy(id, CSSid);
  strcpy(constdecl->name, CSSid);
  sym = CSSGet();
  if (sym != CSSbecomes) CSSError("'=' expected");
  sym = CSSGet();
  constdecl->end = ConstExpression(&expr);
  if (expr->type != CSGlongType) CSSError("constant long expression required");
  obj = AddToList(root, &id);
  InitObj(obj, CSGConst, NULL, type, expr->val);
  if (sym != CSSsemicolon) CSSError("';' expected");
  sym = CSSGet();

  return constdecl;
}


/*************************************************************************/


static ASTNode DesignatorM(CSGNode *x)
{
  register ASTNode desig, curr;
  register CSGNode obj;
  CSGNode y;

  desig = NewASTNode(ASTdesignator);
  strcpy(desig->name, CSSid);
  curr = desig;
  // CSSident already consumed
  while ((sym == CSSperiod) || (sym == CSSlbrak)) {
    if (sym == CSSperiod) {
      sym = CSSGet();
      if ((*x)->type->form != CSGStruct) CSSError("struct type expected");
      if (sym != CSSident) CSSError("field identifier expected");
      obj = FindObj(&(*x)->type->fields, &CSSid);
      curr->end = NewASTNode(ASTdesignator);
      curr = curr->end;
      strcpy(curr->name, CSSid);
      sym = CSSGet();
      if (obj == NULL) CSSError("unknown identifier");
      CSGField(x, obj);
    } else {
      sym = CSSGet();
      if ((*x)->type->form != CSGArray) CSSError("array type expected");
      y = malloc(sizeof(CSGNodeDesc));
      assert(y != NULL);
      curr->end = NewASTNode(ASTdesignator);
      curr = curr->end;
      curr->beg = Expression(&y);
      CSGIndex(x, y);
      if (sym != CSSrbrak) CSSError("']' expected");
      sym = CSSGet();
    }
  }

  return desig;
}


static ASTNode AssignmentM(CSGNode *x)
{
  register ASTNode astmt;
  CSGNode y;

  astmt = NewASTNode(ASTassignmentstmt);
  assert(x != NULL);
  assert(*x != NULL);
  // CSSident already consumed
  y = malloc(sizeof(CSGNodeDesc));
  assert(y != NULL);
  astmt->beg = DesignatorM(x);
  if (sym != CSSbecomes) CSSError("'=' expected");
  sym = CSSGet();
  astmt->end = Expression(&y);
  CSGStore(*x, y);
  if (sym != CSSsemicolon) CSSError("';' expected");
  sym = CSSGet();

  return astmt;
}


static ASTNode ExpList(CSGNode proc)
{
  register ASTNode explist, new, cur;
  register CSGNode curr;
  CSGNode x;

  x = malloc(sizeof(CSGNodeDesc));
  assert(x != NULL);
  curr = proc->dsc;
  explist = NewASTNode(ASTexpressionlist);
  explist->beg = Expression(&x);
  cur = explist;
  if ((curr == NULL) || (curr->dsc != proc)) CSSError("too many parameters");
  if (x->type != curr->type) CSSError("incorrect type");
  CSGParameter(&x, curr->type, curr->class);
  curr = curr->next;
  while (sym == CSScomma) {
    x = malloc(sizeof(CSGNodeDesc));
    assert(x != NULL);
    sym = CSSGet();
    new = NewASTNode(ASTexpressionlist);
    new->beg = Expression(&x);
    cur->end = new;
    cur = new;
    if ((curr == NULL) || (curr->dsc != proc)) CSSError("too many parameters");
    if (x->type != curr->type) CSSError("incorrect type");
    CSGParameter(&x, curr->type, curr->class);
    curr = curr->next;
  }
  if ((curr != NULL) && (curr->dsc == proc)) CSSError("too few parameters");

  return explist;
}


static ASTNode ProcedureCallM(CSGNode obj, CSGNode *x)
{
  register ASTNode pcall;
  CSGNode y;

  pcall = NewASTNode(ASTprocedurecall);
  pcall->val = -1;
  strcpy(pcall->name, CSSid);
  // CSSident already consumed
  CSGMakeNodeDesc(x, obj);
  if (sym != CSSlparen) CSSError("'(' expected");
  sym = CSSGet();
  if ((*x)->class == CSGSProc) {
    pcall->val = (*x)->val;
    y = malloc(sizeof(CSGNodeDesc));
    assert(y != NULL);
    if ((*x)->val == 1) {
      if (sym != CSSident) CSSError("identifier expected");
      obj = FindObj(&globscope, &CSSid);
      if (obj == NULL) CSSError("unknown identifier");
      CSGMakeNodeDesc(&y, obj);
      sym = CSSGet();  // consume ident before calling Designator
      pcall->beg = DesignatorM(&y);
    } else if ((*x)->val == 2) {
      pcall->beg = Expression(&y);
    }
    CSGIOCall(*x, y);
  } else {
    assert((*x)->type == NULL);
    if (sym != CSSrparen) {
      pcall->beg = ExpList(obj);
    } else {
      if ((obj->dsc != NULL) && (obj->dsc->dsc == obj)) CSSError("too few parameters");
    }
    CSGCall(*x);
  }
  if (sym != CSSrparen) CSSError("')' expected");
  sym = CSSGet();
  if (sym != CSSsemicolon) CSSError("';' expected");
  sym = CSSGet();

  return pcall;
}


static ASTNode StatementSequence(void);


// This function parses if statements - helpful for CFG creation.
static ASTNode IfStatement(void)
{
  register ASTNode ifstmt;
  CSGNode label;
  CSGNode x;

  ifstmt = NewASTNode(ASTifstmt);
  ifstmt->end = NULL;
  x = malloc(sizeof(CSGNodeDesc));
  assert(x != NULL);
  assert(sym == CSSif);
  sym = CSSGet();
  CSGInitLabel(&label);
  if (sym != CSSlparen) CSSError("'(' expected");
  sym = CSSGet();
  ifstmt->beg = Expression(&x);
  CSGTestBool(&x);
  CSGFixLink(x->false);
  if (sym != CSSrparen) CSSError("')' expected");
  sym = CSSGet();
  if (sym != CSSlbrace) CSSError("'{' expected");
  sym = CSSGet();
  ifstmt->end = StatementSequence();
  if (sym != CSSrbrace) CSSError("'}' expected");
  sym = CSSGet();
  if (sym == CSSelse) {
    sym = CSSGet();
    CSGFJump(&label);
    CSGFixLink(x->true);
    if (sym != CSSlbrace) CSSError("'{' expected");
    sym = CSSGet();
    ifstmt->alt = StatementSequence();
    if (sym != CSSrbrace) CSSError("'}' expected");
    sym = CSSGet();
  } else {
    CSGFixLink(x->true);
  }
  CSGFixLink(label);

  return ifstmt;
}


// This function parses while statements - helpful for CFG creation.
static ASTNode WhileStatement(void)
{
  register ASTNode wstmt;
  CSGNode label;
  CSGNode x;

  wstmt = NewASTNode(ASTwhilestmt);
  x = malloc(sizeof(CSGNodeDesc));
  assert(x != NULL);
  assert(sym == CSSwhile);
  sym = CSSGet();
  if (sym != CSSlparen) CSSError("'(' expected");
  sym = CSSGet();
  CSGSetLabel(&label);
  wstmt->beg = Expression(&x);
  CSGTestBool(&x);
  CSGFixLink(x->false);
  if (sym != CSSrparen) CSSError("')' expected");
  sym = CSSGet();
  if (sym != CSSlbrace) CSSError("'{' expected");
  sym = CSSGet();
  wstmt->end = StatementSequence();
  if (sym != CSSrbrace) CSSError("'}' expected");
  sym = CSSGet();
  CSGBJump(label);
  CSGFixLink(x->true);

  return wstmt;
}


static ASTNode Statement(void)
{
  register ASTNode stmt = NULL;
  register CSGNode obj;
  CSGNode x;

  switch (sym) {
    case CSSif: stmt = IfStatement(); break;
    case CSSwhile: stmt = WhileStatement(); break;
    case CSSident:
      obj = FindObj(&globscope, &CSSid);
      if (obj == NULL) CSSError("unknown identifier");
      sym = CSSGet();
      x = malloc(sizeof(CSGNodeDesc));
      assert(x != NULL);
      if (sym == CSSlparen) {
        stmt = ProcedureCallM(obj, &x);
      } else {
        CSGMakeNodeDesc(&x, obj);
        stmt = AssignmentM(&x);
      }
      break;
    case CSSsemicolon: break;  /* empty statement */
    default: CSSError("unknown statement");
  }

  return stmt;
}


static ASTNode StatementSequence(void)
{
  register ASTNode statseq = NULL, curr, new;

  while (sym != CSSrbrace) {
    new = NewASTNode(ASTstmtsequence);
    new->beg = Statement();
    if (statseq == NULL) {
      statseq = new;
    } else {
      curr->end = new;
    }
    curr = new;
  }

  return statseq;
}


/*************************************************************************/


static ASTNode FPSection(CSGNode *root, int *paddr)
{
  register ASTNode fpsec;
  register CSGNode obj;
  CSGType type;

  fpsec = NewASTNode(ASTformalparametersection);
  fpsec->beg = Type(&type);
  if (type != CSGlongType) CSSError("only basic type formal parameters allowed");
  if (sym != CSSident) CSSError("identifier expected");
  obj = AddToList(root, &CSSid);
  strcpy(fpsec->name, CSSid);
  sym = CSSGet();
  if (sym == CSSlbrak) CSSError("no array parameters allowed");
  InitObj(obj, CSGVar, *root, type, 0);
  *paddr += type->size; 

  return fpsec;
}


static ASTNode FormalParameters(CSGNode *root)
{
  register ASTNode fp, new, cur;
  register CSGNode curr;
  int paddr;

  paddr = 16;
  fp = NewASTNode(ASTformalparameter);
  fp->beg = FPSection(root, &paddr);
  cur = fp;
  while (sym == CSScomma) {
    sym = CSSGet();
    new = NewASTNode(ASTformalparameter);
    new->beg = FPSection(root, &paddr);
    cur->end = new;
    cur = new;
  }
  curr = (*root)->next;
  while (curr != NULL) {
    paddr -= curr->type->size;
    curr->val = paddr;
    curr = curr->next;
  }

  return fp;
}


static ASTNode ProcedureHeading(CSGNode *proc)
{
  register ASTNode prochead;
  CSSIdent name;

  prochead = NewASTNode(ASTprocedurehead);
  if (sym != CSSident) CSSError("function name expected");
  strcpy(name, CSSid);
  strcpy(prochead->name, CSSid);
  *proc = AddToList(&globscope, &name);
  InitProcObj(*proc, CSGProc, NULL, NULL, CSGpc);
  CSGAdjustLevel(1);
  sym = CSSGet();
  if (sym != CSSlparen) CSSError("'(' expected");
  sym = CSSGet();
  if (sym != CSSrparen) {
    prochead->beg = FormalParameters(proc);
  }
  if (sym != CSSrparen) CSSError("')' expected");
  sym = CSSGet();
  if (strcmp(name, "main") == 0) CSGEntryPoint();

  return prochead;
}


static ASTNode ProcedureBody(CSGNode *proc)
{
  register int returnsize;
  register CSGNode curr;
  register ASTNode procbody, cur, new;

  procbody = NULL;
  tos = 0;
  while ((sym == CSSconst) || (sym == CSSstruct) || ((sym == CSSident) && (strcmp(CSSid, "long") == 0))) {
    new = NewASTNode(ASTprocedurebody);
    if (sym == CSSconst) {
      new->beg = ConstantDeclaration(proc);
    } else {
      new->beg = VariableDeclaration(proc);
    }
    if (procbody == NULL) {
      procbody = new;
      cur = new;
    } else {
      cur->end = new;
      cur = new;
    }
  }
  assert((*proc)->dsc == NULL);
  (*proc)->dsc = (*proc)->next;
  if (-tos > 32768) CSSError("maximum stack frame size of 32kB exceeded");
  CSGEnter(-tos);
  returnsize = 0;
  curr = (*proc)->dsc;
  while ((curr != NULL) && (curr->dsc == *proc)) {
    returnsize += 8;
    curr = curr->next;
  }
  if (procbody == NULL) {
    procbody = NewASTNode(ASTprocedurebody);
    cur = procbody;
  }
  new = NewASTNode(ASTprocedurebody);
  new->beg = StatementSequence();
  if (procbody == NULL) {
    procbody = new;
  } else {
    cur->end = new;
  }
  if (strcmp((*proc)->name, "main") == 0) {
    // CSGClose();
  } else {
    CSGReturn(returnsize);
  }
  CSGAdjustLevel(-1);

  return procbody;
}


static ASTNode ProcedureDeclaration(void)
{
  CSGNode proc;
  register ASTNode procdec;

  procdec = NewASTNode(ASTproceduredeclaration);
  assert(sym == CSSvoid);
  sym = CSSGet();
  procdec->beg = ProcedureHeading(&proc);
  if (sym != CSSlbrace) CSSError("'{' expected");
  sym = CSSGet();
  procdec->end = ProcedureBody(&proc);
  if (sym != CSSrbrace) CSSError("'}' expected");
  sym = CSSGet();
  proc->next = NULL;  // cut off rest of list

  return procdec;
}


static ASTNode Program(void)
{
  register ASTNode program, curr, new;

  program = NULL;
  CSGOpen();
  tos = 32768;
  instruct = 0;
  while ((sym != CSSvoid) && (sym != CSSeof)) {
    new = NewASTNode(ASTprogram);
    if (sym == CSSconst) {
      new->beg = ConstantDeclaration(&globscope);
    } else {
      new->beg = VariableDeclaration(&globscope);
    }
    if (program == NULL) {
      program = new;
      curr = new;
    } else {
      curr->end = new;
      curr = curr->end;
    }
  }
  CSGStart(32768 - tos);
  if (sym != CSSvoid) CSSError("procedure expected");
  while (sym == CSSvoid) {
    new = NewASTNode(ASTprogram);
    new->beg = ProcedureDeclaration();
    if (program == NULL) {
      program = new;
      curr = new;
    } else {
      curr->end = new;
      curr = curr->end;
    }
  }
  if (sym != CSSeof) CSSError("unrecognized characters at end of file");

  return program;
}


/*************************************************************************/


static void InsertObj(CSGNode *root, signed char class, CSGType type, CSSIdent name, long long val)
{
  register CSGNode curr;

  if (*root == NULL) {
    *root = malloc(sizeof(CSGNodeDesc));
    if (*root == NULL) CSSError("out of memory");
    curr = *root;
  } else {
    curr = *root;
    if (strcmp(curr->name, name) == 0) CSSError("duplicate symbol");
    while (curr->next != NULL) {
      curr = curr->next;
      if (strcmp(curr->name, name) == 0) CSSError("duplicate symbol");
    }
    curr->next = malloc(sizeof(CSGNodeDesc));
    assert(curr->next != NULL);
    curr = curr->next;
    if (curr == NULL) CSSError("out of memory");
  }
  curr->next = NULL;
  curr->class = class;
  curr->type = type;
  strcpy(curr->name, name);
  curr->val = val;
  curr->dsc = NULL;
  curr->lev = 0;
}


void ASTCompile(char *filename)
{
  ASTNode ast;

//  printf("compiling %s\n\n", filename);

  globscope = NULL;
  InsertObj(&globscope, CSGTyp, CSGlongType, "long", 8);
  InsertObj(&globscope, CSGSProc, NULL, "ReadLong", 1);
  InsertObj(&globscope, CSGSProc, NULL, "WriteLong", 2);
  InsertObj(&globscope, CSGSProc, NULL, "WriteLine", 3);

  CSSInit(filename);
  sym = CSSGet();
  ast = Program();

  EmitASTNodes(ast);
}
