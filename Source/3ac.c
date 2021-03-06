/* ========================================================================== */
/*                                                                            */
/*   Filename.c                                                               */
/*   (c) 2012 Author                                                          */
/*                                                                            */
/*   Description                                                              */
/*                                                                            */
/* ========================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "define.h"
#include "str.h"
#include "errors.h"
#include "instlist.h"
#include "ial.h"
#include "lex.h"
#include "varframe.h"
#include "interpret.h"
#include "parser.h"

int cnt = 1; // counter pro poÄŤĂ­tĂˇnĂ­ hodnot generovanĂ˝ch promÄ›nnĂ˝ch

// Funkce na generovĂˇnĂ­ novĂ© instrukce, kterĂˇ je nĂˇslednÄ› vloĹľena do seznamu instrukcĂ­
Tinst *genInstr(int InstType, string *src1, string *src2, string *dest)
{
    Tinst *TItem;
    TItem = malloc(sizeof(Tinst));
    if (TItem == NULL){
      return NULL;
    }
    (*TItem).itype = InstType;
    (*TItem).src1 = src1;
    (*TItem).src2 = src2;
    (*TItem).dest = dest;
    return TItem;
}

// Fuknce na generovĂˇnĂ­ novĂ˝ch promÄ›nnĂ˝ch
void GenNewVariable(string *item)
{
    strClear(item);
    strAddChar(item, '#');
    int i;
    i = cnt;
    
    while(i != 0)
    {
        strAddChar(item, (char)(i % 10 + '0'));
        i = i / 10;
    }
    cnt++;                             
}

string *ReadNameVar(TinstList *list)
{
    Tinst *I = ListGetLastInst(list);
    
    return I->dest;
}


string *GtableLastDest(globalTS *T, string *id){
  if (T == NULL || id == NULL){
    return NULL;
  }

  tGData* prom;
  prom = GtableSearch(T, id);
  if(prom == NULL){
    return NULL;
  }
  if(prom->LInstr == NULL){
    return NULL;
  }
  if(prom->LInstr->last == NULL){
    return NULL;
  }
  if(prom->LInstr->last->inst.dest == NULL){
    return NULL;
  }
  
  return prom->LInstr->last->inst.dest;
}
