/****************************************************/
/* File: globals.h                                  */
/* Global types and vars for TINY compiler          */
/* must come before other include files             */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

/* MAXRESERVED = the number of reserved words */
#define MAXRESERVED 6

typedef enum
/* book-keeping tokens */
{
   ENDFILE,
   ERROR,

   /* comment symbols & error */
   COMMENT,
   COMMENT_ERROR,

   /* reserved words */
   IF,
   ELSE,
   INT,
   RETURN,
   VOID,
   WHILE,

   /* multicharacter tokens */
   ID,
   NUM,

   /* special symbols */
   ASSIGN,
   SEMI,
   COMMA,

   LT,
   LTEQ,
   GT,
   GTEQ,
   EQ,
   NOTEQ,

   PLUS,
   MINUS,
   TIMES,
   OVER,

   LPAREN,
   RPAREN,
   LBRACE,
   RBRACE,
   LBRACKET,
   RBRACKET,
} TokenType;

extern FILE *source;  /* source code text file */
extern FILE *listing; /* listing output text file */
extern FILE *code;    /* code text file for TM simulator */

extern int lineno; /* source line number for listing */

/**************************************************/
/***********   Syntax tree for parsing ************/
/**************************************************/

typedef enum
{
   StmtK,
   ExpK,

   /* [HW2] Jiho Rhee */
   // TODO ExpType 하위로 이동하기
   TypeK,
   ArrSizeK,
} NodeKind;
typedef enum
{
   IfK,     /* IF statement */
   ElseK,   /* Else statement */
   AssignK, /* Value assign */
   // RepeatK,
   // ReadK,
   // WriteK

   /* [HW2] Jiho Rhee */
   CompoundK, /* COMPOUND statement */
   WhileK,    /* WHILE statement */
   ReturnK,   /* RETURN statement */

   VarDeclK,   /* Variable declaration */
   ArrayDeclK, /* Array declaration */
   FuncDeclK,  /* Function declaration */
} StmtKind;
typedef enum
{
   OpK,
   ConstK,
   IdK,

   /* [HW2] Jiho Rhee */
   VarCallK,   /* Variable call */
   ArrayCallK, /* Array call */
   FuncCallK,  /* Function call */
   ParamK,     /* Parameter */
   SimpleExpK, /* simple-expression */
   AddExpK,    /* additive-expression */
   TermK,      /* term */
} ExpKind;

/* ExpType is used for type checking */
typedef enum
{
   Void,
   Integer,
   IntegerArray,
   // Boolean
} ExpType;

#define MAXCHILDREN 3

typedef struct treeNode
{
   struct treeNode *child[MAXCHILDREN];
   struct treeNode *sibling;
   int lineno;
   NodeKind nodekind;
   union
   {
      StmtKind stmt;
      ExpKind exp;
   } kind;
   union
   {
      TokenType op;
      int val;
      char *name;
   } attr;
   ExpType type; /* for type checking of exps */
   int arr_size;
} TreeNode;

/**************************************************/
/***********   Flags for tracing       ************/
/**************************************************/

/* EchoSource = TRUE causes the source program to
 * be echoed to the listing file with line numbers
 * during parsing
 */
extern int EchoSource;

/* TraceScan = TRUE causes token information to be
 * printed to the listing file as each token is
 * recognized by the scanner
 */
extern int TraceScan;

/* TraceParse = TRUE causes the syntax tree to be
 * printed to the listing file in linearized form
 * (using indents for children)
 */
extern int TraceParse;

/* TraceAnalyze = TRUE causes symbol table inserts
 * and lookups to be reported to the listing file
 */
extern int TraceAnalyze;

/* TraceCode = TRUE causes comments to be written
 * to the TM code file as code is generated
 */
extern int TraceCode;

/* Error = TRUE prevents further passes if an error occurs */
extern int Error;
#endif
