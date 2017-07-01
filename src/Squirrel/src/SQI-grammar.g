/*
 * Squirrel project
 *
 * file ?	SQI-grammar.g
 * what	?	Squirrel grammar for the parser
 * who	?	jlv
 * when	?	06/18/99
 * last	?	03/29/01
 *
 *
 * (c) Kirilla 1999-2001
 */

// first we set the header to use in all grenerated files
// we need the ifndef in order to avoid having redefine trouble

#header <<
#ifndef sqiheader
#define sqiheader
#include "SQI-Obj.h"
#include "SQI-heap.h"
#include "SQI-exception.h"
#include "SQI-varmap.h"
#include "SQI-ufunc.h"
#include "SQI-interp.h"
#include "SQI-squirrel.h"
#include "SQI-builtin.h"
#include "SQI-clean.h"
#include <stack>
extern stack <char *> TheFilenames;
#endif
>>

#lexprefix <<

#include    "NestedTokenStream.h"
#include <stack>
char *mystrdup(const char *);
extern stack<char *> TheFilenames;

>>

#lexmember <<
    NestedTokenStream           *nestedTokenStream;
    const char *                filename;
    virtual _ANTLRTokenPtr      getToken();
>> 

<<
#include "DLGLexer.h"
#include "myToken.h"
#include "NestedTokenStream.h"
#include "ATokenBuffer.h"

char *mystrdup(const char *s) {
  int   l=strlen(s);
  char  *t=new char[l+1];
  strcpy(t,s);
  return t;
}

_ANTLRTokenPtr DLGLexer::getToken() {
  FileToken         *result;
  result=(FileToken *)DLGLexerBase::getToken();
  result->col=DLGLexerBase::begcol();
  result->dlgLexer=this;
  return result;
}
>>

#lexclass C_COMMENT
#token "[\n\r]" <<skip();newline();>>
#token "\*/" <<mode(START);skip();>>
#token "\*~[/]" <<skip();>>
#token "~[\*\n\r]+" <<skip();>>

#lexclass C_COMMENT2
#token "[\n\r]" <<mode(START);newline();skip();>>
#token "\*~[/]" <<skip();>>
#token "~[\*\n\r]+" <<skip();>>

#lexclass STRINGS
#token "[\t\b]" << more (); replstr("");>>
#token "[\n\r]" <<newline(); more(); replstr("");>>
#token "\^o" << replchar ((char) 147); more (); >>
#token "\\n" << replchar ((char) 0x0A); more (); >>
#token "\\t" << replchar ((char) 0x09); more (); >>
#token "\\v" << replchar ((char) 0x0B); more (); >>
#token "\\b" << replchar ((char) 0x08); more (); >>
#token "\\r" << replchar ((char) 0x0D); more (); >>
#token "\\f" << replchar ((char) 0x0C); more (); >>
#token "\\a" << replchar ((char) 0x07); more (); >>
#token "\\\\" << replchar ((char) 0x5C); more (); >>
#token "\\?" << replchar ((char) 0x3F); more (); >>
#token "\\'" << replchar ((char) 0x27); more (); >>
#token "\"" << replchar ((char) 0x22); more (); >>
#token "~[']" <<more();>>
#token STR "'" << mode(START); >>

/*
#lexclass WORDS
#token "\\n" << replchar ((char) 0x0A); more (); >>
#token "\\t" << replchar ((char) 0x09); more (); >>
#token "\\v" << replchar ((char) 0x0B); more (); >>
#token "\\b" << replchar ((char) 0x08); more (); >>
#token "\\r" << replchar ((char) 0x0D); more (); >>
#token "\\f" << replchar ((char) 0x0C); more (); >>
#token "\\a" << replchar ((char) 0x07); more (); >>
#token "\\\\" << replchar ((char) 0x5C); more (); >>
#token "\\?" << replchar ((char) 0x3F); more (); >>
#token "\\'" << replchar ((char) 0x27); more (); >>
#token "\"" << replchar ((char) 0x22); more (); >>
#token "\\ " << replchar (' '); advance();more (); >>
#token "~[\ \n\r]" <<more();>>
#token WORD "[\n\r\ ]+" <<mode(START);>>
*/

#lexclass LC_INCLUDE

#token  WS          "[\t \ ]+"      <<skip();>>
#token  NL          "\n"            <<skip();newline();set_endcol(0);>>
#token  Filename    "( [A-Z a-z 0-9 \. _]+ ) | ( \' ~[\n \t \r \']* \' )"
<<
    mode(START);
    char *p=_lextext;
    if (*p=='\'') p++;
    for (char *q=p; *q != '\0'; q++) if (*q=='\'') *q='\0';
    const char *    nestedFilename=mystrdup(p);
    FILE *          nestedFILE=fopen(nestedFilename,"r");
    if (nestedFILE == 0) {
      fprintf(stderr,"Can't open #include file \"%s\"\n",nestedFilename);
    } else {
      DLGFileInput  *dlgFileInput=new DLGFileInput(nestedFILE);
      DLGLexer      *dlgLexer=new DLGLexer(dlgFileInput,2000);
      dlgLexer->nestedTokenStream=nestedTokenStream;
      dlgLexer->setToken(token_to_fill);
      dlgLexer->filename=nestedFilename;
      TheFilenames.push(strdup(nestedFilename));
      nestedTokenStream->newTokenStream(dlgLexer);
    };
    nestedTokenStream->skip();
>>

#lexclass START

// the token ID is set starting from LBRAK is 2
// if adding one token before , it's need to modify the SQI-nexpr.h file

#token EOFT     "@" <<free(TheFilenames.top());TheFilenames.pop();>>
#token "/\*" 	<<mode(C_COMMENT);skip();>>
#token ";" 		<<mode(C_COMMENT2);skip();>>
#token "'" 		<<mode (STRINGS);more ();>>		// 2
#token COLON    ":"		// 3
#token LPAR     "\("		// 4
#token RPAR	"\)"		// 5
#token LBRACE	"\{"		// 6
#token RBRACE	"\}"		// 7
#token AWORD "\"([a-zA-Z0-9\.\_]|\\[tbnarv])+"     //<<mode(WORDS);more ();>>			// 8
#token LBRAK	"\["            // 9
#token RBRAK	"\]"            // 10
#token PLUS	"\+"            // 11
#token MINUS	"\-"            // 12
#token TIMES	"\*"            // 13
#token IDIV     "//"            // 25
#token DIV	"/"             // 14
#token MOD	"%"           // 15
#token EQUALS	"=" 		// 16
#token NOT_EQUALS "\<\>"        // 17
#token GT	"\>"            // 18
#token GTE	"\>="           // 19
#token LT	"\<"            // 20
#token LTE	"\<="           // 21
#token AND	"and"           // 22
#token OR	"or"            // 23
#token NOT  "not"           // 24
#token POW  "\*\*"
#token BAND "\&"
#token BIOR "\|"
#token BEOR "\^"
#token BLSH "\<\<"
#token BRSH "\>\>"
#token OBJECT "$"
#token MEMBER "\~"
#tokclass ADD_OP {PLUS MINUS}
#tokclass REL_OP {EQUALS NOT_EQUALS GT GTE LT LTE}
#tokclass BOOL_OP {AND OR}
#tokclass MULT_OP {TIMES IDIV DIV BAND BIOR}
#tokclass EXP_OP {POW}
#tokclass MOD_OP {MOD}
#token TAG			"tag"
#token GOTO     	"goto"
#token TO			"to"
#token END			"end"
#token RUN			"run"
#token RUNRESULT 	"runresult"
#token REPEAT		"repeat"
#token IF			"if"
#token IFELSE		"ifelse"
#token IFTRUE		"iftrue"
#token IFFALSE		"iffalse"
#token CATCH		"catch"
#token FOR			"for"
#token DOWHILE		"do.while"
#token WHILE		"while"
#token DOUNTIL		"do.until"
#token UNTIL		"until"
#token FOREACH  	"for.each"
#token SWITCH		"switch"
#token CASE			"case"
#token RANGE		"range"
#token OTHER		"other"
#token TTRUE		"true"
#token TFALSE		"false"
#token COMMENT2		"#!" 		<<advance();while((ch!='\n')&&(ch!=-1)) advance();>>
#token INCLUDE		"#include" <<mode(LC_INCLUDE);skip();>>	
#token SPACE    	"[\t\ ]+"	<<skip();>>
#token NEXTLINE		"\\\\\n"	<<skip();newline();>>
#token LF       	"[\n\r]" 	<<newline();>>
#token COMMA		","		
#token AID			"[a-zA-Z_][a-zA-Z0-9_\.\?\!]*"
#token EXA			"[0-9]x[0-9a-fA-F]+"
#token FLOAT		"([0-9]+.[0-9]* | [0-9]*.[0-9]+) {[eE]{[\-\+]}[0-9]+}"
#token INTEGER		"[0-9]+"
    
#tokclass RRC {LF}    
#tokclass RC {LF COMMA}
#tokclass COMMENT {COMMENT1 COMMENT2}
#tokclass WORD { AWORD TAG GOTO TO END RUN RUNRESULT REPEAT IF IFELSE IFTRUE IFFALSE CATCH FOR DOWHILE WHILE DOUNTIL UNTIL FOREACH SWITCH CASE RANGE OTHER}
#tokclass ID { AID TAG GOTO TO END RUN RUNRESULT REPEAT IF IFELSE IFTRUE IFFALSE CATCH FOR DOWHILE WHILE DOUNTIL UNTIL FOREACH SWITCH CASE RANGE OTHER}

// Error class

#errclass "\"variable reference\" or \"instruction block\"" { COLON LBRACE }

class SQIParser {
	<<
		public:
			virtual void syn(_ANTLRTokenPtr tok, ANTLRChar *egroup, SetWordType *eset,ANTLRTokenType etok, int k)   
			{
			    int line;				
			    line = LT(1)->getLine();
			    			
			    syntaxErrCount++;                                   /* MR11 */
			    fprintf(stderr, "%s [line %d] syntax error at \"%s\"",TheFilenames.top(),line, LT(1)->getText());
			    if ( !etok && !eset ) {fprintf(stderr, "\n"); return;}
			    if ( k==1 ) fprintf(stderr, " missing");
			    else
			    {
			    	fprintf(stderr, "; \"%s\" not", LT(1)->getText());
			        if ( set_deg(eset)>1 ) fprintf(stderr, " in");
			    }
			    if ( set_deg(eset)>0 ) 
			    	edecode(eset);
			    else fprintf(stderr, " %s", token_tbl[etok]);
			    if ( strlen(egroup) > 0 ) 
			    	fprintf(stderr, " in %s", egroup);
			    fprintf(stderr, "\n"); 	
			};
	>>

	// Build a block from the input
	interp[SQI_Interp *Interp] > [SQI_nBlock *node] 
		:	<<SQI_Object *elem;$node = new SQI_nBlock(Interp->Tree);bool error=false;>> 
			( instruction[Interp] > [elem]
			<<
				if(!error)
				{
					if(elem) 
						$node->Add2Inst(elem);
				}
			>> )*
		;
		exception
			default : <<
				*(Interp->Stream->err) << TheFilenames.top() << " [" << "line " << LT(1)->getLine() << "]" <<" syntax error " << LT(1)->getText() << '\0';
				Interp->Stream->Flush();
				error = true;
				if(elem) elem->Suicidal(true);
				if($node) $node->Suicidal(true);
				$node = NULL;
				exportSignal;
			>>

	// Parse an instruction line
	instruction[SQI_Interp *Interp] > [SQI_Object *node]
		:	control[Interp]>[$node]
		|	predicate[Interp]>[$node]
		|	funcdef[Interp] <<$node=NULL;>>       // return nothing
		|	COMMENT <<$node=NULL;>>
		|	RC <<$node=NULL;>>
		;
		exception 
			default : <<
				exportSignal;
			>>
			
	// Parse a predicate call
	predicate[SQI_Interp *Interp] > [SQI_Object *node]
		:   member[Interp]>[$node]
		|	w:AID <<
				SQI_Node *cmd;list<SQI_Object *> *alist;
				cmd = Interp->FindCall(w->getText());
			>> arguments[Interp]>[alist] (RC)* <<
					cmd->SetArgs(alist);
				$node = cmd;
			>>
		;

	// Parse a function definition
	funcdef[SQI_Interp *Interp]
		: 	<<list<SQI_Object *> *params=NULL;SQI_nBlock *body=NULL;SQI_Defunc *ufunc;>> 
			TO w:ID {lparam[Interp]>[params]} RC
				funcbody[Interp]>[body]
			END <<
				ufunc = new SQI_Defunc();
				ufunc->SetParams(params);
				ufunc->SetBody(body);
				Interp->UFuncMap->AddFunc(w->getText(),ufunc);
			>>
		;

	// Parse a list of parameters
	lparam[SQI_Interp *Interp] > [list<SQI_Object *> *params]
		:	<<SQI_Object *var;$params = new list<SQI_Object *>;>>
			( eparam[Interp]>[var] <<
			$params->push_back(var);
			var->AddRef();
			>> )+
		;

	// Parse a parameter
	eparam[SQI_Interp *Interp] > [SQI_Object *param]
		:	variable[Interp]>[$param]
		|	alist[Interp]>[$param] // the list must have 2 arguments , we need to check that and return an error of not
		;	

	// Parse the body of a function
	funcbody[SQI_Interp *Interp] > [SQI_nBlock *node] 
		:	<<SQI_Object *elem;$node = new SQI_nBlock(Interp->Tree);>> 
			( bodyinst[Interp] > [elem] <<
			if(elem) $node->Add2Inst(elem);>> )*
		;

	// Parse an instruction line in a body
	bodyinst[SQI_Interp *Interp] > [SQI_Object *node]
		:	predicate[Interp]>[$node]
		|	control[Interp]>[$node]
		|	RC <<$node=NULL;>>
		|	COMMENT <<$node=NULL;>>
		;

	// Parse a call to a predicate (in an expression for exemple)
	call[SQI_Interp *Interp] > [SQI_Object *node]
		:   member[Interp]>[$node]
		|	w:AID <<
			SQI_Node *cmd;list<SQI_Object *> *alist;
				cmd = Interp->FindCall(w->getText());
			>> arguments[Interp]>[alist] <<
				if(!alist->size())
					delete alist;
				else
					cmd->SetArgs(alist);
				$node = cmd;
			>>
		;

	// Parse the argument of a predicate call
	arguments[SQI_Interp *Interp] > [list<SQI_Object *> *alist]
		:	<<SQI_Object *elem = NULL; $alist = new list<SQI_Object *>;>>
			( argument[Interp]>[elem] <<$alist->push_back(elem);elem->AddRef();>> )*
		;

	// Parse a argument of a predicate call
	argument[SQI_Interp *Interp] > [SQI_Object *node]
		:	quotedword[Interp]>[$node]
		|	alist[Interp]>[$node]
		|	basic[Interp]>[$node]
		|	block[Interp]>[$node]
		;

/*
	// Parse a basic element
	basic[SQI_Interp *Interp] > [SQI_Object *node]
		:	LPAR ebasic[Interp]>[$node] RPAR
		|	ebasic[Interp]>[$node]
		;
*/

	// Parse a basic element
	basic[SQI_Interp *Interp] > [SQI_Object *node]
		:	mexpr[Interp]>[$node]
		|	expression[Interp]>[$node]
		;
		exception
			default : <<
					//printf("syntax error line %d\n",LT(1)->getLine());
					exportSignal;
				>>

	// Parse a counter for FOR
	counter[SQI_Interp *Interp] > [SQI_Object *node]
		:	variable[Interp]>[$node]
		|	alist[Interp]>[$node]
		|	expression[Interp]>[$node]
		;

	// Parse an expression
	expression[SQI_Interp *Interp] > [SQI_Object *node]	
		:	LPAR expression[Interp]>[$node] RPAR
		|	call[Interp]>[$node]
		;
		exception
			default : <<
					//printf("syntax error line %d\n",LT(1)->getLine());
					exportSignal;
				>>

	// Parse a list
	alist[SQI_Interp *Interp] > [SQI_List *node]	
		:	<<SQI_Object *elem;SQI_List *lst;>>
			LBRAK <<
			$node = new SQI_List(Interp->Eternal);>> 
			( (RRC)* elist[Interp]>[elem] (RRC)* <<
				if(IsNode(elem))
					$node->Complex();
				else
				if(IsList(elem))
				{
					lst = IsList(elem);
					if(lst->IsComplex())
						$node->Complex();	 	
				}	
				
				$node->Add2End(elem);>> 
			)* RBRAK
		;

	// Parse an element list
	elist[SQI_Interp *Interp] > [SQI_Object *node]
		:	quotedword[Interp]>[$node]
		|	keyword[Interp]>[$node]
		|	alist[Interp]>[$node]
		|	basic[Interp]>[$node]
		|	block[Interp]>[$node]
		|	LPAR call[Interp]>[$node] RPAR
		;

	// Parse a block of instruction
	block[SQI_Interp *Interp] > [SQI_Object *node]	
		:	<<SQI_Object *elem;>>
			LBRACE interp[Interp]>[$node] RBRACE
		;

	// Parse an expression simple element
	simple[SQI_Interp *Interp] > [SQI_Object *node]
		:	number[Interp]>[$node]
		|	variable[Interp]>[$node]
		|	quotedword[Interp]>[$node]
		|	keyword[Interp]>[$node]
		|	alist[Interp]>[$node]
		|	LPAR (mexpr[Interp]>[$node] | call[Interp]>[$node]) RPAR
		;

	// Parse an expression (not X)
	boolnegexpr[SQI_Interp *Interp] > [SQI_Object *node]
		:	<<bool fnot=false;>>
			( NOT <<fnot=!fnot;>>)* simple[Interp] > [$node] <<
			if(fnot)
		 		$node = new SQI_nExpr(Interp->Tree,SQI_EXPR_NOT,$node);
			>>
		;
		
	// Parse an expression (** mod)
	expexpr[SQI_Interp *Interp] > [SQI_Object *node]
	:	boolnegexpr[Interp]>[$node] ( <<
			SQI_Object *right;SQI_nExpr *left;>> 
			e:EXP_OP <<
			left = new SQI_nExpr(Interp->Tree,e->getType(),$node);/*printf("%d\n",e->getType());*/>> 
			boolnegexpr[Interp]>[right] <<
			left->Add2Args(right);$node = left;>>
			)*
		;

	// Parse an expression ( -/+ X)
	signexpr[SQI_Interp *Interp] > [SQI_Object *node]
		:	<<bool plus=true;bool minus=true;char t;>>
			(e:ADD_OP <<
			
			t = e->getType();
			if(t==38) plus = true;
			else
			if(t==39) minus = !minus;
			>>)* expexpr[Interp] > [$node] <<
			if(!(plus && minus)) // if the sign is minus
			{
				if($node->Is()==SQI_NUMBER)
				{
					SQI_Number *old = dynamic_cast<SQI_Number *>($node);
					$node = old->Neg();
					old->Suicidal(true);
				}	
				else
					$node = new SQI_nExpr(Interp->Tree,SQI_EXPR_NEG,$node);
			}
			>>
		;

	// Parse an expression (X *// Y)
	multexpr[SQI_Interp *Interp] > [SQI_Object *node]
		:	signexpr[Interp]>[$node] ( <<
			SQI_Object *right;SQI_nExpr *left;>> 
			e:MULT_OP <<
			left = new SQI_nExpr(Interp->Tree,e->getType(),$node);/*printf("%d\n",e->getType());*/>> 
			signexpr[Interp]>[right] <<
			left->Add2Args(right);$node = left;>>
			)*
		;
		
	// Parse an expression X mod Y
	modexpr[SQI_Interp *Interp] > [SQI_Object *node]
		:	multexpr[Interp]>[$node] ( <<
			SQI_Object *right;SQI_nExpr *left;>> 
			e:MOD_OP <<
			left = new SQI_nExpr(Interp->Tree,e->getType(),$node);>> 
			multexpr[Interp]>[right] <<
			left->Add2Args(right);$node = left;>>
			)*
		;		

	// Parse an expression X +/- Y
	addexpr[SQI_Interp *Interp] > [SQI_Object *node]
		:	modexpr[Interp]>[$node] ( <<
			SQI_Object *right;SQI_nExpr *left;>> 
			e:ADD_OP <<
			//printf("+ = %d\n",e->getType());
			left = new SQI_nExpr(Interp->Tree,e->getType(),$node);>> 
			modexpr[Interp]>[right] <<
			left->Add2Args(right);$node = left;>>
			)*
		;

	// Parse a real (?) expression (X <>/=/>/(...) Y)
	relexpr[SQI_Interp *Interp] > [SQI_Object *node]
		:	addexpr[Interp]>[$node] { <<
			SQI_Object *right;SQI_nExpr *left;>>
			 e:REL_OP <<
			left = new SQI_nExpr(Interp->Tree,e->getType(),$node);>> 
			addexpr[Interp]>[right] <<
			left->Add2Args(right);$node = left;>>
			}
		;

	// Parse a boolean or mathematical expression
	mexpr[SQI_Interp *Interp] > [SQI_Object *node]
		:	<<SQI_Object *left,*right;bool rflag = false;>>	
			relexpr[Interp]>[$node] ( <<
			SQI_Object *right;SQI_nExpr *left;>> 
			e:BOOL_OP <<
			left = new SQI_nExpr(Interp->Tree,e->getType(),$node);>> 
			relexpr[Interp]>[right] <<
			left->Add2Args(right);$node = left;>>
			)*
		;
		exception
			default : <<
					//printf("syntax error line %d\n",LT(1)->getLine());
					exportSignal;
				>>
		/*
		|	<<SQI_Object *left,*right;bool rflag = false;>>	
			LPAR relexpr[Interp]>[$node] ( <<
			SQI_Object *right;SQI_nExpr *left;>> 
			f:BOOL_OP <<
			left = Interp->Tree->Expression(f->getType(),$node);>> 
			relexpr[Interp]>[right] <<
			left->Add2Args(right);$node = left;>>
			)* RPAR
		;
		*/

	// Parse a reference to a variable
	variable[SQI_Interp *Interp] > [SQI_Object *node]
		:	COLON w:ID <<
			$node = new SQI_nVarref(Interp->Tree,new SQI_Keyword(Interp->Tree,w->getText()));
			>>
		;
	
	// Parse a reference to the member of an object stored in a variable	
	member[SQI_Interp *Interp] > [SQI_Object *node]
		: << SQI_nMember *mbr;>> 
			OBJECT w:ID MEMBER m:ID <<
			list<SQI_Object *> *alist;
			mbr = new SQI_nMember(Interp->Tree,new SQI_Keyword(Interp->Tree,w->getText()));
			mbr->SetMember(m->getText());
			>> arguments[Interp]>[alist] <<
				if(!alist->size())
					delete alist;
				else
					mbr->SetArgs(alist);
				$node = mbr;
			>>
		;
					
	// Parse a number
	number[SQI_Interp *Interp] > [SQI_Object *node]
		:	i:INTEGER	<<$node = new SQI_Number(Interp->Eternal,atoi(i->getText()));>>
		|	f:FLOAT	    <<$node = new SQI_Number(Interp->Eternal,atof(f->getText()));>>
		|	e:EXA		<<$node = new SQI_Number(Interp->Eternal,strtol(e->getText(),NULL,16));>>
		|	TTRUE		<<$node = Interp->True;Interp->True->AddRef();>>
		|	TFALSE		<<$node = Interp->False;Interp->False->AddRef();>> 
		;

	// Parse a quoted word
	quotedword[SQI_Interp *Interp] > [SQI_String *node]
		:	w:WORD <<
			char *str = CleanWord(w->getText());
			$node = new SQI_Keyword(Interp->Eternal,str);
			free(str);
			>>
		|	s:STR <<
			char *str = CleanString(s->getText());
			$node = new SQI_String(Interp->Eternal,str);
			free(str);
			>>
		;

	// Parse a keyword
	keyword[SQI_Interp *Interp] > [SQI_Object *node]
		:	k:WORD <<
			$node = new SQI_Keyword(Interp->Eternal,k->getText());>>
		;
		
	iblock[SQI_Interp *Interp] > [SQI_Object *node]
		:	variable[Interp]>[$node]
		|	block[Interp]>[$node]
		|	expression[Interp]>[$node]
		;
		exception
			default : <<
				*(Interp->Stream->err) << "missing variable reference or block definition\n" << '\0';
				Interp->Stream->Flush();
				if($node)
				{
					$node->Suicidal(true);
					$node = NULL;	
				}
				exportSignal;
			>>

	// Parse a control sequence
	control[SQI_Interp *Interp] > [SQI_nControl *node]
		:	<<SQI_Object *arg;>> 
			RUN iblock[Interp]>[arg] <<
			$node = new SQI_nControl(Interp->Tree,SQI_CONTROL_RUN);
			$node->Add2Args(arg);
			>>
		|	<<SQI_Object *arg;>> 
			RUNRESULT iblock[Interp]>[arg] <<
			$node = new SQI_nControl(Interp->Tree,SQI_CONTROL_RUNRESULT);
			$node->Add2Args(arg);
			>>
		|	<<SQI_Object *arg;SQI_Object *expr;>> 
			REPEAT basic[Interp]>[expr] iblock[Interp]>[arg] <<
			$node = new SQI_nControl(Interp->Tree,SQI_CONTROL_REPEAT);
			$node->Add2Args(expr);
			$node->Add2Args(arg);
			>>
		|	<<SQI_Object *expr;SQI_Object *arg1;SQI_Object *arg2=NULL;bool error=false;>> 
			IF c:basic[Interp]>[expr] b:iblock[Interp]>[arg1] {a:iblock[Interp]>[arg2]} <<
			if(!error)
			{
				$node = new SQI_nControl(Interp->Tree,SQI_CONTROL_IF);
				$node->Add2Args(expr);
				$node->Add2Args(arg1);
				if(arg2) $node->Add2Args(arg2);
			} 
			else
			{
				if($node)
				{
					$node->Suicidal(true);
					$node = NULL;
				}
				if(expr)
					expr->Suicidal(true);
				if(arg1)
					arg1->Suicidal(true);
				if(arg2)
					arg2->Suicidal(true);	
			}	
			>>
			exception[c]
				default : <<
					error = true;
					exportSignal;
				>>
			exception[b]
				default : <<
					error = true;
					exportSignal;
				>>
			exception[a]
				default : <<
					error = true;
					exportSignal;
				>>				
		|	<<SQI_Object *expr;SQI_Object *arg1;SQI_Object *arg2;>> 
			IFELSE basic[Interp]>[expr] iblock[Interp]>[arg1] iblock[Interp]>[arg2] <<
			$node = new SQI_nControl(Interp->Tree,SQI_CONTROL_IFELSE);
			$node->Add2Args(expr);
			$node->Add2Args(arg1);
			$node->Add2Args(arg2);
			>>
		|	<<SQI_Object *arg;>> 
			IFTRUE iblock[Interp]>[arg] <<
			$node = new SQI_nControl(Interp->Tree,SQI_CONTROL_IFTRUE);
			$node->Add2Args(arg);
			>>
		|	<<SQI_Object *arg;>> 
			IFFALSE iblock[Interp]>[arg] <<
			$node = new SQI_nControl(Interp->Tree,SQI_CONTROL_IFFALSE);
			$node->Add2Args(arg);
			>>
		|	<<SQI_Object *block,*err=NULL;SQI_Object *tag;>> 
			CATCH quotedword[Interp]>[tag] iblock[Interp]>[block] { iblock[Interp]>[err] } <<
			$node = new SQI_nControl(Interp->Tree,SQI_CONTROL_CATCH);
			$node->Add2Args(tag);
			$node->Add2Args(block);
			if(err)
				$node->Add2Args(err);
			>>
		|	<<SQI_Object *block;SQI_Object *loop;>> 
			FOR counter[Interp]>[loop] iblock[Interp]>[block] <<
			$node = new SQI_nControl(Interp->Tree,SQI_CONTROL_FOR);
			$node->Add2Args(loop);
			$node->Add2Args(block);
			>>
		|	<<SQI_Object *block;SQI_Object *expr;>> 
			DOWHILE iblock[Interp]>[block] basic[Interp]>[expr] <<
			$node = new SQI_nControl(Interp->Tree,SQI_CONTROL_DOWHILE);
			$node->Add2Args(block);
			$node->Add2Args(expr);
			>>
		|	<<SQI_Object *block;SQI_Object *expr;>>
			WHILE basic[Interp]>[expr] iblock[Interp]>[block] <<
			$node = new SQI_nControl(Interp->Tree,SQI_CONTROL_WHILE);
			$node->Add2Args(expr);
			$node->Add2Args(block);
			>>
		|	<<SQI_Object *block;SQI_Object *expr;>>
			DOUNTIL iblock[Interp]>[block] basic[Interp]>[expr] <<
			$node = new SQI_nControl(Interp->Tree,SQI_CONTROL_DOUNTIL);
			$node->Add2Args(block);
			$node->Add2Args(expr);
			>>	
		|	<<SQI_Object *block;SQI_Object *expr;>>
			UNTIL basic[Interp]>[expr] iblock[Interp]>[block] <<
	 		$node = new SQI_nControl(Interp->Tree,SQI_CONTROL_UNTIL);
			$node->Add2Args(expr);
			$node->Add2Args(block);
			>>
		|	<<SQI_Object *block;SQI_Object *var;SQI_Object *alist;>>
			FOREACH quotedword[Interp]>[var] counter[Interp]>[alist] iblock[Interp]>[block] <<
	 		$node = new SQI_nControl(Interp->Tree,SQI_CONTROL_FOREACH);
			$node->Add2Args(var);
			$node->Add2Args(alist);
			$node->Add2Args(block);
			>>
		|	<<SQI_Object *expr;SQI_nSwitch *sw;TSwitch *c;SQI_Object *obj;>>
			SWITCH basic[Interp]>[expr] "\{" (LF)*
			<<	
			sw = new SQI_nSwitch(Interp->Tree,expr);
			>>
			( 
				(
				<<c = new TSwitch;>>
				(CASE <<c->type = SWITCH_CASE;c->args= new list<SQI_Object *>;>> (basic[Interp]>[obj] 
				<<
					obj->Contained();
					obj->AddRef();
					c->args->push_back(obj);	
				>>
				)+ block[Interp]>[obj] <<c->block = (SQI_nBlock *)obj;>>) 
				| 
				(RANGE <<c->type = SWITCH_RANGE;c->args= new list<SQI_Object *>;>> 
				basic[Interp]>[obj] 
				<<
					obj->Contained();
					obj->AddRef();
					c->args->push_back(obj);	
				>> 
				basic[Interp]>[obj] 
				<<
					obj->Contained();
					obj->AddRef();
					c->args->push_back(obj);	
				>> 
				block[Interp]>[obj] 
				<<
				c->block = (SQI_nBlock *)obj;
				>>
				) 
				| 
				(OTHER <<c->type = SWITCH_OTHER;c->args=NULL;>> 
				block[Interp]>[obj] <<
				c->block = (SQI_nBlock *)obj;
				>>
				)
				)
				<<sw->AddCase(c);>>	
				(LF)*
			)* {(LF)+}
			<<
			$node = sw;
			>>
			"\}" {LF}
			
		;
		exception
			default : <<
				//printf("syntax error line %d\n",LT(1)->getLine());
				exportSignal;
			>>
}

