/****************************************************/
/* File: main.c                                     */
/* Main program for TINY compiler                   */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"

/* set NO_PARSE to TRUE to get a scanner-only compiler */
#define NO_PARSE TRUE
/* set NO_ANALYZE to TRUE to get a parser-only compiler */
#define NO_ANALYZE FALSE

/* set NO_CODE to TRUE to get a compiler that does not
 * generate code
 */
#define NO_CODE FALSE

/**
 * [HW1] Jiho Rhee
 */
#define RESULT_FILE_LIST "result_file_list.txt"
#define FILE_OUT_SUFFIX "_20161250.txt"

#include "util.h"
#if NO_PARSE
#include "scan.h"
#else
#include "parse.h"
#if !NO_ANALYZE
#include "analyze.h"
#if !NO_CODE
#include "cgen.h"
#endif
#endif
#endif

/* allocate global variables */
int lineno = 0;
FILE *source;
FILE *listing;
FILE *code;

/* allocate and set tracing flags */
int EchoSource = FALSE;
int TraceScan = FALSE;
int TraceParse = FALSE;
int TraceAnalyze = FALSE;
int TraceCode = FALSE;

int Error = FALSE;

int main(int argc, char *argv[])
{
  TreeNode *syntaxTree;
  char pgm[120]; /* source code file name */
  if (argc != 2)
  {
    fprintf(stderr, "usage: %s <filename>\n", argv[0]);
    exit(1);
  }
  strcpy(pgm, argv[1]);
  if (strchr(pgm, '.') == NULL)
    strcat(pgm, ".tny");
  source = fopen(pgm, "r");
  if (source == NULL)
  {
    fprintf(stderr, "File %s not found\n", pgm);
    exit(1);
  }

  /* [HW1] Parse file name & get output file name */
  FILE *result_file_list = fopen(RESULT_FILE_LIST, "a");
  if (result_file_list == NULL)
  {
    fprintf(stderr, "fopen(%s) failed.\n", RESULT_FILE_LIST);
    exit(1);
  }

  char filename_copy[128], *fout_name, *ptr_dummy;
  strcpy(filename_copy, pgm);
  fout_name = strtok_r(filename_copy, ".", &ptr_dummy);
  strcat(fout_name, FILE_OUT_SUFFIX);

  /* Write output file name to result_file_list.txt */
  FILE *result_file = fopen(fout_name, "r");
  if (result_file == NULL)
  {
    fprintf(result_file_list, "%s\n", fout_name);
  }
  else
  {
    fclose(result_file);
  }

  listing = fopen(fout_name, "w"); /* send listing to screen */
  if (listing == NULL)
  {
    fprintf(stderr, "fopen(%s) failed.\n", fout_name);
    exit(1);
  }

  fprintf(listing, "\nC- COMPILATION: %s\n", pgm);
#if NO_PARSE
  fprintf(listing, "%-20s%-20s%s\n", "line number", "token", "lexeme");
  fprintf(listing, "================================================================================\n");
  TokenType tokenType;
  while ((tokenType = getToken()) != ENDFILE)
    printToken(tokenType, tokenString);
  if (tokenType == ENDFILE) /* print EOF */
    printToken(tokenType, tokenString);

#else
  syntaxTree = parse();
  if (TraceParse)
  {
    fprintf(listing, "\nSyntax tree:\n");
    printTree(syntaxTree);
  }
#if !NO_ANALYZE
  if (!Error)
  {
    if (TraceAnalyze)
      fprintf(listing, "\nBuilding Symbol Table...\n");
    buildSymtab(syntaxTree);
    if (TraceAnalyze)
      fprintf(listing, "\nChecking Types...\n");
    typeCheck(syntaxTree);
    if (TraceAnalyze)
      fprintf(listing, "\nType Checking Finished\n");
  }
#if !NO_CODE
  if (!Error)
  {
    char *codefile;
    int fnlen = strcspn(pgm, ".");
    codefile = (char *)calloc(fnlen + 4, sizeof(char));
    strncpy(codefile, pgm, fnlen);
    strcat(codefile, ".tm");
    code = fopen(codefile, "w");
    if (code == NULL)
    {
      printf("Unable to open %s\n", codefile);
      exit(1);
    }
    codeGen(syntaxTree, codefile);
    fclose(code);
  }
#endif
#endif
#endif
  fclose(source);
  fclose(result_file_list);
  return 0;
}
