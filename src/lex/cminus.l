/****************************************************/
/* File: cminus.l                                   */
/* Lex specification for C-                         */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

%{
#include "globals.h"
#include "util.h"
#include "scan.h"
/* lexeme of identifier or reserved word */
char tokenString[MAXTOKENLEN+1];
%}

digit       [0-9]
number      {digit}+
letter      [a-zA-Z]
identifier  {letter}[[:alnum:]]*
newline     \n|\r\n
whitespace  [ \t]+

%%

"if"            {return IF;}
"else"          {return ELSE;}
"int"           {return INT;}
"return"        {return RETURN;}
"void"          {return VOID;}
"while"         {return WHILE;}

"="             {return ASSIGN;}
";"             {return SEMI;}
","             {return COMMA;}

"<"             {return LT;}
"<="            {return LTEQ;}
">"             {return GT;}
">="            {return GTEQ;}
"=="            {return EQ;}
"!="            {return NOTEQ;}

"+"             {return PLUS;}
"-"             {return MINUS;}
"*"             {return TIMES;}
"/"             {return OVER;}

"("             {return LPAREN;}
")"             {return RPAREN;}
"{"             {return LBRACE;}
"}"             {return RBRACE;}
"["             {return LBRACKET;}
"]"             {return RBRACKET;}

{number}        {return NUM;}
{identifier}    {return ID;}
{newline}       {lineno++;}
{whitespace}    {/* skip whitespace */}
"/*"            { 
                  char c;
                  int status = 0; 
                  // 0: comment 내부
                  // 1: '*'이 입력되어 마지막 '/' 입력을 기다리는 상태

                  do {
                    c = input();
                    switch (c) {
                      case '*':
                        status = 1;
                        break;
                      case '/':
                        if (status == 1) //   /* ... */ 형식 완성. COMMENT
                          return COMMENT;
                        break;
                      case '\n': // line break
                        ++lineno;
                        status = 0;
                        break;
                      default: // skip
                        status = 0;
                      }
                    } while (c != EOF);
                  return COMMENT_ERROR;
                }
.               {return ERROR;}

%%

TokenType getToken(void)
{ static int firstTime = TRUE;
  TokenType currentToken;
  if (firstTime)
  { firstTime = FALSE;
    lineno++;
    yyin = source;
    yyout = listing;
  }
  currentToken = yylex();
  strncpy(tokenString,yytext,MAXTOKENLEN);
  if (TraceScan) {
    fprintf(listing,"\t%d: ",lineno);
    printToken(currentToken,tokenString);
  }
  return currentToken;
}

