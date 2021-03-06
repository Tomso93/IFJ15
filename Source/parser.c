/*
*	Projekt: Kompilator pro jazyk IFJ15=>podmnozina C++11
*	Autor: Tomas STRNKA & Martin WOJACZEK
*	Login: xstrnk01
*	Cast: Parser .c +.h
*	Verze: 1.4 
*/

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
#include "3ac.h"


int token; //Globalni promennna pro pradavani tokenu 
string attr; //retezec tokenu
TinstList *list;	//instrukcni paska
int error;



//--------------------------------------------------------------------------------//
//					PRECEDENCNI SYNTAKTICKY ANALYZATOR							  //
//--------------------------------------------------------------------------------//																				

char prec_table[][16]={
//+   -   *   /   <   >  <=  >=  ==  !=	 (	 )    ID  $   ST  IN  FL
{'>','>','<','<','>','>','>','>','>','>','<','>','<','>','<','<'},	// +
{'>','>','<','<','>','>','>','>','>','>','<','>','<','>','0','<'},	// -
{'>','>','>','>','>','>','>','>','>','>','<','>','<','>','0','<'},	// *
{'>','>','>','>','>','>','>','>','>','>','<','>','<','>','0','<'},	// /
{'<','<','<','<','>','>','>','>','>','>','<','>','<','>','<','<'},	//<
{'<','<','<','<','>','>','>','>','>','>','<','>','<','>','<','<'},	//>
{'<','<','<','<','>','>','>','>','>','>','<','>','<','>','<','<'},	//<=
{'<','<','<','<','>','>','>','>','>','>','<','>','<','>','<','<'},	//>=
{'<','<','<','<','>','>','>','>','>','>','<','>','<','>','<','<'},	//==
{'<','<','<','<','>','>','>','>','>','>','<','>','<','>','<','<'},	//!=
{'<','<','<','<','<','<','<','<','<','<','<','=','<','0','<','<'},	//(
{'>','>','>','>','>','>','>','>','>','>','0','>','0','>','0','0'},	//)
{'>','>','>','>','>','>','>','>','>','>','0','>','0','>','0','0'},	//id
{'<','<','<','<','<','<','<','<','<','<','<','0','<','0','<','<'},	//$
{'>','0','0','0','>','>','>','>','>','>','0','>','0','>','0','0'}, //string
{'>','>','>','>','>','>','>','>','>','>','0','>','0','>','0','0'}, // decimal / float


};
//--hleda--rade/sloupec, aby mohl potom rozhodnout---------------------------------
char select_ruler(string *zas_term, int term){			// prosim nesahat, hotove
	int radek, sloupec;
	
	//radek--jeste neni reseno, ze by bylo =<
	if (strCmpConstStr(zas_term, "*\0")==0  || strCmpConstStr(zas_term, "*<\0") == 0) radek=0;
	
	if (strCmpConstStr(zas_term, "/\0")==0  || strCmpConstStr(zas_term, "/<\0") == 0) radek=1;
	
	if (strCmpConstStr(zas_term, "+\0")==0  || strCmpConstStr(zas_term, "+<\0") == 0) radek=2;
	
	if (strCmpConstStr(zas_term, "-\0")==0  || strCmpConstStr(zas_term, "-<\0") == 0) radek=3;
	
	if (strCmpConstStr(zas_term, "<\0")==0  || strCmpConstStr(zas_term, "<<\0") == 0) radek=4;

	if (strCmpConstStr(zas_term, ">\0")==0  || strCmpConstStr(zas_term, "><\0") == 0) radek=5;

	if (strCmpConstStr(zas_term, "<=\0")==0 || strCmpConstStr(zas_term, "<=<\0") == 0) radek=6;

	if (strCmpConstStr(zas_term, ">=\0")==0 || strCmpConstStr(zas_term, ">=<\0") == 0) radek=7;

	if (strCmpConstStr(zas_term, "==\0")==0 || strCmpConstStr(zas_term, "==<\0") == 0) radek=8;

	if (strCmpConstStr(zas_term, "!=\0")==0 || strCmpConstStr(zas_term, "!=<\0") == 0) radek=9;

	if (strCmpConstStr(zas_term, "(\0")==0  || strCmpConstStr(zas_term, "(<\0") == 0) radek=10;

	if (strCmpConstStr(zas_term, ")\0")==0  || strCmpConstStr(zas_term, ")<\0") == 0) radek=11;

	if (strCmpConstStr(zas_term, "i\0")==0 || strCmpConstStr(zas_term, "i<\0") == 0) radek=12;

	if (strCmpConstStr(zas_term, "$\0")==0  || strCmpConstStr(zas_term, "$<\0") == 0) radek=13;

	if (strCmpConstStr(zas_term, "s\0") == 0 || strCmpConstStr(zas_term, "s<\0") == 0) radek = 14;

	if (strCmpConstStr(zas_term, "d\0") == 0 || strCmpConstStr(zas_term, "d<\0") == 0) radek = 15;

	if (strCmpConstStr(zas_term, "f\0") == 0 || strCmpConstStr(zas_term, "f<\0") == 0) radek = 15;

	//sloupec
	switch (term){
		case TOK_ADDITION:
			sloupec=0;
			break;
		case TOK_SUBTRACTION:
			sloupec=1;
			break;
		case TOK_MULTIPLICATION:
			sloupec=2;
			break;
		case TOK_DIVISION:
			sloupec=3;
			break;
		case TOK_LESS_THAN:
			sloupec=4;
			break;
		case TOK_GREATER_THAN:
			sloupec=5;
			break;
		case TOK_GREATER_THAN_OR_EQUAL:
			sloupec=6;
			break;
		case TOK_LESS_THAN_OR_EQUAL:
			sloupec=7;
			break;
		case TOK_COMPARISON: 
			sloupec=8;
			break;
		case TOK_INEQUALITY:
			sloupec=9;
			break;
		case TOK_LEFT_BRACKET:
			sloupec=10;
			break;
		case TOK_RIGHT_BRACKET:
			sloupec=11;
			break;
		case TOK_ID:
			sloupec=12;
			break;
		case TOK_STR:
			sloupec=14;
			break;
		case TOK_DECIMAL_NUMBER:
		case TOK_FLOATING_POINT_NUMBER:
			sloupec=15;
			break;
		default:
			sloupec=13;
			break;
	}

	return prec_table[radek][sloupec];
}


//----------------------------Smaze---zasobnik------------------------------
void SDipose(Tstack* St) {					// prosim nesahat, hotove

	while (St->top > 0) {
		strFree(&(St->pom[St->top]));
		St->top--;
	}
}

//----------------------------INIT-Zasobniku--------------------------------
void InitialSt(Tstack* St) {				// prosim nesahat, hotove
	string pom;
	strInit(&pom);
	strAddChar(&pom, '$');

	St->top = 0;				// vrchol == 0
	St->t_n[St->top] = 'T';		// je to terminal
	St->pom[St->top] = pom;		// je tam znak T
	St->val[St->top] = NULL;	// data o promenne v tabulce
		
}
//-------------------------------add-term-----------------------------------
void GnTerm(Tstack *St, char x){			// prosim nesahat, hotove

	strAddChar(&(St->pom[FindT(St)]), x);
} 
//--------------------------------------------------------------------------
void PushE(Tstack *St, string * data) {		// los nechutnost
	//dam pravidlo na zasobnik
	if (St->top != MAX) {
		string pom;
		strInit(&pom);
		strAddChar(&pom, 'E');

		// vlozi o vlozem informace
		St->top++;
		St->t_n[St->top] = 'N';
		St->pom[St->top] = pom;		// toto, zastav se u toho
		St->val[St->top] = data;	// toto

		// len bordel tu robi darebak, pryc s nim 
	}
}
//----------------------------hleda--prvni--hnadle--------------------------
int FindBrc(Tstack *St) {							// prosim nesahat, hotove
	// hleda prvni handle, jinak vraci syn. err
	int i = 0;
	char pom;
	if (St->pom[(St->top - i)].length >1)
		pom = St->pom[(St->top - i)].str[St->pom[(St->top - i)].length - 1];

	else pom = 0;



	while (pom != '<' && i < St->top) {
		i++;

		if (St->pom[(St->top - i)].length >1)
			pom = St->pom[(St->top - i)].str[St->pom[(St->top - i)].length - 1];
		else pom = 0;
	}

	if (pom != '<' && i == St->top) return SYNTAX_ERROR;

	else return St->top - i;
}

//----------------------------Vloz--token--terminal-------------------------
int TPush(Tstack *St, globalTS *global_table, string *id) {
	//dam terminal na zasobnik
	if (St->top == MAX) return INTERN_ERROR;	//vnitrni chyba interperetu

	else {
		string pom;
		strInit(&pom);
		St->top++;
		St->t_n[St->top] = 'T';
		

		if (token == TOK_ID){
			// tady bych se mel podivat, jestli je uz tabulce symbolu 
			strAddChar(&pom, 'i');
			St->pom[St->top] = pom;	
			
			string *tmp;
			tmp = malloc(sizeof(string));
			strInit(tmp);//sd
			strCopyString(tmp,&attr);
			St->val[St->top] = tmp;
		}

		else if (token == TOK_DECIMAL_NUMBER) {

			strAddChar(&pom, 'd');
			St->pom[St->top] = pom;

			//tady to musime narvat do tabulky promenou, ktera obsahuje cele cislo a do val navrat odkaz na ten symbol promenne, co je v tabulce
			string *NewVar; //label, pro navrat
			NewVar = malloc(sizeof(string));
    		strInit(NewVar); //inicializace
    		GenNewVariable(NewVar);  // vygenerovani promenne
		GtableInsertVar(global_table, id , NewVar, TOK_INT);
    		//tGData* prom = GtableSearch(global_table, id);
    		//LtableInsert(prom->LTable, &NewVar, TOK_INT);    // vlozeni do lokalni tabulky symbolu
    		Tvalue value;
		value.i = atoi(attr.str);
		GtableInsertVarVal(global_table, id, NewVar, value);
    		//LtableInsertValue(prom->LTable, &NewVar, value);
    			
		
			St->val[St->top] = NewVar;
			//strFree(&NewVar);
		}

		else if (token == TOK_FLOATING_POINT_NUMBER) {
			
			strAddChar(&pom, 'f');
			St->pom[St->top] = pom;

			//tady to musime narvat do tabulky promenou, ktera obsahuje float cislo a do val navrat odkaz na ten symbol promenne, co je v tabulce
			string *NewVar; //label, pro navrat
			NewVar = malloc(sizeof(string));
			strInit(NewVar); //inicializace
			GenNewVariable(NewVar);  // vygenerovani promenne
			GtableInsertVar(global_table, id, NewVar, TOK_DOUBLE);		
			//tGData* prom = GtableSearch(global_table, id);
			//LtableInsert(prom->LTable, &NewVar, TOK_DOUBLE);    // vlozeni do lokalni tabulky symbolu
			Tvalue value;
			value.d = (double)atof(attr.str);
			GtableInsertVarVal(global_table, id, NewVar, value);
			//LtableInsertValue(prom->LTable, &NewVar, value);

			
			St->val[St->top] = NewVar;
			//strFree(&NewVar);
		}

		else if (token == TOK_STR) {


			strAddChar(&pom, 's');
			St->pom[St->top] = pom;

			//tady to musime narvat do tabulky promenou, ktera obsahuje retezec a do val navrat odkaz na ten symbol promenne, co je v tabulce
			string *NewVar; //label, pro navrat
			NewVar = malloc(sizeof(string));
			strInit(NewVar); //inicializace
			GenNewVariable(NewVar);  // vygenerovani promenne
			GtableInsertVar(global_table, id, NewVar, TOK_STRING);
			//tGData* prom = GtableSearch(global_table, id);
			//LtableInsert(prom->LTable, &NewVar, TOK_STRING);    // vlozeni do lokalni tabulky symbolu
			Tvalue value;
			strInit(&value.s);
			strCopyString(&value.s, &attr);
			//value.s = attr;
			GtableInsertVarVal(global_table, id, NewVar, value);
			//LtableInsertValue(prom->LTable, &NewVar, value);
			
			St->val[St->top] = NewVar;
			//strFree(&NewVar);

		}
		
		else if (token == TOK_ADDITION) {
			
			strAddChar(&pom, '+');
			St->pom[St->top] = pom; }
		
		else if (token == TOK_SUBTRACTION) {
		
			strAddChar(&pom, '-');
			St->pom[St->top] = pom; }

		else if (token == TOK_MULTIPLICATION) {

			strAddChar(&pom, '*');
			St->pom[St->top] = pom; }

		else if (token == TOK_DIVISION) {

			strAddChar(&pom, '/');
			St->pom[St->top] = pom; }
        
        else if (token == TOK_LESS_THAN) {
        
            strAddChar(&pom, '<');
			St->pom[St->top] = pom; }
        
        else if (token == TOK_GREATER_THAN) {
        
            strAddChar(&pom, '>');
			St->pom[St->top] = pom; }
            
        else if (token == TOK_LESS_THAN_OR_EQUAL) {
        
            strAddChar(&pom, '<');
	    strAddChar(&pom, '=');
			St->pom[St->top] = pom; }
            
        else if (token == TOK_GREATER_THAN_OR_EQUAL) {
        
            strAddChar(&pom, '>');
		strAddChar(&pom, '=');
			St->pom[St->top] = pom; }
            
        else if (token == TOK_COMPARISON) {
        
            strAddChar(&pom, '=');
		strAddChar(&pom, '=');
			St->pom[St->top] = pom; }
            
        else if (token == TOK_INEQUALITY) {
        
            strAddChar(&pom, '!');
		strAddChar(&pom, '=');
			St->pom[St->top] = pom; }
            
        else if (token == TOK_LEFT_BRACKET) {
        
            strAddChar(&pom, '(');
			St->pom[St->top] = pom; }
            
        else if (token == TOK_RIGHT_BRACKET) {
        
            strAddChar(&pom, ')');
			St->pom[St->top] = pom; }
            
		else {
			St->val[St->top] = NULL;
		}
	}

	return SUCCESS;
}
//--------------------Mazu---polozky--------------------------------------------
void DelI(Tstack* St, int n) {				//nejlepsi zpusob, jak mazat prvky, ktere redukuju
	int i = 0;

	while (i<n) {
		strFree(&(St->pom[St->top]));
		St->top--;
		i++;
	}
}

//--------------------hleda-prvni--term--v--zasobniku---------------------------
int FindT(Tstack* St) {					// taktez, radeji ruky do kapes
	int i = 0;
	while (St->t_n[(St->top - i)] != 'T' && i < St->top) {
		i++;
	}
	return St->top - i;
}
//----------------------------Vrchni---term---------------------------------
string TopT(Tstack* St) {				// prosim nesahat

	return St->pom[FindT(St)];
}
//------------Redugujeme--:D---------------------------------------------------
int SReduction_expr(Tstack* St, int i, globalTS *global_table, string *id) {
	// nejlepsi redukce aku svet nezazil
	// upravit potom pushE, tak aby si predavalo dal ukazatel na data
	string * value1;
	string * value3;
	if (error != ERR) {
		value1 = St->val[i];
		
		value3 = malloc(sizeof(string));
    		strInit(value3); //inicializace
    		GenNewVariable(value3);  // vygenerovani promenne
		GtableInsertVar(global_table, id, value3, TOK_AUTO);
    		//tGData* prom = GtableSearch(global_table, id);
    		//LtableInsert(prom->LTable, &value3, TOK_STRING);    // vlozeni do lokalni tabulky symbolu
  
	}
	//----------------------------- E-> id------------------------------------------	
	if ((strCmpConstStr(&(St->pom[i]), "i") == 0) || (strCmpConstStr(&(St->pom[i]), "d") == 0) || (strCmpConstStr(&(St->pom[i]), "f") == 0) || (strCmpConstStr(&(St->pom[i]), "s") == 0)) {

		if (error != ERR){
			//Generovani instrukce
			Tinst *instrukce = genInstr(IMOV, value1, NULL, value3);
			GtableInsertInstr(global_table, id, instrukce);
		}
			DelI(St, 1);

		PushE(St, value3);
		return SYNTAX_OK;
	}
	//------------------------------E-> (E)------------------------------------------	
	else if ((strCmpConstStr(&(St->pom[i]), "(") == 0) &&
		(strCmpConstStr(&(St->pom[i + 1]), "E") == 0) &&
		(strCmpConstStr(&(St->pom[i + 2]), ")") == 0)) {


		if (error != ERR) {
			value1 = St->val[i + 1];
			//Generovani instrukce
			Tinst *instrukce = genInstr(IMOV, value1, NULL, value3);
			GtableInsertInstr(global_table, id, instrukce);
		}

		DelI(St, 3);
		PushE(St, value3);
		return SYNTAX_OK;
	}
	//------------------------------E-> E*E------------------------------------------	
	else if ((strCmpConstStr(&(St->pom[i]), "E") == 0) &&
		(strCmpConstStr(&(St->pom[i + 1]), "*") == 0) &&
		(strCmpConstStr(&(St->pom[i + 2]), "E") == 0)) {


		if (error != ERR) {
			string *value2 = St->val[i + 2];

			//Generovani instrukce
			Tinst *instrukce = genInstr(IMUL, value1, value2, value3);
			GtableInsertInstr(global_table, id, instrukce);
		}


		DelI(St, 3);
		// 3 se ulozi do zasobniku a ceka 
		PushE(St, value3);
		return SYNTAX_OK;

	}

	//---------------------------E-> E/E---------------------------------------------	
	else if ((strCmpConstStr(&(St->pom[i + 1]), "/") == 0) &&
		(strCmpConstStr(&(St->pom[i + 2]), "E") == 0)) {

		// E-> E/E
		if (error != ERR) {
			
			string *value2 = St->val[i + 2];

			
			//Generovani instrukce
			Tinst *instrukce = genInstr(IDIV,value1, value2, value3);
			GtableInsertInstr(global_table, id ,instrukce);
		}


		DelI(St, 3);
		PushE(St, value3);
		return SYNTAX_OK;

	}

	//------------------------E-> E+E----------------------------------------------	
	else if ((strCmpConstStr(&(St->pom[i + 1]), "+") == 0) &&
		(strCmpConstStr(&(St->pom[i + 2]), "E") == 0)) {


		if (error != ERR) {

			string *value2 = St->val[i + 2];

			//Generovani instrukce
			Tinst *instrukce = genInstr(IADD, value1, value2, value3);
			GtableInsertInstr(global_table, id, instrukce);
		}


		DelI(St, 3);
		PushE(St, value3);
		return SYNTAX_OK;

	}

	//-------------------------------E-> E-E---------------------------------------	
	else if ((strCmpConstStr(&(St->pom[i + 1]), "-") == 0) &&
		(strCmpConstStr(&(St->pom[i + 2]), "E") == 0)) {


		if (error != ERR) {
			
			string *value2 = St->val[i + 2];

			//Generovani instrukce
			Tinst *instrukce = genInstr(ISUB,value1, value2, value3);
			GtableInsertInstr(global_table, id, instrukce);
		}


		DelI(St, 3);
		PushE(St, value3);
		return SYNTAX_OK;
	}


	//---------------------------E-> E>E--------------------------------------------	
	else if ((strCmpConstStr(&(St->pom[i + 1]), ">") == 0) &&
		(strCmpConstStr(&(St->pom[i + 2]), "E") == 0)) {


		if (error != ERR) {

			string *value2 = St->val[i + 2];

			//Generovani instrukce
			Tinst *instrukce = genInstr(IBIG, value1, value2, value3);
			GtableInsertInstr(global_table, id, instrukce);
		}


		DelI(St, 3);
		PushE(St, value3);
		return SYNTAX_OK;
	}

	//----------------------------E-> E>=E--------------------------------------------	
	else if ((strCmpConstStr(&(St->pom[i + 1]), ">=") == 0) &&
		(strCmpConstStr(&(St->pom[i + 2]), "E") == 0)) {


		if (error != ERR) {
			string *value2 = St->val[i + 2];

		
			//Generovani instrukce
			Tinst *instrukce = genInstr(IEQBG, value1, value2, value3);
			GtableInsertInstr(global_table, id, instrukce);

		}


		DelI(St, 3);
		PushE(St, value3);
		return SYNTAX_OK;
	}

	//-----------------------E-> E<E-------------------------------------------------	
	else if ((strCmpConstStr(&(St->pom[i + 1]), "<") == 0) &&
		(strCmpConstStr(&(St->pom[i + 2]), "E") == 0)) {


		if (error != ERR) {
			string *value2 = St->val[i + 2];

			//Generovani instrukce
			Tinst *instrukce = genInstr(ISMALL,value1, value2, value3);
			GtableInsertInstr(global_table, id, instrukce);
		}


		DelI(St, 3);
		PushE(St, value3);
		return SYNTAX_OK;
	}


	//------------------------E-> E<=E-----------------------------------------------	
	else if ((strCmpConstStr(&(St->pom[i + 1]), "<=") == 0) &&
		(strCmpConstStr(&(St->pom[i + 2]), "E") == 0)) {

		if (error != ERR) {
			
			string *value2 = St->val[i + 2];

			//Generovani instrukce
			Tinst *instrukce = genInstr(IEQSM, value1, value2, value3);
			GtableInsertInstr(global_table, id, instrukce);
		}

		DelI(St, 3);
		PushE(St, value3);
		return SYNTAX_OK;
	}


	//------------------------E-> E=E------------------------------------------------	
	else if ((strCmpConstStr(&(St->pom[i + 1]), "==") == 0) &&
		(strCmpConstStr(&(St->pom[i + 2]), "E") == 0)) {


		if (error != ERR) {
			string *value2 = St->val[i + 2];

			//Generovani instrukce
			Tinst *instrukce = genInstr(IEQUAL, value1,value2, value3);
			GtableInsertInstr(global_table, id, instrukce);
		}


		DelI(St, 3);
		PushE(St, value3);
		return SYNTAX_OK;
	}


	//--------------------------E-> E!=E---------------------------------------------	
	else if ((strCmpConstStr(&(St->pom[i + 1]), "!=") == 0) &&
		(strCmpConstStr(&(St->pom[i + 2]), "E") == 0)) {


		if (error != ERR) {
			
			string *value2 = St->val[i + 2];

			//Generovani instrukce
			Tinst *instrukce = genInstr(INOTEQ, value1, value2, value3);
			GtableInsertInstr(global_table, id, instrukce);
		}


		DelI(St, 3);
		PushE(St, value3);
		return SYNTAX_OK;
	}

	return SYNTAX_ERROR;

}
//-----------------------------------------------------------------------------------------
int comp_expr(globalTS *global_table, string *id) {

	char result;
	int i;
	int chba;
	int vyber;
	Tstack St;
	string pom;

	strInit(&pom);
	InitialSt(&St);
	pom = TopT(&St);

	if(token != TOK_LEFT_BRACE || token != TOK_SEMICOLON) {

		result = select_ruler(&pom, token);

		if (result == '<') vyber = TOK_LESS_THAN;
		else if (result == '>') vyber = TOK_GREATER_THAN;
		else if (result == '=') vyber = TOK_EQUALS;
		else vyber = OTHER;


		switch (vyber) {
		case TOK_LESS_THAN:
			GnTerm(&St, '<');
			TPush(&St, global_table, id);
			if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			break;

		case TOK_GREATER_THAN:
			if ((i = FindBrc(&St)) == SYNTAX_ERROR) {
				SDipose(&St);
				error = ERR;
				return SYNTAX_ERROR;
			}
			strDelLastChar(&(St.pom[i]));
			chba = (SReduction_expr(&St, i + 1, global_table, id));
			if (chba != SYNTAX_OK) {
				SDipose(&St);
				error = ERR;
				return result;
			}

			break;

		case TOK_EQUALS:
			chba = TPush(&St, global_table, id);
			if (chba != SUCCESS) return chba;
			if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			//token = getNextToken(&attr);
			break;

		case OTHER:
			SDipose(&St);
			error = ERR;
			return SYNTAX_ERROR;
			break;
		}

		
		pom = TopT(&St);	//prvni term.
	}	while (TRUE)   //(strCmpConstStr(&pom, "$\0") != 0) && (token != TOK_LEFT_BRACE || token != TOK_SEMICOLON))
	{

		result = select_ruler(&pom, token);

		if (result == '<') vyber = TOK_LESS_THAN;
		else if (result == '>') vyber = TOK_GREATER_THAN;
		else if (result == '=') vyber = TOK_EQUALS;
		else vyber = OTHER;


		switch (vyber) {
		case TOK_LESS_THAN:
			GnTerm(&St, '<');
			TPush(&St, global_table, id);
			if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			break;

		case TOK_GREATER_THAN:
			i = FindBrc(&St);			
			strDelLastChar(&(St.pom[i]));
			chba = (SReduction_expr(&St, i + 1, global_table, id));
			if (chba != SYNTAX_OK) {
				SDipose(&St);
				error = ERR;
				return result;
			}

			break;

		case TOK_EQUALS:
			chba = TPush(&St, global_table, id);
			if (chba != SUCCESS) return chba;
			if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			//token = getNextToken(&attr);
			break;

		case OTHER:
			SDipose(&St);
			error = ERR;
			return SYNTAX_ERROR;
			break;
		}

		
		pom = TopT(&St);	//prvni term.
		if(strCmpConstStr(&pom, "$\0") == 0)
			if((token == TOK_LEFT_BRACE) ||(token == TOK_SEMICOLON) || (token == TOK_RIGHT_BRACKET))		
				break;

		
		
				
	}


	strFree(&pom);
	SDipose(&St);
	return SYNTAX_OK;
}


//--------------------------------------------------------------------------------//
//								REKURZIVNI SESTUP								  //
//--------------------------------------------------------------------------------//

//-TERM->TOK_STR--||--TOK_ID--||--TOK_DECIMAL_NUMBER--||--TOK_FLOATING_POINT_NUMBER
int term(){
	
	switch (token) {
		case(TOK_STR) :
		case(TOK_ID) :
		case(TOK_DECIMAL_NUMBER) :
		case(TOK_FLOATING_POINT_NUMBER) :
			return SYNTAX_OK;
			break;
		default:
			return SYNTAX_ERROR;
			break;
	}
}
//-----------NONTERMINAL-TYPE---------------------------------------------------
int type(){
	
	switch(token){
		case(TOK_INT) :
		case(TOK_DOUBLE) :
		case(TOK_STRING) :
		case(TOK_AUTO) :
			return SYNTAX_OK;
			break;
		default:
			return SYNTAX_ERROR;
			break;
	}
}

//-----I_PROM->--=--EXPR--||--eps
int _i_prom(globalTS *global_table, string *id, string *backid){
	int result;

	switch(token){
		case TOK_SEMICOLON:
			return SYNTAX_OK;
			break;
		
		case TOK_EQUALS:
			if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			
			result= comp_expr(global_table, id);
			if(result !=SYNTAX_OK) return result;

			//
		string *LastVar = GtableLastDest(global_table, id);
    		string *tmp;
		    tmp = malloc(sizeof(string));
		    strInit(tmp);
		    strCopyString(tmp,LastVar);
		    Tinst *instrukce = genInstr(IMOV, tmp, NULL, backid);
		    GtableInsertInstr(global_table, id, instrukce);
		       //

			//if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			if(token != TOK_SEMICOLON) return SYNTAX_ERROR;
			
			return SYNTAX_OK;
			break;

	}
	return SYNTAX_ERROR;
}

//-----LIST_PAR_N->--,--TERM--LIST_PAR_N--||--eps--------------------------------
int list_par_n(globalTS *global_table, string *id, string *funcid){
	int result;
	switch(token){
		case TOK_RIGHT_BRACKET:
			if((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			return SYNTAX_OK;
			break;
		
		case TOK_COMMA:
			if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			result= term();

				string *funcid;
				funcid = malloc(sizeof(string));
				strInit(funcid);
				strCopyString(funcid, id);

			switch(token){
			
		case TOK_ID:
			result= term();
			///
				string *tmp;
				tmp = malloc(sizeof(string));
				strInit(tmp);
				strCopyString(tmp, &attr);
			 	Tinst *instrukce = genInstr(IPAR, funcid, tmp, NULL);
			    	GtableInsertInstr(global_table, id, instrukce);
			///
			if(result !=SYNTAX_OK) return result;

			if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			result= list_par_n(global_table, id, funcid);

			if (result !=SYNTAX_OK) return result;
			return SYNTAX_OK;
			break;

		case TOK_DECIMAL_NUMBER:
			result= term();
			///

				string *NewVar;
				NewVar = malloc(sizeof(string));
				strInit(NewVar);
				GenNewVariable(NewVar);
				GtableInsertVar(global_table, id, NewVar, TOK_INT);
				
				Tvalue v;
				v.i = atoi(attr.str);
				GtableInsertVarVal(global_table, id, NewVar, v);

			 	instrukce = genInstr(IPAR, funcid, NewVar, NULL);
			    	GtableInsertInstr(global_table, id, instrukce);
			///
			if(result !=SYNTAX_OK) return result;

			if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			result= list_par_n(global_table, id, funcid);

			if (result !=SYNTAX_OK) return result;
			return SYNTAX_OK;
			break;

		case TOK_FLOATING_POINT_NUMBER:
			result= term();
			///

				//string *NewVar;
				NewVar = malloc(sizeof(string));
				strInit(NewVar);
				GenNewVariable(NewVar);
				GtableInsertVar(global_table, id, NewVar, TOK_DOUBLE);
				
				//Tvalue v;
				v.d = (double)atof(attr.str);
				GtableInsertVarVal(global_table, id, NewVar, v);

			 	instrukce = genInstr(IPAR, funcid, NewVar, NULL);
			    	GtableInsertInstr(global_table, id, instrukce);
			///
			if(result !=SYNTAX_OK) return result;

			if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			result= list_par_n(global_table, id, funcid);

			if (result !=SYNTAX_OK) return result;
			return SYNTAX_OK;
			break;


		case TOK_STR:
			result= term();
			///

				//string *NewVar;
				NewVar = malloc(sizeof(string));
				strInit(NewVar);
				GenNewVariable(NewVar);
				GtableInsertVar(global_table, id, NewVar, TOK_STRING);
				
				//Tvalue v;
				strInit(&v.s);
				strCopyString(&v.s,&attr);
				GtableInsertVarVal(global_table, id, NewVar, v);

			 	instrukce = genInstr(IPAR, funcid, NewVar, NULL);
			    	GtableInsertInstr(global_table, id, instrukce);
			///
			if(result !=SYNTAX_OK) return result;

			if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			result= list_par_n(global_table, id, funcid);

			if (result !=SYNTAX_OK) return result;
			return SYNTAX_OK;
			break;


			}

			if(result !=SYNTAX_OK) return result;

			if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			return list_par_n(global_table, id, funcid);
			break;



	}
	return SYNTAX_ERROR;
}
//-----LIST_PAR->--TERM--LIST_PAR_N--||--eps-------------------------------------
int list_par(globalTS *global_table, string *id, string *funcid){
	int result;

	switch(token){
		case TOK_RIGHT_BRACKET:
			return SYNTAX_OK;
			break;

		case TOK_ID:
			result= term();
			///
				string *tmp;
				tmp = malloc(sizeof(string));
				strInit(tmp);
				strCopyString(tmp, &attr);
			 	Tinst *instrukce = genInstr(IPAR, funcid, tmp, NULL);
			    	GtableInsertInstr(global_table, id, instrukce);
			///
			if(result !=SYNTAX_OK) return result;

			if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			result= list_par_n(global_table, id, funcid);

			if (result !=SYNTAX_OK) return result;
			return SYNTAX_OK;
			break;

		case TOK_DECIMAL_NUMBER:
			result= term();
			///

				string *NewVar;
				NewVar = malloc(sizeof(string));
				strInit(NewVar);
				GenNewVariable(NewVar);
				GtableInsertVar(global_table, id, NewVar, TOK_INT);
				
				Tvalue v;
				v.i = atoi(attr.str);
				GtableInsertVarVal(global_table, id, NewVar, v);

			 	instrukce = genInstr(IPAR, funcid, NewVar, NULL);
			    	GtableInsertInstr(global_table, id, instrukce);
			///
			if(result !=SYNTAX_OK) return result;

			if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			result= list_par_n(global_table, id, funcid);

			if (result !=SYNTAX_OK) return result;
			return SYNTAX_OK;
			break;

		case TOK_FLOATING_POINT_NUMBER:
			result= term();
			///

				//string *NewVar;
				NewVar = malloc(sizeof(string));
				strInit(NewVar);
				GenNewVariable(NewVar);
				GtableInsertVar(global_table, id, NewVar, TOK_DOUBLE);
				
				//Tvalue v;
				v.d = (double)atof(attr.str);
				GtableInsertVarVal(global_table, id, NewVar, v);

			 	instrukce = genInstr(IPAR, funcid, NewVar, NULL);
			    	GtableInsertInstr(global_table, id, instrukce);
			///
			if(result !=SYNTAX_OK) return result;

			if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			result= list_par_n(global_table, id, funcid);

			if (result !=SYNTAX_OK) return result;
			return SYNTAX_OK;
			break;


		case TOK_STR:
			result= term();
			///
				//string *NewVar;
				NewVar = malloc(sizeof(string));
				strInit(NewVar);
				GenNewVariable(NewVar);
				GtableInsertVar(global_table, id, NewVar, TOK_STRING);
				
				//Tvalue v;
				strInit(&v.s);
				strCopyString(&v.s,&attr);
				GtableInsertVarVal(global_table, id, NewVar, v);

			 	instrukce = genInstr(IPAR, funcid, NewVar, NULL);
			    	GtableInsertInstr(global_table, id, instrukce);
			///
			if(result !=SYNTAX_OK) return result;

			if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			result= list_par_n(global_table, id, funcid);

			if (result !=SYNTAX_OK) return result;
			return SYNTAX_OK;
			break;

		default:
			return SYNTAX_ERROR;
			break;
	}
	return SYNTAX_ERROR;		
}
//-----CALLF_DEC->--id--(--LIST_PAR--)--||->EXPR---------------------------------
int callf_dec(globalTS *global_table, string *id, string *backid){
	int result;
	tGData *fnce;
	switch(token){
		case TOK_ID:
			fnce = GtableSearch(global_table, &attr);
			if(fnce != NULL) {

				////
				string *funcid;
				funcid = malloc(sizeof(string));
				strInit(funcid);
				strCopyString(funcid, &attr);
			 	Tinst *instrukce = genInstr(IPRECALL, funcid, NULL, NULL);
			    	GtableInsertInstr(global_table, id, instrukce);
				////
				if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
				if(token !=TOK_LEFT_BRACKET) return SYNTAX_ERROR;

				if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			
				result= list_par(global_table, id, funcid);
				////
				instrukce = genInstr(ICALL, funcid, NULL, NULL);
			    	GtableInsertInstr(global_table, id, instrukce);
				
				string *R;
				R = malloc(sizeof(string));
				strInit(R);
				strAddChar(R, '#');
				strAddChar(R, 'R');
				strAddChar(R, 'E');
				strAddChar(R, 'T');
				strAddChar(R, 'U');
				strAddChar(R, 'R');
				strAddChar(R, 'N');

				instrukce = genInstr(IMOV, backid, NULL, R);
			    	GtableInsertInstr(global_table, id, instrukce);
				////
				if(result !=SYNTAX_OK) return result; }
			else {
				result = comp_expr(global_table, id);

//
		string *LastVar = GtableLastDest(global_table, id);
    		string *tmp;
		    tmp = malloc(sizeof(string));
		    strInit(tmp);
		    strCopyString(tmp,LastVar);
		    Tinst *instrukce = genInstr(IMOV, tmp, NULL, backid);
		    GtableInsertInstr(global_table, id, instrukce);
//

				if(result !=SYNTAX_OK) return result; }
			return SYNTAX_OK;
			break;

		 default:
		 	result= comp_expr(global_table, id);
		 	
		 	////
		string *LastVar = GtableLastDest(global_table, id);
    		string *tmp;
		    tmp = malloc(sizeof(string));
		    strInit(tmp);
		    strCopyString(tmp,LastVar);
		    Tinst *instrukce = genInstr(IMOV, tmp, NULL, backid);
		    GtableInsertInstr(global_table, id, instrukce);
		    ////
		 	if(result !=SYNTAX_OK) return result;

		 	return SYNTAX_OK;
		 	break;
	}
	return SYNTAX_ERROR;
}

//-----PROM->--id--=--CALL_DEF--;||->TYPE--id--I_PROM--;-------------------------
int _prom(globalTS *global_table, string *id){
	int result;
	string *tmp;
			string *backid;
			backid = malloc(sizeof(string));
			strInit(backid);
			strCopyString(backid, &attr);
			
			int typ = token;

	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	
	switch(token){
		case TOK_EQUALS:


			if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			
			result= callf_dec(global_table, id, backid);
			if(result !=SYNTAX_OK)return result;
			
			//if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			if(token !=TOK_SEMICOLON) return SYNTAX_ERROR;
			
			return SYNTAX_OK;
			break;
		
		case TOK_ID:

					
    					tmp = malloc(sizeof(string));
    					strInit(tmp);
    					strCopyString(tmp,&attr);
    					GtableInsertVar(global_table, id, tmp, typ);
			

			if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			result= _i_prom(global_table, id, tmp);

			if(result !=SYNTAX_OK) return result;

			return SYNTAX_OK;
			break;
	}
	return SYNTAX_ERROR;
}

//-----RETURN->--return--EXPR--;------------------------------------------------
int _return(globalTS *global_table, string *id){
	int result;
	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;

	result= comp_expr(global_table, id);
	if (result !=SYNTAX_OK) return result;

	//if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	if(token != TOK_SEMICOLON) return SYNTAX_ERROR;
	
	//tGData* prom = GtableSearch(global_table, id);
	//string *LastVar = ReadNameVar(prom->LInstr); // funkce na cteni nazvu posledni instrukce
        string *LastVar = GtableLastDest(global_table, id);
	string *tmp;
	tmp = malloc(sizeof(string));
	strInit(tmp);
	strCopyString(tmp,LastVar);
	Tinst *instrukce = genInstr(IRET, tmp, NULL, NULL);
    	GtableInsertInstr(global_table, id, instrukce);
	
	//return je dobre zapsan, neni co resit
	return SYNTAX_OK;
}

//-----------FOR->for--(--TYPE--id--I_PROM--;--EXPR--;--id--=--EXPR)--BODY------
int _for(globalTS *global_table, string *id){
	int result; 
	
	
	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	if (token !=TOK_LEFT_BRACKET) return SYNTAX_ERROR;

	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	
	result= type();
	//ulozeni promenne
	if (result !=SYNTAX_OK) return SYNTAX_ERROR;

	int typ = token;

	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	if (token !=TOK_ID) return SYNTAX_ERROR;

	string *tmp1;
	tmp1 = malloc(sizeof(string));
	strInit(tmp1);
    	strCopyString(tmp1,&attr);
    	GtableInsertVar(global_table, id, tmp1, typ);

	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	result= _i_prom(global_table, id, tmp1);



	if (result !=SYNTAX_OK) return SYNTAX_ERROR;

    string *Label_1; //label, pro navrat
    Label_1 = malloc(sizeof(string));
    strInit(Label_1); //inicializace
    GenNewVariable(Label_1);  // vygenerovani promenne
    GtableInsertVar(global_table, id, Label_1, TOK_STRING);
   // tGData* prom = GtableSearch(global_table, id);
    //LtableInsert(prom->LTable, &Label_1, TOK_STRING);    // vlozeni do lokalni tabulky symbolu
    //LtableInsertValue(global_table->data->LTable, &Label_1, Label_1);
   
    // instrukce pro label
    Tinst *instrukce = genInstr(ILABEL, Label_1, NULL, NULL);
    GtableInsertInstr(global_table, id, instrukce);
    
	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	result= comp_expr(global_table, id);
	if (result !=SYNTAX_OK) return SYNTAX_ERROR;
    
    // najde promennou ve ktere je vyhodnocena podminka
    //prom = GtableSearch(global_table, id);
    //string *LastVar = ReadNameVar(prom->LInstr);
    string *LastVar = GtableLastDest(global_table, id);
    string *tmp;
    tmp = malloc(sizeof(string));
    strInit(tmp);
    strCopyString(tmp,LastVar);
    instrukce = genInstr(INOT, tmp, NULL, tmp);
    GtableInsertInstr(global_table, id, instrukce);
    
    string *Label_2; //label, pro navrat
    Label_2 = malloc(sizeof(string));
    strInit(Label_2); //inicializace
    GenNewVariable(Label_2);  // vygenerovani promenne
    GtableInsertVar(global_table, id, Label_2, TOK_STRING);
    //prom = GtableSearch(global_table, id);
    //LtableInsert(prom->LTable, &Label_2, TOK_STRING);
    //LtableInsertValue(global_table->data->LTable, &Label_2, Label_2);
    
    instrukce = genInstr(IIFGOTO, tmp, NULL, Label_2);
    GtableInsertInstr(global_table, id, instrukce);
     
	//if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	if (token !=TOK_SEMICOLON) return SYNTAX_ERROR;

	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	if (token !=TOK_ID) return SYNTAX_ERROR;
	//if(!(tableSearch(local_table, &attr))) return SEMANTIC_ERROR;

//ulozit id
    string *backid; //label, pro navrat
    backid = malloc(sizeof(string));
    strInit(backid); //inicializace
    strCopyString(backid, &attr);
//
	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	if (token !=TOK_EQUALS) return SYNTAX_ERROR;

	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	result= comp_expr(global_table, id);

//nacist posledni vysledek
	string *LVar = GtableLastDest(global_table, id);
       	string *temp;
       	temp = malloc(sizeof(string));
	strInit(temp);
	strCopyString(temp,LVar);
//

	if (result !=SYNTAX_OK) return result;

	//if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	//if (token !=TOK_RIGHT_BRACKET) return SYNTAX_ERROR;

	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	if (token !=TOK_LEFT_BRACE) return SYNTAX_ERROR;
	result= stmnt(global_table, id);

	if (result != SYNTAX_OK) return result;
//ulozit posledni vysledek do backid

    instrukce = genInstr(IMOV, temp, NULL, backid); // negace podminky
    GtableInsertInstr(global_table, id, instrukce);

//

    // instrukce skoku
    instrukce = genInstr(IGOTO, NULL, NULL, Label_1);
    GtableInsertInstr(global_table, id, instrukce);
    //instrukce label pro skonceni cyklu
    instrukce = genInstr(ILABEL, Label_2, NULL, NULL);
    GtableInsertInstr(global_table, id, instrukce);
    
	if(result !=SYNTAX_OK) return result;
    //strFree(&Label_1);
    //strFree(&Label_2);
	//cely for je v tom, ze je to opravdu for a dokonce spravne zapsany >:D
	return SYNTAX_OK;
}

//-----------TERM_N->--<<--TERM--TERM_N--||eps--------------------------------
int term_n(globalTS *global_table, string *id){
	int result;
	string *NewVar;
	string *tmp;
	Tvalue v;
	Tinst *instrukce;

	switch (token) {
		case TOK_SEMICOLON:
			return SYNTAX_OK;
			break;
		
		case TOK_DOUBLE_ARROW_LEFT:
			if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			
			result = term();
			if (result != SYNTAX_OK) return result;
			
			switch (token) {
			case TOK_ID:

				
				tmp = malloc(sizeof(string));
				strInit(tmp);
				strCopyString(tmp, &attr);
				instrukce = genInstr(IWRITE, NULL, NULL, tmp);
				GtableInsertInstr(global_table, id, instrukce);
				break;


				///////////////
			case TOK_DECIMAL_NUMBER:

				
				NewVar = malloc(sizeof(string));
				strInit(NewVar);
				GenNewVariable(NewVar);
				GtableInsertVar(global_table, id, NewVar, TOK_INT);

				
				v.i = atoi(attr.str);
				GtableInsertVarVal(global_table, id, NewVar, v);

				instrukce = genInstr(IWRITE, NULL, NULL, NewVar);
				GtableInsertInstr(global_table, id, instrukce);
				break;

				/////////////////////
			case TOK_FLOATING_POINT_NUMBER:

			
				NewVar = malloc(sizeof(string));
				strInit(NewVar);
				GenNewVariable(NewVar);
				GtableInsertVar(global_table, id, NewVar, TOK_DOUBLE);

				
				v.d = (double)atof(attr.str);
				GtableInsertVarVal(global_table, id, NewVar, v);

				instrukce = genInstr(IWRITE, NULL, NULL, NewVar);
				GtableInsertInstr(global_table, id, instrukce);
				break;

				////////////////////////
			case TOK_STR:

				
				NewVar = malloc(sizeof(string));
				strInit(NewVar);
				GenNewVariable(NewVar);
				GtableInsertVar(global_table, id, NewVar, TOK_STRING);

				
				strInit(&v.s);
				strCopyString(&v.s, &attr);
				GtableInsertVarVal(global_table, id, NewVar, v);

				instrukce = genInstr(IWRITE, NULL, NULL, NewVar);
				GtableInsertInstr(global_table, id, instrukce);
				break;
			}
			
			
			//mozna jich je jeste vic, radeji si ho zavolam znovu
			if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			return term_n(global_table, id);
			break;
	}
	return SYNTAX_ERROR;
}

//-----------COUT->--cout--<<--TERM--TERM_N--;--------------------------------
int _cout(globalTS *global_table, string *id){
	int result;
	string *NewVar;
	string *tmp;
	Tvalue v;
	Tinst *instrukce;

	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	if (token !=TOK_DOUBLE_ARROW_LEFT) return SYNTAX_ERROR;

	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	result = term();

	if(result !=SYNTAX_OK) return result;
	
	// instrukce pro zapis, pokud jich je vice generuji se v term_n()
	switch (token) {
		case TOK_ID:


			
			tmp = malloc(sizeof(string));
			strInit(tmp);
			strCopyString(tmp, &attr);
			instrukce = genInstr(IWRITE, NULL, NULL, tmp);
			GtableInsertInstr(global_table, id, instrukce);
			break;
		
		
		///////////////
		case TOK_DECIMAL_NUMBER:

			
			NewVar = malloc(sizeof(string));
			strInit(NewVar);
			GenNewVariable(NewVar);
			GtableInsertVar(global_table, id, NewVar, TOK_INT);

			
			v.i = atoi(attr.str);
			GtableInsertVarVal(global_table, id, NewVar, v);

			instrukce = genInstr(IWRITE, NULL, NULL, NewVar);
			GtableInsertInstr(global_table, id, instrukce);
			break;

		/////////////////////
		case TOK_FLOATING_POINT_NUMBER:
			

			
			NewVar = malloc(sizeof(string));
			strInit(NewVar);
			GenNewVariable(NewVar);
			GtableInsertVar(global_table, id, NewVar, TOK_DOUBLE);

			
			v.d = (double)atof(attr.str);
			GtableInsertVarVal(global_table, id, NewVar, v);

			instrukce = genInstr(IWRITE, NULL, NULL, NewVar);
			GtableInsertInstr(global_table, id, instrukce);
			break;

		////////////////////////
		case TOK_STR:

			
			NewVar = malloc(sizeof(string));
			strInit(NewVar);
			GenNewVariable(NewVar);
			GtableInsertVar(global_table, id, NewVar, TOK_STRING);

			
			strInit(&v.s);
			strCopyString(&v.s, &attr);
			GtableInsertVarVal(global_table, id, NewVar, v);

			instrukce = genInstr(IWRITE, NULL, NULL, NewVar);
			GtableInsertInstr(global_table, id, instrukce);
			break;
	}
	
	//zavolam scanner a jdu zjistit, jestli termu neni vic a jestli jsou ok
	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	result = term_n(global_table, id);

	if (result !=SYNTAX_OK) return result;
	
	return SYNTAX_OK;
}

//-----------ID_N->-->>--id--ID_N--||--eps------------------------------------
int _id_n(globalTS *global_table, string *id){

	switch (token){
		case TOK_SEMICOLON:
			return SYNTAX_OK;
			break;
		
		case TOK_DOUBLE_ARROW_RIGHT:
			if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			if (token !=TOK_ID) return SYNTAX_ERROR;
			//if(!(tableSearch(local_table, &attr))) return SEMANTIC_ERROR;
			
			// vygeneruju instrukci
			string *tmp;
			tmp = malloc(sizeof(string));
			strInit(tmp);
			strCopyString(tmp,&attr);
			Tinst *instrukce = genInstr(IREAD, tmp, NULL, NULL);
			GtableInsertInstr(global_table, id, instrukce);
			
			//a zavolam si ji znova
			if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			return _id_n(global_table, id);
			break;

	}
	return SYNTAX_ERROR;
}

//-----------CIN->cin-->>--id--ID_N--;
int _cin(globalTS *global_table, string *id){
	int result;
	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	if (token !=TOK_DOUBLE_ARROW_RIGHT) return SYNTAX_ERROR;

	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	if (token !=TOK_ID) return SYNTAX_ERROR;
	//if(!(tableSearch(local_table, &attr))) return SEMANTIC_ERROR;
	
	// generuji prvni instrukci, ostatni se generujou v _id_n()
	string *tmp;
	tmp = malloc(sizeof(string));
	strInit(tmp);
	strCopyString(tmp,&attr);
	Tinst *instrukce = genInstr(IREAD, tmp, NULL, NULL);
	GtableInsertInstr(global_table, id, instrukce);
	
	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	
	//podivu se, jestli tam neni vic identifikatoru
	result= _id_n(global_table, id);
	if(result !=SYNTAX_OK) return result;

	// cin je dobre po syn. strance
	return SYNTAX_OK;
}

//-----------IF->if--(--EXPR--)--BODY--else--BODY----------------------------
int _if(globalTS *global_table, string *id){
	int result;
	
	//nasleduje leva zavorka a v ni vyraz
	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	if (token !=TOK_LEFT_BRACKET) return SYNTAX_ERROR;

	//if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	result = comp_expr(global_table, id);

	if (result !=SYNTAX_OK) return result;
	//vyraz je ve v poradku uzavru ho

	//if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	//if (token != TOK_RIGHT_BRACKET) return SYNTAX_ERROR;

    //generovani pomocne promenne
   // tGData* prom = GtableSearch(global_table, id);
   //string *LastVar = ReadNameVar(prom->LInstr);  funkce na cteni nazvu posledni instrukce 
    string *LastVar = GtableLastDest(global_table, id);
       	string *tmp;
       	tmp = malloc(sizeof(string));
	strInit(tmp);
	strCopyString(tmp,LastVar);
    Tinst *instrukce = genInstr(INOT, tmp, NULL, tmp); // negace podminky
    GtableInsertInstr(global_table, id, instrukce);
    
    string *Label_1; //novy label, skok na vetev else
    Label_1 = malloc(sizeof(string));
    strInit(Label_1); //inicializace
    GenNewVariable(Label_1);  // vygenerovani promenne
    GtableInsertVar(global_table, id, Label_1, TOK_STRING);
    //prom = GtableSearch(global_table, id);
    //LtableInsert(prom->LTable, &Label_1, TOK_STRING);    // vlozeni do lokalni tabulky symbolu
    //LtableInsertValue(global_table->data->LTable, &Label_1, Label_1);
    
    //generovani skoku na ELSE vetev
    instrukce = genInstr(IIFGOTO, tmp, NULL, Label_1);
    GtableInsertInstr(global_table, id, instrukce);
	//telo pokud je v if pravda
	//if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	if (token !=TOK_LEFT_BRACE) return SYNTAX_ERROR;
	result= stmnt(global_table, id);

	if (result != SYNTAX_OK) return result;


    string *Label_2;  // label dva, skok az za else, podminka v IF byla pravda
    Label_2 = malloc(sizeof(string));
    strInit(Label_2);
    GenNewVariable(Label_2);
    GtableInsertVar(global_table, id, Label_2, TOK_STRING);
    //prom = GtableSearch(global_table, id);
    //LtableInsert(prom->LTable, &Label_2, TOK_STRING);
    //LtableInsertValue(global_table->data->LTable, &Label_2, Label_2);
    
    // skok za ELSE
    instrukce = genInstr(IGOTO, NULL, NULL, Label_2);
    GtableInsertInstr(global_table, id, instrukce);
    instrukce = genInstr(ILABEL, Label_1, NULL, NULL); // musime vlozit label za telo IFu
    GtableInsertInstr(global_table, id, instrukce);
    
	if(result !=SYNTAX_OK) return result;
	//vse ok, nasleduje else a za ni else

	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	if (token !=TOK_ELSE) return SYNTAX_ERROR;

	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	if (token !=TOK_LEFT_BRACE) return SYNTAX_ERROR;
	result= stmnt(global_table, id);

	if (result != SYNTAX_OK) return result;

    // instrukce pro label_2, sem se skoci jestlize podminka IF byla pravda
    instrukce = genInstr(ILABEL, Label_2, NULL, NULL);
    GtableInsertInstr(global_table, id, instrukce);
	if(result !=SYNTAX_OK) return result;
    //strFree(&Label_1);
    //strFree(&Label_2);
	// konstrukce if je v poradku muze opustit s pozitvni odpovedi
	return SYNTAX_OK;

}
//-----------STMNT->IF||BODY||FOR||CIN||COUT||RETURN||PROM-------------------
int stmnt(globalTS *global_table, string *id){
	int result;

	switch (token){
		case TOK_LEFT_BRACE:
			return body(global_table, id);
			break;
		
		case TOK_IF:
			result= _if(global_table, id);
			if (result != SYNTAX_OK) return result;
			if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			return stmnt(global_table, id);
			break;
		
		case TOK_FOR:
			result= _for(global_table, id);
			if (result != SYNTAX_OK) return result;
			if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			return stmnt(global_table, id);
			break;

		case TOK_CIN:
			result= _cin(global_table, id);
			if (result != SYNTAX_OK) return result;
			if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			return stmnt(global_table, id);
			break;

		case TOK_COUT:
			result= _cout(global_table, id);
			if (result != SYNTAX_OK) return result;
			// volam rekurzvine funkci
			if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			return stmnt(global_table, id);
			break;
		
		case TOK_RETURN:
			result= _return(global_table, id);
			if (result != SYNTAX_OK) return result;
			if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			return stmnt(global_table, id);
			break;

		//PROM
		case TOK_ID:
		case TOK_INT:
		case TOK_STRING:
		case TOK_DOUBLE:
		case TOK_AUTO:
			result= _prom(global_table, id);
			//
			if (result != SYNTAX_OK) return result;
			if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			return stmnt(global_table, id);
			break;
		
		//prazdny statement
		case TOK_RIGHT_BRACE:
			return SYNTAX_OK;
			break;
	}
	return SYNTAX_ERROR;
}
//-----------BODY->{STMNT}----------prazdny-statement-nebo-zaplneny----------
int body(globalTS *global_table, string *id){
	int result;
	// jsme v body a melo bz nasledovat prud } nebo eps
	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;

	result = stmnt(global_table, id);
	if (result !=SYNTAX_OK) return result;

	return SYNTAX_OK;

}
//-----------SELECT->BODY--||--;----bud-je-to-funkce-nebo-deklarace----------
int select(globalTS *global_table, string *id){
	int result;
	//ocekavam v token { nebo ;, podle toho poznam, zda se jedna o fce nebo dek.

	switch (token){
		case TOK_LEFT_BRACE:
			result=body(global_table, id);
			if (result !=SYNTAX_OK) return result;
			
			return SYNTAX_OK;
			break;
		case TOK_SEMICOLON:
			// jedna se o deklaraci fce
			return SYNTAX_OK;
			break;
	}
	return SYNTAX_ERROR;
}

//-----------PARAM_N->-,-TYPE--ID--PARAM_N-----------------------------------
int param_n(globalTS *global_table, string *id){
	int result;
	
	//pozadam o dalsi token a musi byt ) nebo ,
	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;

	switch (token){
		case TOK_RIGHT_BRACKET:
			return SYNTAX_OK;
			break;
		case TOK_COMMA:
			if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			
			//musi nasledovat TYPE a id
			result = type();
			if (result != SYNTAX_OK) return result;
			int InternalType = token;
			
			if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			if (token != TOK_ID) return SYNTAX_ERROR;
			string *idParam = &attr;
			
			GtableInsertParam(global_table, id, idParam, InternalType);
			// zavolam si zpet funkci, zda nema dalsi parametry
			return param_n(global_table, id);
			
			break;
	}
	return SYNTAX_ERROR;
}

//-----------PARAM->TYPE--id--PARAM_N----------------------------------------
int param(globalTS *global_table, string *id){
	int result;
	int InternalType;
	//pozdaval jsem o dalsi token a ocekavam () nebo type
	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	
	
	
	switch (token){
		case TOK_RIGHT_BRACKET:
			return SYNTAX_OK;
			break;
		case TOK_INT:
		case TOK_AUTO:
		case TOK_STRING:
		case TOK_DOUBLE:
			InternalType = token; // ulozeni typu parametru, pro pozdejsi vlozeni do TS
			//po Type nasleduje ID
			if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			if (token != TOK_ID) return SYNTAX_ERROR;
			// ulozeni identifikatoru pro parametr
			string *idParam = &attr;
			//vlozeni parametru do GTS
			GtableInsertParam(global_table, id, idParam, InternalType);
			//zjistim, zda neni v zavorce vice parametru
			result=param_n(global_table, id);
			if (result !=SYNTAX_OK) return result;

			return SYNTAX_OK;
			break;
	}
	return SYNTAX_ERROR;
}
//-----------FUNC_DCLR->TYPE--id--(--PARAM--)--SELECT---FUNC_DCLR------------
int func_dclr(globalTS *global_table){
	int result;
	
	
	
	result= type();
	if (result != SYNTAX_OK) return result;
///	
	int InternalType = token; 
	
	// dalsi token ID
	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	if (token != TOK_ID) return SYNTAX_ERROR;
	
		//musi byt, jinak nefunguje.
	string id1;
	strInit(&id1);
	strCopyString(&id1, &attr);
	string *id = &id1;
//	string *id = &attr;
	GtableInsert(global_table, id, InternalType);
///
	
	// po ID nasleduje (
	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	if (token != TOK_LEFT_BRACKET) return SYNTAX_ERROR;
	// TYPE ID(PARAM)
	result=param(global_table, id);
	
	if (result !=SYNTAX_OK) return result;
	// uvnitr zavorky je vse ok, frcim dal
	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	result=select(global_table, id);

	if (result !=SYNTAX_OK) return result;
	//!!!!!!! nevim, jestli muzu ukoncit uspesne, protoze pokud prijde pouze dekalarace
	// a nenasleduje zadna fuknce, neni to prece validni
	
	
	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	// mela bz prijit rekurze, ale nevim jestli bude fungovat
	
//GTablePrintInst (global_table, id);
//GtablePrintVarsAll (global_table);
	return SYNTAX_OK;

}

//------------PROGRAM->FUNC_DCLR---------------------------------------------
int program(globalTS *global_table){
	int result;
	result= func_dclr(global_table);
	if(result != SYNTAX_OK) return result;
	//prosel jsem cely program a scanner uz nema co davat
	while (token != TOK_END_OF_FILE) {
		//cyklim, dokud mi nedojde konec souboru
		
		func_dclr(global_table);
	}
	
	//generuji konec programu
	  string start;
  strInit(&start);
  strAddChar(&start, 'm');
  strAddChar(&start, 'a');
  strAddChar(&start, 'i');
  strAddChar(&start, 'n');
	Tinst *instrukce = genInstr(IEND,NULL,NULL,NULL);
	GtableInsertInstr(global_table, &start, instrukce);
	return SYNTAX_OK;
	
}
//------------------START-POINT--------------------------------------------------
int parse(globalTS *ST){

  int result;
  globalTS *global_table = ST;
  strInit(&attr);

	if((token = getNextToken(&attr))== LEX_ERROR) return LEX_ERROR;
	
	else
		result=program(global_table);	// volam prvni neterminal 
	
	strFree(&attr);
	return result;
}



