

#include <stdbool.h>
#include <malloc.h>
#include "define.h"
#include "str.h"
#include "instlist.h"
#include "stable.h"

int hash(string *key){
  int delka = strGetLength(key);
  int vysledek = 0; 
  for (int i = 0; i < delka; i++){
    vysledek = vysledek + (int)((*key)[i]);
  }
  vysledek = vysledek % TABLE_SIZE;
  return vysledek;
}

//-----lokalni tabulka symbolu-------------------------------------------------

int LtableInit(localTS *T){
  for (int i = 0; i < TABLE_SIZE; i++){
    T[i] = NULL;
  }
  return SUCCESS;
}


int LtableFree (localTS *T){
//  int success;
  tLTableItem *act;
  for (int i = 0; i < TABLE_SIZE; i++){
    act = T[i];
    while (act != NULL){
      strFree(&(act->key));
      if (act->data.varType == TOK_STRING){
        strFree(&(act->data.varValue.s));
      }
      T[i] = act->nextItem;
      free(act);
      act = T[i];
    }
  }
  return SUCCESS;
}


int LtableInsert (localTS *T, string *key, int varType){
  int hashed = hash(key);
  tLTableItem *act;
  tLTableItem *newItem;
  newItem = malloc(sizeof(tLTableItem));
  if (newItem == NULL){
    return INTERN_ERROR;
  }
  
  strInit(&(newItem->key));
  strCopyString(&(newItem->key), key);
  newItem->data.varType = varType;
  newItem->isinit = false;
  newItem->nextItem = T[hashed];
  T[hashed] = newItem;
}


tLData *LtableSearch (localTS *T, string *key){
  int i;
  int hashed = hash(key);
  bool found = false;
  tLTableItem *act = T[hashed];
  while (found != true && act != NULL){
    if (i = strcmpString(key, &(act->key)) == 0){
      found = true;
    }else{
      act = act->nextItem;
    }
  }

  if (found){
    return &(act->data);
  }else{
    return NULL;
  }
}


int LtableInsertValue (localTS *T, string *key, Tvalue v){
  tLData *act;
  act = LtableSearch (T, key);
  if (act == NULL){
    return INTERN_ERROR;
  }
  switch(act->varType){

    case TOK_INT:
      act->varValue.i = v.i;
    break;

    case TOK_DOUBLE:
      act->varValue.d = v.d;
    break;

    case TOK_STRING:
      strInit(&(act->varValue.s));
      strCopyString(&(act->varValue.s), &(v.s));
    break;

    case TOK_AUTO:
      return CONVERT_ERROR;
    break;
  }
  act->isinit = true;
  return SUCCESS;
}


//-----globalni tabulka symbolu------------------------------------------------

int GtableInit (globalTS *T){
  for (int i = 0; i < TABLE_SIZE; i++){
    T[i] = NULL;
  }
  return SUCCESS;
}


int GtableFree (globalTS *T){
  tGTableItem *act;
  for (int i = 0; i < TABLE_SIZE; i++){
    act = T[i];
    while (act != NULL){
      strFree(&(act->key));

      for (int j = 0; j < TABLE_SIZE; j++){
        if (act->data.params[j] != NULL){
          strFree(act->data.params[j]);
        }
      } 
      LtableFree(act->data.LTable);
      ListDispose(act->data.LInstr);
      T[i] = act->nextItem;
      free(act);
      act = T[i];
    }
  }
  return SUCCESS;
}


int GtableInsert (globalTS *T, string *key, int funcType){
  localTS LTS;
  TinstList LIL;
  
  int success;
  int hashed = hash(key);
  tGTableItem *act;
  tGTableItem *newItem;
  newItem = malloc(sizeof(tGTableItem));
  if (newItem == NULL){
    return INTERN_ERROR;
  }
  success = LtableInit(&LTS);
  if (success != SUCCESS){
    return INTERN_ERROR;
  }
  success = ListInit(&LIL);
  if (success != SUCCESS){
    return INTERN_ERROR;
  }

  strInit(&(newItem->key));
  strCopyString(&(newItem->key), key);
  newItem->data.funcType = funcType;
  newItem->isdef = false;
  newItem->LTable = LTS;
  newItem->LInstr = LIL;
  for (int i = 0; i < TABLE_SIZE; i++){
    newItem->params[i] = NULL;
  } 
  newItem->nextItem = T[hashed];
  T[hashed] = newItem;
  return SUCCESS;
}


tGData* GtableSearch (globalTS *T, string *key){
  int i;
  int hashed = hash(key);
  bool found = false;
  tGTableItem *act = T[hashed];
  while (found != true && act != NULL){
    if (i = strcmpString(key, &(act->key)) == 0){
      found = true;
    }else{
      act = act->nextItem;
    }
  }

  if (found){
    return &(act->data);
  }else{
    return NULL;
  }
}


int GtableInsertVar (globalTS *T, string *funcID, string *varID, int varType){
  int success;
  tGData *act;
  act = gtableSearch (T, funcID);
  if (act == NULL){
    return INTERN_ERROR;
  }
  success = LtableInsert (act->LTable, varID, varType);
  act->isdef = true;
  return success;
}


int GtableInsertVarVal (globalTS *T, string *funcID, string *varID, Tvalue v){
  int success;
  tGData *act;
  act = gtableSearch (T, funcID);
  if (act == NULL){
    return INTERN_ERROR;
  }
  success = LtableInsertValue (act->LTable, varID, v);
  act->isdef = true;
  return success;
}


int GtableInsertParam (globalTS *T, string *funcID, string *parID, int parType){
  int success;
  int i;
  tGData *act;
  act = gtableSearch (T, funcID);
  if (act == NULL){
    return INTERN_ERROR;
  }
  success = LtableInsert (act->LTable, parID, parType);
  if (success != SUCCESS){
    return success;
  }
  i = 0;
  while (act->params[i] != NULL){
    i++;
  }
  act->params[i] = parID;
  act->isdef = true;
  return success;
}

int GtableInsertInstr (globalTS *T, string *funcID, Tinst instrukce){
  int success;
  tGData *act;
  act = gtableSearch (T, funcID);
  if (act == NULL){
    return INTERN_ERROR;
  }
  success = ListInsert(act->LInstr, instrukce);
  act->isdef = true;
  return success;
}
