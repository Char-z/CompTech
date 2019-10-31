 
%{
/**FileName:	java_scanner.lex
 * Author:	Wang,Z
 * Date:	2019.10.29
 */

#include <stdio.h>
#include <stdlib.h>

#define uchar unsigned char

#define MAX_LEN 50

#define KEY_OP_TABLE	\
	KEY(ABSTRACT,	"abstract")	\
	KEY(BOOLEAN,	"boolean")	\
	KEY(BREAK,	"break")	\
	KEY(BYTE,	"byte")	\
	KEY(CASE,	"case")	\
	KEY(CATCH,	"catch")	\
	KEY(CHAR,	"char")	\
	KEY(CLASS,	"class")	\
	KEY(CONTINUE,	"continue")	\
	KEY(DEFAULT,	"default")	\
	KEY(DO,	"do")	\
	KEY(DOUBLE,	"double")	\
	KEY(ELSE,	"else")	\
	KEY(ENUM,	"enum")	\
	KEY(EXTENDS,	"extends")	\
	KEY(FINAL,	"final")	\
	KEY(FINALLY,	"finally")	\
	KEY(FLOAT,	"float")	\
	KEY(FOR,	"for")	\
	KEY(IF,	"if")	\
	KEY(IMPLEMENTS,	"implements")	\
	KEY(IMPORT,	"import")	\
	KEY(INSTANCEOF,	"instanceof")	\
	KEY(INT,	"int")	\
	KEY(INTERFACE,	"interface")	\
	KEY(LONG,	"long")	\
	KEY(NATIVE,	"native")	\
	KEY(NEW,	"new")	\
	KEY(KNULL,	"null")	\
	KEY(PACKAGE,	"package")	\
	KEY(PRIVATE,	"private")	\
	KEY(PROTECTED,	"protected")	\
	KEY(PUBLIC,	"public")	\
	KEY(RETURN,	"return")	\
	KEY(SHORT,	"short")	\
	KEY(STATIC,	"static")	\
	KEY(STRICTFP,	"strictfp")	\
	KEY(SUPER,	"super")	\
	KEY(SWITCH,	"switch")	\
	KEY(SYNCHRONIZED,	"synchronized")	\
	KEY(THIS,	"this")	\
	KEY(THROW,	"throw")	\
	KEY(THROWS,	"throws")	\
	KEY(TRANSIENT,	"transient")	\
	KEY(TRY,	"try")	\
	KEY(VOID,	"void")	\
	KEY(WHILE,	"while")	\
	KEY(VOLATILE,	"volatile")	\
	OP(LE,	"<=")	\
	OP(GE,	">=")	\
	OP(NE,	"!=")	\
	OP(E,	"==")	\
	OP(ADD,	"+")	\
	OP(SUB,	"-")	\
	OP(MUL,	"*")	\
	OP(DIV,	"/")	\
	OP(L,	"<")	\
	OP(G,	">")	\
	OP(ASSIGN,	"=")	\
	OP(LPAR,	"(")	\
	OP(RPAR,	")")	\
	OP(COM,	",")	\
	OP(SEM,	";")	\
	OP(DOT,	".")
//end KEY_OP_TABLE

/* --------------------------MacroFunc--------------------------------- */

#define nop()		//nothing
#define __EXIT_WHEN__(condition, cod, description)	\
	if(condition) errExit(cod, description)

/* ---------------------------TypeDefine------------------------------- */

enum ErrorCode { IncompleteAnotetation, MemAlloc_Failure, OpenFile_Failure, TooLong, NullPointer, InvalidOperator, OtherError};

#define KEY(e,	s) JAVA_KEY_ ## e,
#define OP(e,	s) JAVA_OP_ ## e,
enum Token {
	KEY_OP_TABLE	//must be first
	JAVA_ID,
	JAVA_CONST_INT,
	JAVA_CONST_DOUBLE,
	JAVA_CONST_STR,
	JAVA_SIGN,
	JAVA_EOF,
	JAVA_TOKEN		//meaningless
};
#undef KEY
#undef OP
typedef enum Token	Token;

struct TokenBuffer{
	Token tok;
	union{
		int num_int;		//valid when tok is JAVA_CONST_INT
		double num_dbl;		//valid when tok is JAVA_CONST_DOUBLE
		uchar chs[MAX_LEN + 1];		//valid when tok is JAVA_CONST_STR, JAVA_ID or JAVA_keywords
		uchar sign;			//valid when tok is JAVA_SIGN
	}value;
	int chs_pos;
};

typedef enum ErrorCode	ErrorCode;
typedef struct TokenBuffer TokenBuffer;

/* ---------------------------- */
#define KEY(e,	s) //discard
#define OP(e,	s) s,
uchar *_opDict[] = { KEY_OP_TABLE 0};
#undef KEY
#undef OP

char* concateStrs(char *Res, char *str0, char *str1, char *str2);
void DispToken_Value(Token tok);

%}

num	[0-9]
alpha	[a-zA-Z_]
alpha_num	({num}|{alpha})
white_space	[ \t\n]*

identifier	{alpha}{alpha_num}*
un_int	{num}*
un_dbl	{num}+\.{num}*
string	\"[^\n"]+\"
bad_string  \"[^\n"]+\n

%%

{string}    return JAVA_CONST_STR;
{bad_string} fprintf(stderr, "bad_string\n"); exit(-1);
"//".*\n    /* skip line comment */
"/*"    {   /* skip block comment*/
        char ch0 = input();
        for(;;){
            if(ch0 == '*'){
                if((ch0=input()) == '/'){
                    break;
                }else{
                    if(ch0 == -1){
                        fprintf(stderr, "incomplete comment\n");
                        exit(EXIT_FAILURE);
                    }else
                        continue;
                }
            }else{
                ch0 = input();
                if(ch0 == -1){
                    fprintf(stderr, "incomplete comment\n");
                    exit(EXIT_FAILURE);
                }
            }
        }
        }

abstract	return JAVA_KEY_ABSTRACT;
boolean	return JAVA_KEY_BOOLEAN;
break	return JAVA_KEY_BREAK;
byte	return JAVA_KEY_BYTE;
case	return JAVA_KEY_CASE;
catch	return JAVA_KEY_CATCH;
char	return JAVA_KEY_CHAR;
class	return JAVA_KEY_CLASS;
continue	return JAVA_KEY_CONTINUE;
default	return JAVA_KEY_DEFAULT;
do	return JAVA_KEY_DO;
double	return JAVA_KEY_DOUBLE;
else	return JAVA_KEY_ELSE;
enum	return JAVA_KEY_ENUM;
extends	return JAVA_KEY_EXTENDS;
final	return JAVA_KEY_FINAL;
finally	return JAVA_KEY_FINALLY;
float	return JAVA_KEY_FLOAT;
for	return JAVA_KEY_FOR;
if	return JAVA_KEY_IF;
implements	return JAVA_KEY_IMPLEMENTS;
import	return JAVA_KEY_IMPORT;
instanceof	return JAVA_KEY_INSTANCEOF;
int	return JAVA_KEY_INT;
interface	return JAVA_KEY_INTERFACE;
long	return JAVA_KEY_LONG;
native	return JAVA_KEY_NATIVE;
new	return JAVA_KEY_NEW;
null	return JAVA_KEY_KNULL;
package	return JAVA_KEY_PACKAGE;
private	return JAVA_KEY_PRIVATE;
protected	return JAVA_KEY_PROTECTED;
public	return JAVA_KEY_PUBLIC;
return	return JAVA_KEY_RETURN;
short	return JAVA_KEY_SHORT;
static	return JAVA_KEY_STATIC;
strictfp	return JAVA_KEY_STRICTFP;
super	return JAVA_KEY_SUPER;
switch	return JAVA_KEY_SWITCH;
synchronized	return JAVA_KEY_SYNCHRONIZED;
this	return JAVA_KEY_THIS;
throw	return JAVA_KEY_THROW;
throws	return JAVA_KEY_THROWS;
transient	return JAVA_KEY_TRANSIENT;
try	return JAVA_KEY_TRY;
void	return JAVA_KEY_VOID;
while   return JAVA_KEY_WHILE;
volatile    return JAVA_KEY_VOLATILE;
"<="    return JAVA_OP_LE;
">="    return JAVA_OP_GE;
"!="    return JAVA_OP_NE;
"=="    return JAVA_OP_E;
"+"    return JAVA_OP_ADD;
"-"    return JAVA_OP_SUB;
"*"    return JAVA_OP_MUL;
"/"    return JAVA_OP_DIV;
"<"    return JAVA_OP_L;
">"    return JAVA_OP_G;
"="    return JAVA_OP_ASSIGN;
"("    return JAVA_OP_LPAR;
")"    return JAVA_OP_RPAR;
","    return JAVA_OP_COM;
";"    return JAVA_OP_SEM;
"."    return JAVA_OP_DOT;
{identifier}    return JAVA_ID;
{un_int}        return JAVA_CONST_INT;
{un_dbl}        return JAVA_CONST_DOUBLE;
{white_space}   /* do nothing */

.       return JAVA_SIGN;
<<EOF>> return JAVA_EOF;

%%

int main(int argc, char **argv )
{
    --argc; ++argv;
    char *filename;
    if(argc > 0){
        filename = argv[0];
    }else{
        filename = "test.java";
    }
    yyin = fopen(filename, "r");
    if(!yyin){
        printf("%s not exist, will scan from stdin.\n",filename);
        yyin = stdin;
    }
        
    Token tok = yylex();
    
    while(tok != JAVA_EOF)
    {
        DispToken_Value(tok);
        putchar('\n');
        tok = yylex();
    }
    DispToken_Value(tok);
    putchar('\n');
    
    return 0;
}
/* ------------Concatenation Strings-------- */
char* concateStrs(char *Res, char *str0, char *str1, char *str2)
{
	char *cur = Res;
	while(*str0){
		*cur++ = *str0++;
	}
	while(*str1){
		*cur++ = *str1++;
	}
	while(*str2){
		*cur++ = *str2++;
	}
	*cur = 0;
	return Res;
}
/* ------------Display for Debug------------ */
void DispToken_Value(Token tok)
{
	char *str = malloc(sizeof(char)*(MAX_LEN+3));
	int lenl = 15;
	int lenr = MAX_LEN;
	switch (tok)
	{
	case JAVA_ID:
		printf("<%*s,%*s>", lenl, "identifier", lenr, yytext);
		break;
	case JAVA_CONST_INT:
		printf("<%*s,%*d>", lenl, "const_int", lenr, (int)strtod(yytext, 0));
        break;
	case JAVA_CONST_DOUBLE:
		printf("<%*s,%*f>", lenl, "const_double", lenr, strtod(yytext, 0));
		break;
	case JAVA_CONST_STR:
		printf("<%*s,%*s>", lenl, "const_str", lenr, yytext);
		break;
	case JAVA_SIGN:
		printf("<%*s,%*s>", lenl, "signal", lenr, concateStrs(str, "'", yytext, "'"));
		break;
	case JAVA_EOF:
		printf("<%*s,%*d>", lenl, "EOF", lenr, -1);
		break;
	default:
		if(tok<=JAVA_KEY_VOLATILE){
			printf("<%*s,%*s>", lenl, "keyword", lenr, yytext);
		}else{
			printf("<%*s,%*s>", lenl, "operator", lenr,  concateStrs(str, "\"", _opDict[tok-JAVA_OP_LE], "\""));
		}
		break;
	}
	free(str);
}
