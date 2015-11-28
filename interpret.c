/*
 *	interpret.c
 *	zaverecna interpretace ridiciho programu
 *	Autor: Petr Zufan
 *	
 */

#include <stdio.h>
#include <string.h>
#include "str.h" //soubor od Martin Kvapil. funkce pro praci s retezci
#include "ial.h" //funkce find a sort a hashovaci tabulka
#include "stable.h" //tabulka symbolu
#include "instlist.h" //seznamem instrukci
#include "define.h" //definice konstant
#include "errors.h" 
#include "interpret.h"


int interpret (TinstList *LOI){
  int success = SUCCESS;
  Tinst *instr;  

  instr = ListGetInst(LOI);
  if (instr == NULL){
    printerror(INTERN_ERROR);
    return INTERN_ERROR;
  }

  while ( !(instr->itype == IEND) ){
    switch (instr->itype){

      case IMOV:
        success = move(instr->src1, instr->dest);
        break;

      case IADD:
        success = addition(instr->src1, instr->src2, instr->dest);
        break;

      case ISUB:
        success = substraction(instr->src1, instr->src2, instr->dest);
        break;

      case IMUL:
        success = multiplication(instr->src1, instr->src2, instr->dest);
        break;

      case IDIV:
        success = division(instr->src1, instr->src2, instr->dest);
        break;

      case IEQUAL:
        success = equal(instr->src1, instr->src2, instr->dest);
        break;

      case ISMALL:
        success = smaller(instr->src1, instr->src2, instr->dest);
        break;

      case IBIG:
        success = bigger(instr->src1, instr->src2, instr->dest);
        break;

      case IEQSM:
        success = equalsmaller(instr->src1, instr->src2, instr->dest);
        break;

      case IEQBG:
        success = equalbigger(instr->src1, instr->src2, instr->dest);
        break;

      case INOTEQ:
        success = notequal(instr->src1, instr->src2, instr->dest);
        break;

      case INOT:
        success = negation(instr->src1, instr->dest);
        break;
/*
      case IAND:
        success = konjunction(instr->src1, instr->src2, instr->dest);
        break;

      case IOR:
        success = disjunction(instr->src1, instr->src2, instr->dest);
        break;
*/
      case ILABEL:
        break;

      case IGOTO:
        success = jump(instr->dest, LOI);
        break;

      case IIFGOTO:
        success = jumpif(instr->src1, instr->dest, LOI);
        break;

      case IREADI:
        success = readint(instr->dest);
        break;

      case IREADD:  
        success = readdouble(instr->dest);
        break;

      case IREADS:  
        success = readstring(instr->dest);
        break;

      case IREAD: 
        success = read(instr->dest);
        break;

      case IWRITE:  
        success = write(instr->src1);
        break;

      case IFIND:
        success = findme(instr->src1, instr->src2, instr->dest);
        break;

      case ISORT:
        success = sortme(instr->src1, instr->dest);
        break;

      case ICAT:
        success = concatenate(instr->src1, instr->src2, instr->dest);
        break;

      case ILENGTH:
        success = lengthstring(instr->src1, instr->dest);
        break;

      case ISUBSTR:
        success = substring(instr->src1, instr->src2, instr->dest);
        break;

      default:
        success = INTERN_ERROR;
        break;
    }

    if (!success){
      ListDespose(LOI);
      printerror(success);
      return success;
    }
    ListSucc(LOI);
    instr = ListGetInst(LOI);
    if (instr == NULL){
      printerror(INTERN_ERROR);
      return INTERN_ERROR;
    }
  }
  ListDespose(LOI);
  printerror(success);
  return success;
}

//-------------------IMOV------------------------------------------------------

int move(tData *src1, tData *dest){

  if ( (src1->varType == TOK_INT) && (dest->varType == TOK_INT) ){
    dest->varValue.i = src1->varValue.i;
    return SUCCESS;
  }else if ( (src1->varType == TOK_DOUBLE) && (dest->varType == TOK_DOUBLE) ){
    dest->varValue.d = src1->varValue.d;
    return SUCCESS;
  }else if ( (src1->varType == TOK_DOUBLE) && (dest->varType == TOK_INT) ){
    dest->varValue.i = (int)src1->varValue.d;
    return SUCCESS;
  }else if ( (src1->varType == TOK_INT) && (dest->varType == TOK_DOUBLE) ){
    dest->varValue.d = (double)src1->varValue.i;
    return SUCCESS;
  }else if ( (src1->varType == TOK_STRING) && (dest->varType == TOK_STRING) ){
    if (strCopyString(&(dest->varValue.s), &(src1->varValue.s)) == STR_ERROR){
      return RUNTIME_ERROR;
    }else{
      return SUCCESS;
    }
  }else{
    return TYPE_ERROR;
  }
  return INTERN_ERROR;
}

//-------------------IADD------------------------------------------------------

int addition(tData *src1, tData *src2, tData *dest){

  if ( (src1->varType == TOK_INT) && (src2->varType == TOK_INT) ){
     if (dest->varType == TOK_INT){
       dest->varValue.i = src1->varValue.i + src2->varValue.i;
       return SUCCESS;
     }else if (dest->varType == TOK_DOUBLE){
       dest->varValue.d = (double)(src1->varValue.i + src2->varValue.i);
       return SUCCESS;
     }

  }else if ( (src1->varType == TOK_DOUBLE) && (src2->varType == TOK_INT) ){
     if (dest->varType == TOK_INT){
       dest->varValue.i = (int)(src1->varValue.d + (double)src2->varValue.i);
       return SUCCESS;
     }else if (dest->varType == TOK_DOUBLE){
       dest->varValue.d = (src1->varValue.d + (double)src2->varValue.i);
       return SUCCESS;
     }

  }else if ( (src1->varType == TOK_INT) && (src2->varType == TOK_DOUBLE) ){
     if (dest->varType == TOK_INT){
       dest->varValue.i = (int)((double)src1->varValue.i + src2->varValue.d);
       return SUCCESS;
     }else if (dest->varType == TOK_DOUBLE){
       dest->varValue.d = (double)src1->varValue.i + src2->varValue.d;
       return SUCCESS;
     }

  }else if ( (src1->varType == TOK_DOUBLE) && (src2->varType == TOK_DOUBLE) ){
     if (dest->varType == TOK_INT){
       dest->varValue.i = (int)(src1->varValue.d + src2->varValue.d);
       return SUCCESS;
     }else if (dest->varType == TOK_DOUBLE){
       dest->varValue.d = src1->varValue.d + src2->varValue.d;
       return SUCCESS;
     }
  }else{
    return TYPE_ERROR;
  }
  return INTERN_ERROR;

}

//-------------------ISUB------------------------------------------------------
int substraction(tData *src1, tData *src2, tData *dest){
  
  if ( (src1->varType == TOK_INT) && (src2->varType == TOK_INT) ){
     if (dest->varType == TOK_INT){
       dest->varValue.i = src1->varValue.i - src2->varValue.i;
       return SUCCESS;
     }else if (dest->varType == TOK_DOUBLE){
       dest->varValue.d = (double)(src1->varValue.i - src2->varValue.i);
       return SUCCESS;
     }

  }else if ( (src1->varType == TOK_DOUBLE) && (src2->varType == TOK_INT) ){
     if (dest->varType == TOK_INT){
       dest->varValue.i = (int)(src1->varValue.d - (double)src2->varValue.i);
       return SUCCESS;
     }else if (dest->varType == TOK_DOUBLE){
       dest->varValue.d = (src1->varValue.d - (double)src2->varValue.i);
       return SUCCESS;
     }

  }else if ( (src1->varType == TOK_INT) && (src2->varType == TOK_DOUBLE) ){
     if (dest->varType == TOK_INT){
       dest->varValue.i = (int)((double)src1->varValue.i - src2->varValue.d);
       return SUCCESS;
     }else if (dest->varType == TOK_DOUBLE){
       dest->varValue.d = (double)src1->varValue.i - src2->varValue.d;
       return SUCCESS;
     }

  }else if ( (src1->varType == TOK_DOUBLE) && (src2->varType == TOK_DOUBLE) ){
     if (dest->varType == TOK_INT){
       dest->varValue.i = (int)(src1->varValue.d - src2->varValue.d);
       return SUCCESS;
     }else if (dest->varType == TOK_DOUBLE){
       dest->varValue.d = src1->varValue.d - src2->varValue.d;
       return SUCCESS;
     }
  }else{
    return TYPE_ERROR;
  }
  return INTERN_ERROR;
}

//-------------------IMUL------------------------------------------------------
int multiplication(tData *src1, tData *src2, tData *dest){

  if ( (src1->varType == TOK_INT) && (src2->varType == TOK_INT) ){
     if (dest->varType == TOK_INT){
       dest->varValue.i = src1->varValue.i * src2->varValue.i;
       return SUCCESS;
     }else if (dest->varType == TOK_DOUBLE){
       dest->varValue.d = (double)(src1->varValue.i * src2->varValue.i);
       return SUCCESS;
     }

  }else if ( (src1->varType == TOK_DOUBLE) && (src2->varType == TOK_INT) ){
     if (dest->varType == TOK_INT){
       dest->varValue.i = (int)(src1->varValue.d * (double)src2->varValue.i);
       return SUCCESS;
     }else if (dest->varType == TOK_DOUBLE){
       dest->varValue.d = (src1->varValue.d * (double)src2->varValue.i);
       return SUCCESS;
     }

  }else if ( (src1->varType == TOK_INT) && (src2->varType == TOK_DOUBLE) ){
     if (dest->varType == TOK_INT){
       dest->varValue.i = (int)((double)src1->varValue.i * src2->varValue.d);
       return SUCCESS;
     }else if (dest->varType == TOK_DOUBLE){
       dest->varValue.d = (double)src1->varValue.i * src2->varValue.d;
       return SUCCESS;
     }

  }else if ( (src1->varType == TOK_DOUBLE) && (src2->varType == TOK_DOUBLE) ){
     if (dest->varType == TOK_INT){
       dest->varValue.i = (int)(src1->varValue.d * src2->varValue.d);
       return SUCCESS;
     }else if (dest->varType == TOK_DOUBLE){
       dest->varValue.d = src1->varValue.d * src2->varValue.d;
       return SUCCESS;
     }
  }else{
    return TYPE_ERROR;
  }
  return INTERN_ERROR;
}

//-------------------IDIV------------------------------------------------------
int division(tData *src1, tData *src2, tData *dest){
  if ( (src1->varType == TOK_INT) && (src2->varType == TOK_INT) ){
     if (src2->varValue.i == 0){
       return ZERO_DIV_ERROR;
     }else if (dest->varType == TOK_INT){
       dest->varValue.i = src1->varValue.i % src2->varValue.i;
       return SUCCESS;
     }else if (dest->varType == TOK_DOUBLE){
       dest->varValue.d = (double)(src1->varValue.i % src2->varValue.i);
       return SUCCESS;
     }

  }else if ( (src1->varType == TOK_DOUBLE) && (src2->varType == TOK_INT) ){
     if (src2->varValue.i == 0){
       return ZERO_DIV_ERROR;
     }else if (dest->varType == TOK_INT){
       dest->varValue.i = (int)(src1->varValue.d / (double)src2->varValue.i);
       return SUCCESS;
     }else if (dest->varType == TOK_DOUBLE){
       dest->varValue.d = (src1->varValue.d / (double)src2->varValue.i);
       return SUCCESS;
     }

  }else if ( (src1->varType == TOK_INT) && (src2->varType == TOK_DOUBLE) ){
     if (src2->varValue.d == 0.0){
       return ZERO_DIV_ERROR;
     }else if (dest->varType == TOK_INT){
       dest->varValue.i = (int)((double)src1->varValue.i / src2->varValue.d);
       return SUCCESS;
     }else if (dest->varType == TOK_DOUBLE){
       dest->varValue.d = (double)src1->varValue.i / src2->varValue.d;
       return SUCCESS;
     }

  }else if ( (src1->varType == TOK_DOUBLE) && (src2->varType == TOK_DOUBLE) ){
     if (src2->varValue.d == 0.0){
       return ZERO_DIV_ERROR;
     }else if (dest->varType == TOK_INT){
       dest->varValue.i = (int)(src1->varValue.d / src2->varValue.d);
       return SUCCESS;
     }else if (dest->varType == TOK_DOUBLE){
       dest->varValue.d = src1->varValue.d / src2->varValue.d;
       return SUCCESS;
     }
  }else{
    return TYPE_ERROR;
  }
  return INTERN_ERROR;
}

//-------------------IEQUAL----------------------------------------------------
int equal(tData *src1, tData *src2, tData *dest){

  if (dest->varType != TOK_INT){
    if (dest->varType == TOK_DOUBLE){
      dest->varType = TOK_INT;
    }else{
      return TYPE_ERROR;
    }
  }else if ( (src1->varType == TOK_INT) && (dest->varType == TOK_INT) ){
    dest->varValue.i = (int)(src1->varValue.i == src2->varValue.i);
    return SUCCESS;
  }else if ( (src1->varType == TOK_DOUBLE) && (dest->varType == TOK_DOUBLE) ){
    dest->varValue.i = (int)(src1->varValue.d == src2->varValue.d);
    return SUCCESS;
  }else if ( (src1->varType == TOK_DOUBLE) && (dest->varType == TOK_INT) ){
    dest->varValue.i = (int)(src1->varValue.d == (double)src2->varValue.i);
    return SUCCESS;
  }else if ( (src1->varType == TOK_INT) && (dest->varType == TOK_DOUBLE) ){
    dest->varValue.i = (int)((double)src1->varValue.i == src2->varValue.d);
    return SUCCESS;
  }else if ( (src1->varType == TOK_STRING) && (dest->varType == TOK_STRING) ){
  dest->varValue.i = (int)(strCmpString(&(src1->varValue.s), &(src2->varValue.s)) == 0);
    return SUCCESS;
  }else{
    return TYPE_ERROR;
  }
  return INTERN_ERROR;
}

//-------------------ISMALL----------------------------------------------------
int smaller(tData *src1, tData *src2, tData *dest){

  if (dest->varType != TOK_INT){
    if (dest->varType == TOK_DOUBLE){
      dest->varType = TOK_INT;
    }else{
      return TYPE_ERROR;
    }
  }else if ( (src1->varType == TOK_INT) && (dest->varType == TOK_INT) ){
    dest->varValue.i = (int)(src1->varValue.i < src2->varValue.i);
    return SUCCESS;
  }else if ( (src1->varType == TOK_DOUBLE) && (dest->varType == TOK_DOUBLE) ){
    dest->varValue.i = (int)(src1->varValue.d < src2->varValue.d);
    return SUCCESS;
  }else if ( (src1->varType == TOK_DOUBLE) && (dest->varType == TOK_INT) ){
    dest->varValue.i = (int)(src1->varValue.d < (double)src2->varValue.i);
    return SUCCESS;
  }else if ( (src1->varType == TOK_INT) && (dest->varType == TOK_DOUBLE) ){
    dest->varValue.i = (int)((double)src1->varValue.i < src2->varValue.d);
    return SUCCESS;
  }else if ( (src1->varType == TOK_STRING) && (dest->varType == TOK_STRING) ){
  dest->varValue.i = (int)(strCmpString(&(src1->varValue.s), &(src2->varValue.s)) < 0);
    return SUCCESS;
  }else{
    return TYPE_ERROR;
  }
  return INTERN_ERROR;
}

//-------------------IBIG------------------------------------------------------
int bigger(tData *src1, tData *src2, tData *dest){

  if (dest->varType != TOK_INT){
    if (dest->varType == TOK_DOUBLE){
      dest->varType = TOK_INT;
    }else{
      return TYPE_ERROR;
    }
  }else if ( (src1->varType == TOK_INT) && (dest->varType == TOK_INT) ){
    dest->varValue.i = (int)(src1->varValue.i > src2->varValue.i);
    return SUCCESS;
  }else if ( (src1->varType == TOK_DOUBLE) && (dest->varType == TOK_DOUBLE) ){
    dest->varValue.i = (int)(src1->varValue.d > src2->varValue.d);
    return SUCCESS;
  }else if ( (src1->varType == TOK_DOUBLE) && (dest->varType == TOK_INT) ){
    dest->varValue.i = (int)(src1->varValue.d > (double)src2->varValue.i);
    return SUCCESS;
  }else if ( (src1->varType == TOK_INT) && (dest->varType == TOK_DOUBLE) ){
    dest->varValue.i = (int)((double)src1->varValue.i > src2->varValue.d);
    return SUCCESS;
  }else if ( (src1->varType == TOK_STRING) && (dest->varType == TOK_STRING) ){
  dest->varValue.i = (int)(strCmpString(&(src1->varValue.s), &(src2->varValue.s)) > 0);
    return SUCCESS;
  }else{
    return TYPE_ERROR;
  }
  return INTERN_ERROR;
}

//-------------------IEQSM-----------------------------------------------------
int equalsmaller(tData *src1, tData *src2, tData *dest){

  if (dest->varType != TOK_INT){
    if (dest->varType == TOK_DOUBLE){
      dest->varType = TOK_INT;
    }else{
      return TYPE_ERROR;
    }
  }else if ( (src1->varType == TOK_INT) && (dest->varType == TOK_INT) ){
    dest->varValue.i = (int)(src1->varValue.i <= src2->varValue.i);
    return SUCCESS;
  }else if ( (src1->varType == TOK_DOUBLE) && (dest->varType == TOK_DOUBLE) ){
    dest->varValue.i = (int)(src1->varValue.d <= src2->varValue.d);
    return SUCCESS;
  }else if ( (src1->varType == TOK_DOUBLE) && (dest->varType == TOK_INT) ){
    dest->varValue.i = (int)(src1->varValue.d <= (double)src2->varValue.i);
    return SUCCESS;
  }else if ( (src1->varType == TOK_INT) && (dest->varType == TOK_DOUBLE) ){
    dest->varValue.i = (int)((double)src1->varValue.i <= src2->varValue.d);
    return SUCCESS;
  }else if ( (src1->varType == TOK_STRING) && (dest->varType == TOK_STRING) ){
  dest->varValue.i = (int)(strCmpString(&(src1->varValue.s), &(src2->varValue.s)) <= 0);
    return SUCCESS;
  }else{
    return TYPE_ERROR;
  }
  return INTERN_ERROR;
}

//-------------------IEQBG-----------------------------------------------------
int equalbigger(tData *src1, tData *src2, tData *dest){

  if (dest->varType != TOK_INT){
    if (dest->varType == TOK_DOUBLE){
      dest->varType = TOK_INT;
    }else{
      return TYPE_ERROR;
    }
  }else if ( (src1->varType == TOK_INT) && (dest->varType == TOK_INT) ){
    dest->varValue.i = (int)(src1->varValue.i >= src2->varValue.i);
    return SUCCESS;
  }else if ( (src1->varType == TOK_DOUBLE) && (dest->varType == TOK_DOUBLE) ){
    dest->varValue.i = (int)(src1->varValue.d >= src2->varValue.d);
    return SUCCESS;
  }else if ( (src1->varType == TOK_DOUBLE) && (dest->varType == TOK_INT) ){
    dest->varValue.i = (int)(src1->varValue.d >= (double)src2->varValue.i);
    return SUCCESS;
  }else if ( (src1->varType == TOK_INT) && (dest->varType == TOK_DOUBLE) ){
    dest->varValue.i = (int)((double)src1->varValue.i >= src2->varValue.d);
    return SUCCESS;
  }else if ( (src1->varType == TOK_STRING) && (dest->varType == TOK_STRING) ){
  dest->varValue.i = (int)(strCmpString(&(src1->varValue.s), &(src2->varValue.s)) >= 0);
    return SUCCESS;
  }else{
    return TYPE_ERROR;
  }
  return INTERN_ERROR;
}

//-------------------INOTEQ----------------------------------------------------
int notequal(tData *src1, tData *src2, tData *dest){

  if (dest->varType != TOK_INT){
    if (dest->varType == TOK_DOUBLE){
      dest->varType = TOK_INT;
    }else{
      return TYPE_ERROR;
    }
  }else if ( (src1->varType == TOK_INT) && (dest->varType == TOK_INT) ){
    dest->varValue.i = (int)(src1->varValue.i != src2->varValue.i);
    return SUCCESS;
  }else if ( (src1->varType == TOK_DOUBLE) && (dest->varType == TOK_DOUBLE) ){
    dest->varValue.i = (int)(src1->varValue.d != src2->varValue.d);
    return SUCCESS;
  }else if ( (src1->varType == TOK_DOUBLE) && (dest->varType == TOK_INT) ){
    dest->varValue.i = (int)(src1->varValue.d != (double)src2->varValue.i);
    return SUCCESS;
  }else if ( (src1->varType == TOK_INT) && (dest->varType == TOK_DOUBLE) ){
    dest->varValue.i = (int)((double)src1->varValue.i != src2->varValue.d);
    return SUCCESS;
  }else if ( (src1->varType == TOK_STRING) && (dest->varType == TOK_STRING) ){
  dest->varValue.i = (int)(strCmpString(&(src1->varValue.s), &(src2->varValue.s)) != 0);
    return SUCCESS;
  }else{
    return TYPE_ERROR;
  }
  return INTERN_ERROR;
}

//-------------------INOT------------------------------------------------------
int negation(tData *src1, tData *dest){

  if (dest->varType != TOK_INT){
    if (dest->varType == TOK_DOUBLE){
      dest->varType = TOK_INT;
    }else{
      return TYPE_ERROR;
    }
    
  }else if (src1->varType == TOK_INT){
      dest->varValue.i = (int)(src1->varValue.i == 0);
      return SUCCESS;
  }else if (src1->varType == TOK_DOUBLE){
      dest->varValue.i = (int)((int)src1->varValue.d == 0);
      return SUCCESS;
  }else{
      return TYPE_ERROR;
  }
  return INTERN_ERROR;
}

//-------------------IGOTO-----------------------------------------------------
int jump(tData *dest, TinstList *LOI){ 
  return ListGoto(LOI, dest);
}

//-------------------IIFGOTO---------------------------------------------------
int jumpif(tData *src1, tData *dest, TinstList *LOI){
  if (src1->varValue.i) {
    return ListGoto(LOI, dest);
  }else{
    return SUCCESS;
  }
  return INTERN_ERROR;
}

//-------------------IREADI----------------------------------------------------
int readint(tData *dest){ 
  if (dest->varType != TOK_INT){
    return TYPE_ERROR;
  }else if (scanf("%d", &(dest->varValue.i)) < 0){
    return READ_NUM_ERROR;
  }
  return SUCCESS;
}

//-------------------IREADD----------------------------------------------------
int readdouble(tData *dest){ 
  if (dest->varType != TOK_DOUBLE){
    return TYPE_ERROR;
  }else if (scanf("%lg", &(dest->varValue.d)) < 0){
    return READ_NUM_ERROR;
  }
  return SUCCESS;
}

//-------------------IREADS----------------------------------------------------
int readstring(tData *dest){
  char c;
 
  if (dest->varType != TOK_STRING){
    return TYPE_ERROR;
  }

  c=getchar();
  while ((c == '\n') && (c == '\t') && (c == '\\')){
    c=getchar();
  }

  while((c != '\n') && (c != '\t') && (c != '\\') && (c != EOF) && (c != ';')){
    if ( strAddChar(&(dest->varValue.s), c) == 1 ){
      return INTERN_ERROR;
    }
    c=getchar();
  }
  return SUCCESS;
}

//-------------------IREAD----------------------------------------------------
int read(tData *dest){ 
  if (dest->varType == TOK_INT){
    return readint(dest);
  }else if (dest->varType == TOK_INT){
    return readdouble(dest);
  }else if (dest->varType == TOK_STRING){
    return readstring(dest);
  }else{
    return INTERN_ERROR;
  }
}

//-------------------IWRITE----------------------------------------------------
int write(tData *dest){ 
  if (dest->varType == TOK_DOUBLE){
     if (printf("%lg", dest->varValue.d) < 0){
       return RUNTIME_ERROR;
     }else{
       return SUCCESS;
     }

  }else if (dest->varType == TOK_INT){
    if (printf("%d", dest->varValue.i) < 0){
       return RUNTIME_ERROR;
     }else{
       return SUCCESS;
     }

  }else if (dest->varType == TOK_STRING){
    if (printf("%s", dest->varValue.s.str) < 0){
       return RUNTIME_ERROR;
     }else{
       return SUCCESS;
     }
  }
  return INTERN_ERROR;
}

//-------------------ISORT-----------------------------------------------------
int sortme(tData *src1, tData *dest){
  string *sortedstr = NULL;
  strInit(sortedstr);

  if ( (dest->varType == TOK_STRING) && (src1->varType == TOK_STRING) ){
    sortedstr = sort(&(src1->varValue.s));
    strCopyString(&(dest->varValue.s), sortedstr);
    strFree(sortedstr);
    return SUCCESS;
  }else{
    strFree(sortedstr);
    return TYPE_ERROR;
  }
  strFree(sortedstr);
  return INTERN_ERROR;
}

//-------------------IFIND----------------------------------------------------
int findme(tData *src1, tData *src2, tData *dest){
  if ( (src1->varType == TOK_STRING) && (src2->varType == TOK_STRING) ){
    if (dest->varType == TOK_INT){
      dest->varValue.i = find(&(src1->varValue.s), &(src2->varValue.s));
      return SUCCESS;
    }else if (dest->varType == TOK_DOUBLE){
	      dest->varValue.d = (double)find(&(src1->varValue.s), &(src2->varValue.s));
      return SUCCESS;
    }else{
      return TYPE_ERROR;
    }
  }else{
    return TYPE_ERROR;
  }
  return INTERN_ERROR;
}

//-------------------ILENGTH-----------------------------------------------------
int lengthstring(tData *src1, tData *dest){
  if ( (dest->varType == TOK_INT) && (src1->varType == TOK_STRING) ){
    dest->varValue.i = strGetLength(&(src1->varValue.s));
    return SUCCESS;
  }else if ( (dest->varType == TOK_DOUBLE) && (src1->varType == TOK_STRING) ){
    dest->varValue.d = (double)strGetLength(&(src1->varValue.s));
    return SUCCESS;
  }else{
    return TYPE_ERROR;
  }
  return INTERN_ERROR;
}

//-------------------ICAT-----------------------------------------------------
int concatenate(tData *src1, tData *src2, tData *dest){
  if ( (src1->varType == TOK_STRING) && (src2->varType == TOK_STRING) && (dest->varType == TOK_STRING) ){
    dest->varValue.s = concat(src1->varValue.s, src2->varValue.s);
    return SUCCESS;
  }else{
    return TYPE_ERROR;
  }
  return INTERN_ERROR;
}

//-------------------ISUBSTR--------------------------------------------------
int substring(instr->src1, instr->src2, instr->dest){
  return INTERN_ERROR;
}
