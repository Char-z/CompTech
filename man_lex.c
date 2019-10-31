#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

enum State{ s_begin, s_id_key, s_int, s_double, s_str, s_op_sign};

struct Position { int line; int column; };

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
typedef struct Position	Position;
typedef enum State	State;

/* ---------------------------Variable--------------------------------- */

/* visual only in Func myFgetc(FILE*):uchar and errExit(ErrorCode):void */
Position _pos	= (Position){ .line = 1, .column = 0};

#define KEY(e,	s) s,
#define OP(e,	s) //discard
uchar *_keyDict[] = { KEY_OP_TABLE  0};
#undef KEY
#undef OP
#define KEY(e,	s) //discard
#define OP(e,	s) s,
uchar *_opDict[] = { KEY_OP_TABLE 0};
#undef KEY
#undef OP

TokenBuffer _tokBuf	= { .value.chs = {}, .chs_pos = 0 };
uchar _chBuf[3]	= {};	//last(2),now(1),next(0)

/* visual only in Func getTok(FILE*):Token */
State _nowState	= s_begin;


/* ---------------------------- */

/* ---------------------------Functions-------------------------------- */

/* ---------------------------Declaration------------------------------ */

Token isIDorKey(void);
Token isOPorSign(void);
Token getTok(FILE *fsrc);

uchar getChar(FILE *src);
uchar myFgetc(FILE *src);
void errExit(ErrorCode cod, char *description);

void DispToken_Value(Token tok);

/* --------------------------Implements-------------------------------- */

int main(int argc, char **argv)
{
    FILE *fsrc = NULL;
    --argc; ++argv;
    char *filename;
    if(argc > 0){
        filename = argv[0];
    }else{
        filename = "test.java";
    }
    fsrc = fopen(filename, "r");
    if(!fsrc){
        printf("%s not exist, will scan from stdin.\n",filename);
        fsrc = stdin;
    }
	Token tok = getTok(fsrc);
	while(tok!=JAVA_EOF){
		DispToken_Value(tok);
		putchar('\n');
		tok = getTok(fsrc);
	}
	DispToken_Value(tok);
    putchar('\n');

	return 0;
}

/**	Func:	find string pointed by _tokBuf.value.chs in _keyDict,
 * 			if find the string, return corresponding value of enumeration type,
 * 			else return JAVA_ID:Token. 
 */
Token isIDorKey(void)
{
	register int i = 0;
	for(; _keyDict[i]; i++){
		if(strcmp(_keyDict[i], _tokBuf.value.chs) == 0)
		{
			return (Token)i;
		}
	}
	return JAVA_ID;
}
/**	Func:	find character stored at _tokBuf.value.sign in _opDict,
 * 			if find the character, return correspoing value of enumeration type,
 * 			else return JAVA_SIGN:Token. 
 */
Token isOPorSign(void)
{
	register int i = JAVA_OP_ADD-JAVA_OP_LE;
	for(; _opDict[i]; i++){
		if(_tokBuf.value.sign == _opDict[i][0])
		{
			return (Token)(JAVA_OP_LE + i);
		}
	}
	return JAVA_SIGN;
}

/* visual only in Func lex(void):void */
#define go(state) _nowState = state
#define fresh_chBuf(fsrc) {	\
	_chBuf[2] = _chBuf[1];	\
	_chBuf[1] = _chBuf[0];	\
	_chBuf[0] = getChar(fsrc);	\
}
#define COMPLETE(x)	//nothing
/**	Func:	get next Token from fsrc:FILE*
 * 
 */
Token getTok(FILE *fsrc)
{
    static int retry_times = 5;
	__EXIT_WHEN__(fsrc==NULL, NullPointer, "exit from getTok(FILE*):Token.");

	if(_pos.line==1 && _pos.column==0){
		//first invocation
		_tokBuf.value.chs[MAX_LEN] = 0;
		_chBuf[1] = getChar(fsrc);
		_chBuf[0] = getChar(fsrc);
	}
	
	bool isOK = false;
	_nowState = s_begin;
	_tokBuf.chs_pos = 0;
	do{
        if(feof(fsrc)){
            --retry_times;
            if(retry_times == 0)
                return (_tokBuf.tok = JAVA_EOF);
        }
		switch (_nowState)
		{
		case s_begin:
			if(_chBuf[1]=='"') go(s_str);
			else if(isdigit(_chBuf[1])) go(s_int);
			else if(isalpha(_chBuf[1]) || _chBuf[1]=='_') go(s_id_key);
			else if(ispunct(_chBuf[1])) go(s_op_sign);
			else{
				fresh_chBuf(fsrc); nop();
			}
			break;
		case s_id_key:
			__EXIT_WHEN__(_tokBuf.chs_pos == MAX_LEN, TooLong, NULL);
			_tokBuf.value.chs[_tokBuf.chs_pos++] = _chBuf[1];
			fresh_chBuf(fsrc);
			if(isalnum(_chBuf[1]) || _chBuf[1]=='_')
				nop();
			else COMPLETE(s_id_key){
				isOK = true;
				_tokBuf.value.chs[_tokBuf.chs_pos] = 0;
				_tokBuf.tok = isIDorKey();
			}
			break;
		case s_int:
			__EXIT_WHEN__(_tokBuf.chs_pos == MAX_LEN, TooLong, NULL);
			_tokBuf.value.chs[_tokBuf.chs_pos++] = _chBuf[1];
			fresh_chBuf(fsrc);
			if(isdigit(_chBuf[1]))
				nop();
			else if(_chBuf[1] == '.')
				go(s_double);
			else COMPLETE(s_int){
				isOK = true;
				_tokBuf.value.chs[_tokBuf.chs_pos] = 'x';	//terminate number with 'x':char
				_tokBuf.value.num_int = (int)strtod(_tokBuf.value.chs, 0);
				_tokBuf.tok = JAVA_CONST_INT;
			}
			break;
		case s_double:
			__EXIT_WHEN__(_tokBuf.chs_pos == MAX_LEN, TooLong, NULL);
			_tokBuf.value.chs[_tokBuf.chs_pos++] = _chBuf[1];
			fresh_chBuf(fsrc);
			__EXIT_WHEN__(_chBuf[1] == '.', OtherError, "invalid number:double");
			if(isdigit(_chBuf[1]))
				nop();
			else COMPLETE(s_double){
				isOK = true;
				_tokBuf.value.chs[_tokBuf.chs_pos] = 'x';	//terminate number with 'x':char
				_tokBuf.value.num_dbl = strtod(_tokBuf.value.chs, 0);
				_tokBuf.tok = JAVA_CONST_DOUBLE;
			}
			break;
		case s_str:
			if(_tokBuf.chs_pos == 0){
				fresh_chBuf(fsrc);	//discard first quote
				__EXIT_WHEN__(_chBuf[1] == '"', OtherError, "empty string");
			}
			__EXIT_WHEN__(_tokBuf.chs_pos == MAX_LEN, TooLong, NULL);
			_tokBuf.value.chs[_tokBuf.chs_pos++] = _chBuf[1];
			fresh_chBuf(fsrc);
			if(_chBuf[1] != '"')
				nop();
			else COMPLETE(s_str){
				isOK = true;
				_tokBuf.value.chs[_tokBuf.chs_pos] = 0;
				_tokBuf.tok = JAVA_CONST_STR;
				fresh_chBuf(fsrc);
			}
			break;
		case s_op_sign:
			COMPLETE(s_op_sign)
			isOK = true;
			if(_chBuf[0] == '='){
				switch (_chBuf[1])
				{
				case '<':
					_tokBuf.tok = JAVA_OP_LE;
					break;
				case '>':
					_tokBuf.tok = JAVA_OP_GE;
					break;
				case '!':
					_tokBuf.tok = JAVA_OP_NE;
					break;
				case '=':
					_tokBuf.tok = JAVA_OP_E;
					break;
				default:
					errExit(InvalidOperator, "unsupported operator.");
				}
				_chBuf[1] = getChar(fsrc);
				_chBuf[0] = getChar(fsrc);
			}
			else{
				_tokBuf.value.sign = _chBuf[1];
				_tokBuf.tok = isOPorSign();
				fresh_chBuf(fsrc);
			}
			break;
		default:
			errExit(OtherError, "exit from getTok(FILE*):Token.");
		}
	}while(!isOK);
	return _tokBuf.tok;
}

/* visual only in Func getChar(FILE*):char */
#define getCharBuf(src)	{	\
	LastChar = NowChar;		\
	NowChar = NextChar;		\
	NextChar = myFgetc(src);\
}
/**	Func:	get a uchar from a stream one by one, but regard anotations or some consistent blanks as a space uchar.
 * 	Param:	src:	a FILE-pointer to stream
 * 	Retun:	uchar:	return 'SPC' if there is an anotation,
 * 					also 'SPC' if the stream is at end-of-file when called.
 */
uchar getChar(FILE *src)
{
	static bool isFirst = true;
	static uchar LastChar = 0;
	static uchar NowChar = 0;
	static uchar NextChar = 0;
	if(isFirst){
		NowChar = myFgetc(src);
		NextChar = myFgetc(src);
		isFirst = false;
	}else
		getCharBuf(src);
	
	if(NowChar=='/'){
		if(NextChar=='/'){	//skip line comment.
			while('\n'!=NowChar){
				NowChar = myFgetc(src);
				if(feof(src)) break;
			}
			NextChar = (feof(src))?' ':myFgetc(src);
			NowChar = ' ';
		}else if(NextChar=='*'){	//skip block comment.
			while(1){
				getCharBuf(src);
				if(NowChar=='/' && LastChar=='*')	//once NowChar is not '/', end the judgment process to save time.
					break;
				else if(feof(src))
					errExit(IncompleteAnotetation, NULL);
			}
			NowChar = ' ';
		}else{
			errExit(IncompleteAnotetation, NULL);
		}
	}

	return NowChar;
}

/**	Func:	get a uchar from the stream one by one, although it's a uchar located at a anotation.
 * 			Also move the flag of position.
 * 	Param:	src:	a FILE-pointer to stream
 * 	Retun:	uchar:	just return fgetc(src) at any time.
 */
uchar myFgetc(FILE *src)
{
	uchar res;
	char ch = fgetc(src);
	if(ch==-1) res = ' ';
	else res = (uchar)ch;
	if(res=='\n'){
		_pos.line++;
		_pos.column=0;
	}else{
		_pos.column++;
	}
	return res;	
}

void errExit(ErrorCode cod, char *description)
{
	printf("Error:	");
	switch (cod)
	{
	case IncompleteAnotetation:
		printf("<Incomplete Anotetation>");
		break;
	case MemAlloc_Failure:
		printf("<Memory Allocation Failure>");
		break;
	case OpenFile_Failure:
		printf("<Open_File Failure>");
		break;
	case TooLong:
		printf("<Too Long>");
		break;
	case NullPointer:
		printf("<Null Pointer>");
		break;
	case InvalidOperator:
		printf("<Invalid Operator>");
		printf("--'%c%c'", _chBuf[1], _chBuf[0]);
		break;
	case OtherError:
		printf("<Unknowen Error>");
		break;	
	default:
		printf("Exit from errExit(ErrorCode, char*):void");
	}

	printf("--%s", description);
	printf(" --..[%3d,%3d]\n", _pos.line, _pos.column-1);

	exit(EXIT_FAILURE);
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
	//In C language objects with static storage duration can only be initialized with constant expressions. 
	char *str = malloc(sizeof(char)*(MAX_LEN+3));
	int lenl = 15;
	int lenr = MAX_LEN;
	switch (tok)
	{
	case JAVA_ID:
		printf("<%*s,%*s>", lenl, "identifier", lenr, _tokBuf.value.chs);
		break;
	case JAVA_CONST_INT:
		printf("<%*s,%*d>", lenl, "const_int", lenr, _tokBuf.value.num_int);
		break;
	case JAVA_CONST_DOUBLE:
		printf("<%*s,%*f>", lenl, "const_double", lenr, _tokBuf.value.num_dbl);
		break;
	case JAVA_CONST_STR:
		printf("<%*s,%*s>", lenl, "const_str", lenr, concateStrs(str, "\"", _tokBuf.value.chs, "\""));
		break;
	case JAVA_SIGN:
		_tokBuf.value.chs[0] = _tokBuf.value.sign;
		_tokBuf.value.chs[1] = 0;
		printf("<%*s,%*s>", lenl, "signal", lenr, concateStrs(str, "'", _tokBuf.value.chs, "'"));
		break;
	case JAVA_EOF:
		printf("<%*s,%*d>", lenl, "EOF", lenr, -1);
		break;
	default:
		if(tok<=JAVA_KEY_VOLATILE){
			printf("<%*s,%*s>", lenl, "keyword", lenr, _tokBuf.value.chs);
		}else{
			printf("<%*s,%*s>", lenl, "operator", lenr,  concateStrs(str, "\"", _opDict[tok-JAVA_OP_LE], "\""));
		}
		break;
	}
	free(str);
}
