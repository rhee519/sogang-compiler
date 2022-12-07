/****************************************************/
/* File: util.c                                     */
/* Utility function implementation                  */
/* for the TINY compiler                            */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "util.h"

/* Procedure printToken prints a token
 * and its lexeme to the listing file
 */
void printToken(TokenType token, const char *tokenString)
{
  if (TraceScan)
  {
    /* comment는 skip */
    if (token == COMMENT)
      return;

    /* line number */
    fprintf(listing, "%-10d%10s", lineno, "");
    switch (token)
    {

    /* error handling */
    case ERROR:
      fprintf(listing, "%-20s%s\n", "ERROR", tokenString);
      break;
    case COMMENT_ERROR:
      fprintf(listing, "%-20s%s\n", "ERROR", "Comment Error");
      break;
    case ENDFILE:
      fprintf(listing, "%-20s\n", "EOF");
      break;

    /* 나머지 token들은 그대로 출력 */
    /* reserved words */
    case IF:
      fprintf(listing, "%-20s\t%s\n", "IF", tokenString);
      break;
    case ELSE:
      fprintf(listing, "%-20s\t%s\n", "ELSE", tokenString);
      break;
    case INT:
      fprintf(listing, "%-20s%s\n", "INT", tokenString);
      break;
    case RETURN:
      fprintf(listing, "%-20s\t%s\n", "RETURN", tokenString);
      break;
    case VOID:
      fprintf(listing, "%-20s\t%s\n", "VOID", tokenString);
      break;
    case WHILE:
      fprintf(listing, "%-20s\t%s\n", "WHILE", tokenString);
      break;

    /* multicharacter tokens */
    case ID:
      fprintf(listing, "%-20s%s\n", "ID", tokenString);
      break;
    case NUM:
      fprintf(listing, "%-20s%s\n", "NUM", tokenString);
      break;

    /* special symbols */
    case ASSIGN:
      fprintf(listing, "%-20s%s\n", "=", tokenString);
      break;
    case SEMI:
      fprintf(listing, "%-20s%s\n", ";", tokenString);
      break;
    case COMMA:
      fprintf(listing, "%-20s%s\n", ",", tokenString);
      break;

    case LT:
      fprintf(listing, "%-20s%s\n", "<", tokenString);
      break;
    case LTEQ:
      fprintf(listing, "%-20s%s\n", "<=", tokenString);
      break;
    case GT:
      fprintf(listing, "%-20s%s\n", ">", tokenString);
      break;
    case GTEQ:
      fprintf(listing, "%-20s%s\n", ">=", tokenString);
      break;
    case EQ:
      fprintf(listing, "%-20s%s\n", "==", tokenString);
      break;
    case NOTEQ:
      fprintf(listing, "%-20s%s\n", "!=", tokenString);
      break;

    case PLUS:
      fprintf(listing, "%-20s%s\n", "+", tokenString);
      break;
    case MINUS:
      fprintf(listing, "%-20s%s\n", "-", tokenString);
      break;
    case TIMES:
      fprintf(listing, "%-20s%s\n", "*", tokenString);
      break;
    case OVER:
      fprintf(listing, "%-20s%s\n", "/", tokenString);
      break;

    case LPAREN:
      fprintf(listing, "%-20s%s\n", "(", tokenString);
      break;
    case RPAREN:
      fprintf(listing, "%-20s%s\n", ")", tokenString);
      break;
    case LBRACE:
      fprintf(listing, "%-20s%s\n", "{", tokenString);
      break;
    case RBRACE:
      fprintf(listing, "%-20s%s\n", "}", tokenString);
      break;
    case LBRACKET:
      fprintf(listing, "%-20s%s\n", "[", tokenString);
      break;
    case RBRACKET:
      fprintf(listing, "%-20s%s\n", "]", tokenString);
      break;

    default: /* should never happen */
      fprintf(listing, "Unknown token: %d\n", token);
    }
  }
  if (TraceParse)
  {
    switch (token)
    {
      // TODO
    /* error handling */
    case ERROR:
      fprintf(listing, "%s, %s\n", "ERROR", tokenString);
      break;
    case COMMENT_ERROR:
      fprintf(listing, "%s, %s\n", "ERROR", "Comment Error");
      break;
    case ENDFILE:
      fprintf(listing, "%s\n", "EOF");
      break;

    /* 나머지 token들은 그대로 출력 */
    /* reserved words */
    case IF:
      fprintf(listing, "%s, %s\n", "IF", tokenString);
      break;
    case ELSE:
      fprintf(listing, "%s, %s\n", "ELSE", tokenString);
      break;
    case INT:
      fprintf(listing, "%s, %s\n", "INT", tokenString);
      break;
    case RETURN:
      fprintf(listing, "%s, %s\n", "RETURN", tokenString);
      break;
    case VOID:
      fprintf(listing, "%s, %s\n", "VOID", tokenString);
      break;
    case WHILE:
      fprintf(listing, "%s, %s\n", "WHILE", tokenString);
      break;

    /* multicharacter tokens */
    case ID:
      fprintf(listing, "%s, %s\n", "ID", tokenString);
      break;
    case NUM:
      fprintf(listing, "%s, %s\n", "NUM", tokenString);
      break;

    /* special symbols */
    case ASSIGN:
      fprintf(listing, "%s, %s\n", "=", tokenString);
      break;
    case SEMI:
      fprintf(listing, "%s, %s\n", ";", tokenString);
      break;
    case COMMA:
      fprintf(listing, "%s, %s\n", ",", tokenString);
      break;

    case LT:
      fprintf(listing, "%s, %s\n", "<", tokenString);
      break;
    case LTEQ:
      fprintf(listing, "%s, %s\n", "<=", tokenString);
      break;
    case GT:
      fprintf(listing, "%s, %s\n", ">", tokenString);
      break;
    case GTEQ:
      fprintf(listing, "%s, s%s\n", ">=", tokenString);
      break;
    case EQ:
      fprintf(listing, "%s, %s\n", "==", tokenString);
      break;
    case NOTEQ:
      fprintf(listing, "%s, %s\n", "!=", tokenString);
      break;

    case PLUS:
      fprintf(listing, "%s, %s\n", "+", tokenString);
      break;
    case MINUS:
      fprintf(listing, "%s, %s\n", "-", tokenString);
      break;
    case TIMES:
      fprintf(listing, "%s, %s\n", "*", tokenString);
      break;
    case OVER:
      fprintf(listing, "%s, %s\n", "/", tokenString);
      break;

    case LPAREN:
      fprintf(listing, "%s, %s\n", "(", tokenString);
      break;
    case RPAREN:
      fprintf(listing, "%s, %s\n", ")", tokenString);
      break;
    case LBRACE:
      fprintf(listing, "%s, %s\n", "{", tokenString);
      break;
    case RBRACE:
      fprintf(listing, "%s, %s\n", "}", tokenString);
      break;
    case LBRACKET:
      fprintf(listing, "%s, %s\n", "[", tokenString);
      break;
    case RBRACKET:
      fprintf(listing, "%s, %s\n", "]", tokenString);
      break;

    default: /* should never happen */
      fprintf(listing, "Unknown token: %d\n", token);
    }
  }
}

/* Function newStmtNode creates a new statement
 * node for syntax tree construction
 */
TreeNode *newStmtNode(StmtKind kind)
{
  TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));
  int i;
  if (t == NULL)
    fprintf(listing, "Out of memory error at line %d\n", lineno);
  else
  {
    for (i = 0; i < MAXCHILDREN; i++)
      t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = StmtK;
    t->kind.stmt = kind;
    t->lineno = lineno;

    /* [HW2] Jiho Rhee */
    t->is_param = FALSE;
    t->arr_size = 0;
  }
  return t;
}

/* Function newExpNode creates a new expression
 * node for syntax tree construction
 */
TreeNode *newExpNode(ExpKind kind)
{
  TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));
  int i;
  if (t == NULL)
    fprintf(listing, "Out of memory error at line %d\n", lineno);
  else
  {
    for (i = 0; i < MAXCHILDREN; i++)
      t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = ExpK;
    t->kind.exp = kind;
    t->lineno = lineno;
    t->type = Void;

    /* [HW2] Jiho Rhee */
    t->is_param = FALSE;
    t->arr_size = 0;
  }
  return t;
}

/**
 * [HW2] Jiho Rhee
 */

/**
 * Create new node for TYPE keyword.
 * This node will be a child of VarDeclK or ArrayDeclK or FuncDeclK.
 */
TreeNode *newTypeNode(ExpType type)
{
  TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));
  int i;
  if (t == NULL)
    fprintf(listing, "Out of memory error at line %d\n", lineno);
  else
  {
    for (i = 0; i < MAXCHILDREN; i++)
      t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = TypeK;
    t->lineno = lineno;
    t->type = type; /* This member will be printed to parse tree. */
  }
  return t;
}

/**
 * Create new node for ARRAY SIZE.
 * This node will be a child of ArrayDeclK.
 */
TreeNode *newArrSizeNode(int size)
{
  TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));
  int i;
  if (t == NULL)
    fprintf(listing, "Out of memory error at line %d\n", lineno);
  else
  {
    for (i = 0; i < MAXCHILDREN; i++)
      t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = ArrSizeK;
    t->lineno = lineno;
    t->arr_size = size; /* This member will be printed to parse tree. */
  }
  return t;
}

/**
 * Create new node for PARAM.
 * This node will be a child of FuncDeclK, FuncCallK.
 */
TreeNode *newParamNode(ExpType type)
{
  TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));
  int i;
  if (t == NULL)
    fprintf(listing, "Out of memory error at line %d\n", lineno);
  else
  {
    for (i = 0; i < MAXCHILDREN; i++)
      t->child[i] = NULL;
    t->lineno = lineno;
    t->sibling = NULL;
    t->nodekind = ParamK;
    t->attr.name = type == Integer ? "int" : "void";
    t->type = type;
    t->is_param = TRUE;
  }
  return t;
}

/* Function copyString allocates and makes a new
 * copy of an existing string
 */
char *copyString(char *s)
{
  int n;
  char *t;
  if (s == NULL)
    return NULL;
  n = strlen(s) + 1;
  t = (char *)malloc(n);
  if (t == NULL)
    fprintf(listing, "Out of memory error at line %d\n", lineno);
  else
    strcpy(t, s);
  return t;
}

/* Variable indentno is used by printTree to
 * store current number of spaces to indent
 */
static int indentno = 0;

/* macros to increase/decrease indentation */
#define INDENT indentno += 2
#define UNINDENT indentno -= 2

/* printSpaces indents by printing spaces */
static void printSpaces(void)
{
  int i;
  for (i = 0; i < indentno; i++)
    fprintf(listing, " ");
}

/* procedure printTree prints a syntax tree to the
 * listing file using indentation to indicate subtrees
 */
void printTree(TreeNode *tree)
{
  int i;
  INDENT;
  while (tree != NULL)
  {
    printSpaces();
    if (tree->nodekind == StmtK)
    {
      switch (tree->kind.stmt)
      {
      case IfK:
        fprintf(listing, "If\n");
        break;
      // case RepeatK:
      //   fprintf(listing, "Repeat\n");
      //   break;
      case AssignK:
        fprintf(listing, "Assign to: %s\n", tree->attr.name);
        break;
      // case ReadK:
      //   fprintf(listing, "Read: %s\n", tree->attr.name);
      //   break;
      // case WriteK:
      //   fprintf(listing, "Write\n");
      //   break;

      /* [HW2] Jiho Rhee */
      case CompoundK: /* COMPOUND statement */
        fprintf(listing, "Compound Statement\n");
        break;
      case WhileK: /* WHILE statement */
        fprintf(listing, "While\n");
        break;
      case ReturnK: /* RETURN statement */
        fprintf(listing, "Return\n");
        break;
      case VarDeclK: /* Variable declaration */
        fprintf(listing, "Variable Declare : %s\n", tree->attr.name);
        break;
      case ArrayDeclK: /* Array declaration */
        fprintf(listing, "Array Declare : %s\n", tree->attr.name);
        break;
      case FuncDeclK: /* Function declaration */
        fprintf(listing, "Function Declare : %s\n", tree->attr.name);
        break;
      default:
        fprintf(listing, "Unknown ExpNode kind\n");
        break;
      }
    }
    else if (tree->nodekind == ExpK)
    {
      switch (tree->kind.exp)
      {
      case OpK:
        fprintf(listing, "Op: ");
        printToken(tree->attr.op, "\0");
        break;
      case ConstK:
        fprintf(listing, "Const: %d\n", tree->attr.val);
        break;
      case IdK:
        fprintf(listing, "Id: %s\n", tree->attr.name);
        break;
      case VarCallK:
        fprintf(listing, "Variable: %s\n", tree->attr.name);
        break;
      case ArrayCallK:
        fprintf(listing, "Array: %s\n", tree->attr.name);
        break;
      case FuncCallK:
        fprintf(listing, "Function: %s\n", tree->attr.name);
        break;
      default:
        fprintf(listing, "Unknown ExpNode kind\n");
        break;
      }
    }
    else if (tree->nodekind == TypeK)
    {
      switch (tree->type)
      {
      case Integer:
        fprintf(listing, "Type: %s\n", "int");
        break;
      case Void:
        fprintf(listing, "Type: %s\n", "void");
        break;
      default:
        fprintf(listing, "Unknown type\n");
        break;
      }
    }
    else if (tree->nodekind == ArrSizeK)
      fprintf(listing, "Size: %d\n", tree->arr_size);
    else if (tree->nodekind == ParamK)
      fprintf(listing, "Parameter: %s\n", tree->attr.name);
    else
      fprintf(listing, "Unknown node kind\n");
    for (i = 0; i < MAXCHILDREN; i++)
      printTree(tree->child[i]);
    tree = tree->sibling;
  }
  UNINDENT;
}
