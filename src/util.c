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
  /* comment는 skip */
  if (token == COMMENT)
    return;

  /* line number */
  // fprintf(listing, "%-10d%10s", lineno, "");
  switch (token)
  {
  /* [HW2] Jiho Rhee */
  case IF:
  case WHILE:
  case RETURN:
  case INT:
  case VOID:
  case ELSE:
    fprintf(listing, "reserved word: %s\n", tokenString);
    break;

  /* ADDITION AND MODIFICATION OF SOME TOKENS */
  case ASSIGN:
    fprintf(listing, "=\n");
    break;
  case EQ:
    fprintf(listing, "==\n");
    break;
  case NOTEQ:
    fprintf(listing, "!=\n");
    break;
  case LT:
    fprintf(listing, "<\n");
    break;
  case LTEQ:
    fprintf(listing, "<=\n");
    break;
  case GT:
    fprintf(listing, ">\n");
    break;
  case GTEQ:
    fprintf(listing, ">=\n");
    break;
  case LPAREN:
    fprintf(listing, "(\n");
    break;
  case RPAREN:
    fprintf(listing, ")\n");
    break;
  case LBRACE:
    fprintf(listing, "{\n");
    break;
  case RBRACE:
    fprintf(listing, "}\n");
    break;
  case LBRACKET:
    fprintf(listing, "[\n");
    break;
  case RBRACKET:
    fprintf(listing, "]\n");
    break;
  case SEMI:
    fprintf(listing, ";\n");
    break;
  case COMMA:
    fprintf(listing, ",\n");
    break;
  case PLUS:
    fprintf(listing, "+\n");
    break;
  case MINUS:
    fprintf(listing, "-\n");
    break;
  case TIMES:
    fprintf(listing, "*\n");
    break;
  case OVER:
    fprintf(listing, "/\n");
    break;
  case ENDFILE:
    fprintf(listing, "EOF\n");
    break;
  case NUM:
    fprintf(listing,
            "NUM, val= %s\n", tokenString);
    break;
  case ID:
    fprintf(listing,
            "ID, name= %s\n", tokenString);
    break;
  case ERROR:
    fprintf(listing,
            "ERROR: %s\n", tokenString);
    break;
    // /* error handling */
    // case ERROR:
    //   fprintf(listing, "%-20s%s\n", "ERROR", tokenString);
    //   break;
    // case COMMENT_ERROR:
    //   fprintf(listing, "%-20s%s\n", "ERROR", "Comment Error");
    //   break;
    // case ENDFILE:
    //   fprintf(listing, "%-20s\n", "EOF");
    //   break;

    // /* 나머지 token들은 그대로 출력 */
    // /* reserved words */
    // case IF:
    //   fprintf(listing, "%-20s\t%s\n", "IF", tokenString);
    //   break;
    // case ELSE:
    //   fprintf(listing, "%-20s\t%s\n", "ELSE", tokenString);
    //   break;
    // case INT:
    //   fprintf(listing, "%-20s%s\n", "INT", tokenString);
    //   break;
    // case RETURN:
    //   fprintf(listing, "%-20s\t%s\n", "RETURN", tokenString);
    //   break;
    // case VOID:
    //   fprintf(listing, "%-20s\t%s\n", "VOID", tokenString);
    //   break;
    // case WHILE:
    //   fprintf(listing, "%-20s\t%s\n", "WHILE", tokenString);
    //   break;

    // /* multicharacter tokens */
    // case ID:
    //   fprintf(listing, "%-20s%s\n", "ID", tokenString);
    //   break;
    // case NUM:
    //   fprintf(listing, "%-20s%s\n", "NUM", tokenString);
    //   break;

    // /* special symbols */
    // case ASSIGN:
    //   fprintf(listing, "%-20s%s\n", "=", tokenString);
    //   break;
    // case SEMI:
    //   fprintf(listing, "%-20s%s\n", ";", tokenString);
    //   break;
    // case COMMA:
    //   fprintf(listing, "%-20s%s\n", ",", tokenString);
    //   break;

    // case LT:
    //   fprintf(listing, "%-20s%s\n", "<", tokenString);
    //   break;
    // case LTEQ:
    //   fprintf(listing, "%-20s%s\n", "<=", tokenString);
    //   break;
    // case GT:
    //   fprintf(listing, "%-20s%s\n", ">", tokenString);
    //   break;
    // case GTEQ:
    //   fprintf(listing, "%-20s%s\n", ">=", tokenString);
    //   break;
    // case EQ:
    //   fprintf(listing, "%-20s%s\n", "==", tokenString);
    //   break;
    // case NOTEQ:
    //   fprintf(listing, "%-20s%s\n", "!=", tokenString);
    //   break;

    // case PLUS:
    //   fprintf(listing, "%-20s%s\n", "+", tokenString);
    //   break;
    // case MINUS:
    //   fprintf(listing, "%-20s%s\n", "-", tokenString);
    //   break;
    // case TIMES:
    //   fprintf(listing, "%-20s%s\n", "*", tokenString);
    //   break;
    // case OVER:
    //   fprintf(listing, "%-20s%s\n", "/", tokenString);
    //   break;

    // case LPAREN:
    //   fprintf(listing, "%-20s%s\n", "(", tokenString);
    //   break;
    // case RPAREN:
    //   fprintf(listing, "%-20s%s\n", ")", tokenString);
    //   break;
    // case LBRACE:
    //   fprintf(listing, "%-20s%s\n", "{", tokenString);
    //   break;
    // case RBRACE:
    //   fprintf(listing, "%-20s%s\n", "}", tokenString);
    //   break;
    // case LBRACKET:
    //   fprintf(listing, "%-20s%s\n", "[", tokenString);
    //   break;
    // case RBRACKET:
    //   fprintf(listing, "%-20s%s\n", "]", tokenString);
    //   break;

  default: /* should never happen */
    fprintf(listing, "Unknown token: %d\n", token);
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
    t->is_param = FALSE;
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

char *typeName(ExpType type)
{
  static char i[] = "int";
  static char v[] = "void";
  static char invalid[] = "<<invalid type>>";

  switch (type)
  {
  case Integer:
    return i;
  case Void:
    return v;
  default:
    return invalid;
  }
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
      case CompStmtK:
        fprintf(listing, "Compound Statement :\n");
        break;
      case SelStmtK:
        if (tree->child[2] != NULL)
          fprintf(listing, "If (condition) (body) (else) \n");
        else
          fprintf(listing, "If (condition) (body)\n");
        break;
      case IterStmtK:
        fprintf(listing, "While (condition) (body) \n");
        break;
      case RetStmtK:
        if (tree->child[0] == NULL)
          fprintf(listing, "Return Statement, with NOTHING\n");
        else
          fprintf(listing, "Return Statement, with below\n");
        break;
      case CallK:
        if (tree->child[0] != NULL)
          fprintf(listing, "Call, name : %s, with arguments below\n", tree->attr.name);
        else
          fprintf(listing, "Call, name : %s, with NOTHING\n", tree->attr.name);
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
      case VarDeclK:
        if (tree->is_param == TRUE)
          fprintf(listing, "Single Parameter, name : %s, type : %s\n", tree->attr.name, typeName(tree->type));
        else
          fprintf(listing, "Var Declaration, name : %s, type : %s\n", tree->attr.name, typeName(tree->type));
        break;
      case VarArrayDeclK:
        if (tree->is_param == TRUE)
          fprintf(listing, "Array Parameter, name : %s, type : %s\n", tree->attr.name, typeName(tree->type));
        else
          fprintf(listing, "Array Var Declaration, name : %s, type : %s, size : %d\n", tree->attr.name, typeName(tree->type), tree->arr_size);
        break;
      case FuncDeclK:
        fprintf(listing, "Function Declaration, name : %s, type : %s\n", tree->attr.name, typeName(tree->type));
        break;
      case AssignK:
        fprintf(listing, "Assign : (destination) (source) \n");
        break;
      case OpK:
        fprintf(listing, "Op : ");
        printToken(tree->attr.op, "\0");
        break;
      case IdK:
        fprintf(listing, "Id : %s\n", tree->attr.name);
        break;
      case ConstK:
        fprintf(listing, "Const : %d\n", tree->attr.val);
        break;
      default:
        fprintf(listing, "Unknown ExpNode kind\n");
        break;
      }
    }
    else
      fprintf(listing, "Unknown node kind\n");
    for (i = 0; i < MAXCHILDREN; i++)
      printTree(tree->child[i]);
    tree = tree->sibling;
  }
  UNINDENT;
}
// void printTree(TreeNode *tree)
// {
//   int i;
//   INDENT;
//   while (tree != NULL)
//   {
//     printSpaces();
//     if (tree->nodekind == StmtK)
//     {
//       switch (tree->kind.stmt)
//       {
//       case CompStmtK:
//         fprintf(listing, "Compound Statement :\n");
//         break;
//       case SelStmtK:
//         if (tree->child[2] != NULL)
//           fprintf(listing, "If (condition) (body) (else) \n");
//         else
//           fprintf(listing, "If (condition) (body)\n");
//         break;
//       case IterStmtK:
//         fprintf(listing, "While (condition) (body) \n");
//         break;
//       case RetStmtK:
//         if (tree->child[0] == NULL)
//           fprintf(listing, "Return Statement, with NOTHING\n");
//         else
//           fprintf(listing, "Return Statement, with below\n");
//         break;
//       case CallK:
//         if (tree->child[0] != NULL)
//           fprintf(listing, "Call, name : %s, with arguments below\n", tree->attr.name);
//         else
//           fprintf(listing, "Call, name : %s, with NOTHING\n", tree->attr.name);
//         break;
//       // case IfK:
//       //   fprintf(listing, "If\n");
//       //   break;
//       // case RepeatK:
//       //   fprintf(listing, "Repeat\n");
//       //   break;
//       // case AssignK:
//       //   fprintf(listing, "Assign to: %s\n", tree->attr.name);
//       //   break;
//       // case ReadK:
//       //   fprintf(listing, "Read: %s\n", tree->attr.name);
//       //   break;
//       // case WriteK:
//       //   fprintf(listing, "Write\n");
//       //   break;
//       default:
//         fprintf(listing, "Unknown ExpNode kind\n");
//         break;
//       }
//     }
//     else if (tree->nodekind == ExpK)
//     {
//       switch (tree->kind.exp)
//       {
//       case VarDeclK:
//         if (tree->is_param == TRUE)
//           fprintf(listing, "Single Parameter, name : %s, type : %s\n", tree->attr.name, typeName(tree->type));
//         else
//           fprintf(listing, "Var Declaration, name : %s, type : %s\n", tree->attr.name, typeName(tree->type));
//         break;
//       case VarArrayDeclK:
//         if (tree->is_param == TRUE)
//           fprintf(listing, "Array Parameter, name : %s, type : %s\n", tree->attr.name, typeName(tree->type));
//         else
//           fprintf(listing, "Array Var Declaration, name : %s, type : %s, size : %d\n", tree->attr.name, typeName(tree->type), tree->arr_size);
//         break;
//       case FuncDeclK:
//         fprintf(listing, "Function Declaration, name : %s, type : %s\n", tree->attr.name, typeName(tree->type));
//         break;
//       case AssignK:
//         fprintf(listing, "Assign : (destination) (source) \n");
//         break;
//       case OpK:
//         fprintf(listing, "Op: ");
//         printToken(tree->attr.op, "\0");
//         break;
//       case ConstK:
//         fprintf(listing, "Const: %d\n", tree->attr.val);
//         break;
//       case IdK:
//         fprintf(listing, "Id: %s\n", tree->attr.name);
//         break;
//       default:
//         fprintf(listing, "Unknown ExpNode kind\n");
//         break;
//       }
//     }
//     else
//       fprintf(listing, "Unknown node kind\n");
//     for (i = 0; i < MAXCHILDREN; i++)
//       printTree(tree->child[i]);
//     tree = tree->sibling;
//   }
//   UNINDENT;
// }
