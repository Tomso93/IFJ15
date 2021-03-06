//SCANER - Lex. analyza
/** stavy - cisla od 1 do 100 */
#define STATE_START 1
#define STATE_SLASH 2 // lomitko - /
#define STATE_LINE_COMMENT 3
#define STATE_BLOCK_COMMENT 4
#define STATE_BLOCK_COMMENT_STAR 5
#define STATE_GREATER_THAN 6
#define STATE_ALPHANUM 7
#define STATE_EQUALS 8
#define STATE_DIGIT 9
#define STATE_FLOATING_POINT 10 // STAV = cislo s radovou teckou?
#define STATE_LESS_THAN 11
#define STATE_EXPONENT 12
#define STATE_MORE_IN_EXPONENT 13
#define STATE_MORE_IN_EXPONENT2 15
#define STATE_STRING 16
#define STATE_ESCAPE_SEQUENCE 17
#define STATE_HEXA_CHAR 18
#define STATE_HEXA_CHAR_SECOND 19
#define STATE_INEQUALITY 20


/** tokeny - cisla od 101 do n */
//klícová slova
#define TOK_INT 107         // keyword int
#define TOK_DOUBLE 108      // keyword double
#define TOK_ELSE 115    // key word else
#define TOK_IF 116      // key word if
#define TOK_RETURN 117  // key word return
#define TOK_STRING 118  // key word string
#define TOK_AUTO 119    // key word auto
#define TOK_CIN 120     // key word cin
#define TOK_FOR 121     // key word for
#define TOK_COUT 122   // key word count

//operátory
#define TOK_ADDITION 101        // +
#define TOK_SUBTRACTION 102     // -
#define TOK_MULTIPLICATION 103  // *
#define TOK_DIVISION 104        // /
#define TOK_GREATER_THAN 105            // >
#define TOK_GREATER_THAN_OR_EQUAL 106   // >=
#define TOK_LESS_THAN_OR_EQUAL 127 //<=
#define TOK_LESS_THAN 128 // <
#define TOK_COMPARISON 111  // ==
#define TOK_EQUALS 112      // =
#define TOK_INEQUALITY 135 // !=

//identifikátor
#define TOK_ID 109          // ID

//øetìzecová konstanta
#define TOK_STR 134

//reálná konstanta
//#define TOK_REAL_NUMBER 133 // realnumber

//celoèíselná konstanta
#define TOK_DECIMAL_NUMBER 113 // cele cislo (rozsah C-int)

//double
#define TOK_FLOATING_POINT_NUMBER 114 // necele cislo (0.1, 5.889, ...)

//jednotlivé znaky
#define TOK_LEFT_BRACKET 123    // (
#define TOK_RIGHT_BRACKET 124    // )
#define TOK_DOT 125 // .
#define TOK_COMMA 125 // ,
#define TOK_LEFT_BRACE 129 // {
#define TOK_RIGHT_BRACE 130 // }
#define TOK_SEMICOLON 110   // ;
#define TOK_DOUBLE_ARROW_RIGHT 131 // >>
#define TOK_DOUBLE_ARROW_LEFT 132 // <<

//spec. znak
#define TOK_END_OF_FILE 126 // EOF



/** ErrorCodes - cisla od  do n */
#define LEX_ERROR 1 // chyba lex. analyzy


//chyby programu

#define LEXICAL_ERROR	1  //lexikalni chyba
#define SYNTAX_ERROR	2  //syntakticka chyba
#define DEFINE_ERROR	3  //nedefinovana fce/prom, redefinace fce/prom
#define TYPE_ERROR	4  //typova nekompatibilita operace, spatny pocet param
#define CONVERT_ERROR	5  //chyba odvozovani datoveho typu
#define SEMANTIC_ERROR	6  //ostatni semanticke chyby
#define READ_NUM_ERROR	7  //chyba pri cteni cisla ze vstupu
#define UNINIT_ERROR	8  //prace s neinicializovanou prom
#define ZERO_DIV_ERROR	9  //deleni nuou
#define RUNTIME_ERROR	10 //ostatni behove chyby
#define INTERN_ERROR	99 //interni chyba interpretu
#define SUCCESS		0  //bez chyb


// typy instrukci - cisla 900 - 950

#define IMOV	 900	//:=
#define IADD	 901	//+
#define ISUB	 902	//-
#define IMUL	 903	//*
#define IDIV	 904	//div
#define IEQUAL	 905	//==
#define ISMALL	 906	//<
#define IBIG	 907	//>
#define IEQSM	 908	//<=
#define IEQBG	 909	//>=
#define INOTEQ	 910	//!=
#define INOT	 911	//not
//#define IAND	 912	//and -neimplementovano
//#define IOR	 913	//or  -neimplementovano
#define ILABEL	 914	//navesti
#define IGOTO	 915	//skok
#define IIFGOTO	 916	//podmineny skok
#define IWRITE	 917	//write
#define IREAD	 918	//read
#define IREADI	 919	//read int
#define IREADD	 920	//read double
#define IREADS	 921	//read string
#define IFIND	 922	//find - vestavena
#define ISORT	 923	//sort - vestavena
#define IEND	 924	//konec programu
#define ICAT	 925	//konkatenace retezcu (concat) - vestavena
#define ILENGTH	 926	//delka retezce (length) - vestavena
			//podretezec (substr) - vestavena
			//tyto dve instrukce musi nasledovat ihned za sebou
#define ISUBSTR1 927	//src1=vstupni string, src2=NULL, dest=vystupni string
#define ISUBSTR2 928    //src1=prvni int, src2=druhy int, dest=NULL
#define IPRECALL 929	//provede operace nezbytne pred volanim funkce
#define IPAR	 930	//ulozi hodnotu parametru
#define ICALL	 931	//zavola funkci
#define IRET	 932	//vrati se z funkce




#define STR_ERROR   1
#define STR_SUCCESS 0
#define TRUE -1

#define TABLE_SIZE 20

#define SYNTAX_OK 0
#define SEM_ERROR SEMANTIC_ERROR
// Vedlejsi potreby parseru
#define MAX_SIZE 20 
#define OTHER 871
#define ERR	  872

