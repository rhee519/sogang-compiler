/****************************************************/
/* File: parse.c                                    */
/* The parser implementation for the TINY compiler  */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

static TokenType token; /* holds current token */

/**
 * [HW2] Jiho Rhee
 *
 * A BNF grammar for C- is as follows:
 *
 * program → declaration-list
 * declaration-list → declaration-list declaration | declaration
 * declaration → var-declaration | fun-declaration
 * var-declaration → type-specifier ID; | type-specifier ID [NUM];
 * type-specifier → int | void
 * fun-declaration → type-specifier ID ( params ) compound-stmt
 * params → param-list | void
 * param-list → param-list , param | param
 * param → type-specifier ID | type-specifier ID [ ]
 * compound-stmt → { local-declarations statement-list }
 * local-declarations → local-declarations var-declaration | empty
 * statement-list → statement-list statement | empty
 * statement → expression-stmt | compound-stmt | selection-stmt | iteration-stmt | return-stmt
 * expression-stmt → expression ; | ;
 * selction-stmt → if ( expression ) statement | if ( expression ) statement else statement
 * iteration-stmt → while ( expression ) statement
 * return-stmt → return ; | return expression ;
 * expression → var = expression | simple-expression
 * var → ID | ID [ expression ]
 * simple-expression → additive-expression relop additive-expression | additive-expression
 * additive-expression → additive-expression addop term | term
 * term → term mulop factor
 * factor → ( expression ) | var | call | NUM
 * relop → <= | < | >= | > | == | !=
 * addop → + | -
 * mulop → * | /
 * call → ID ( args )
 * args → arg-list | empty
 * arg-list → arg-list , expression | expression
 */
static TreeNode *declare_list(void);  /* declaration-list → declaration-list declaration | declaration */
static TreeNode *declare(void);       /* declaration → var-declaration | fun-declaration */
static TreeNode *var_declare(void);   /* var-declaration → type-specifier ID; | type-specifier ID [NUM]; */
static ExpType type_spec(void);       /* type-specifier → int | void */
static TreeNode *fun_declare(void);   /* fun-declaration → type-specifier ID ( params ) compound-stmt */
static TreeNode *params(void);        /* params → param-list | void */
static TreeNode *param_list(void);    /* param-list → param-list , param | param */
static TreeNode *param(void);         /* param → type-specifier ID | type-specifier ID [ ] */
static TreeNode *compound_stmt(void); /* compound-stmt → { local-declarations statement-list } */
static TreeNode *local_declare(void); /* local-declarations → local-declarations var-declaration | empty */
static TreeNode *stmt_list(void);     /* statement-list → statement-list statement | empty */
static TreeNode *stmt(void);          /* statement → expression-stmt | compound-stmt | selection-stmt | iteration-stmt | return-stmt */
static TreeNode *exp_stmt(void);      /* expression-stmt → expression ; | ; */
static TreeNode *select_stmt(void);   /* selction-stmt → if ( expression ) statement | if ( expression ) statement else statement */
static TreeNode *iter_stmt(void);     /* iteration-stmt → while ( expression ) statement */
static TreeNode *return_stmt(void);   /* return-stmt → return ; | return expression ; */
static TreeNode *exp(void);           /* expression → var = expression | simple-expression */
static TreeNode *var(void);           /* var → ID | ID [ expression ] */
static TreeNode *simple_exp(void);    /* simple-expression → additive-expression relop additive-expression | additive-expression */
static TreeNode *add_exp(void);       /* additive-expression → additive-expression addop term | term */
static TreeNode *term(void);          /* term → term mulop factor */
static TreeNode *factor(void);        /* factor → ( expression ) | var | call | NUM */
static TreeNode *relop(void);         /* relop → <= | < | >= | > | == | != */
static TreeNode *addop(void);         /* addop → + | - */
static TreeNode *mulop(void);         /* mulop → * | / */
static TreeNode *call(void);          /* call → ID ( args ) */
static TreeNode *args(void);          /* args → arg-list | empty */
static TreeNode *arg_list(void);      /* arg-list → arg-list , expression | expression */

static void syntaxError(char *message)
{
  fprintf(listing, "\n>>> ");
  fprintf(listing, "Syntax error at line %d: %s", lineno, message);
  Error = TRUE;
}

static void match(TokenType expected)
{
  if (token == expected)
    token = getToken();
  else
  {
    syntaxError("unexpected token ( match() ) -> ");
    printToken(token, tokenString);
    fprintf(listing, "\t\texpected: ");
    printToken(expected, "");
    fprintf(listing, "      ");
  }
}

/**
 * [HW2] Jiho Rhee
 */

static int is_func_decl(TreeNode *t)
{
  return t != NULL && t->nodekind == StmtK && t->kind.stmt == FuncDeclK;
}

/* declaration-list → declaration-list declaration | declaration */
static TreeNode *declare_list(void)
{
  TreeNode *t = declare();
  TreeNode *p = t;

  /* FuncDecl is not followed by SEMI(;). */
  if (!is_func_decl(t))
    match(SEMI);

  while (token != ENDFILE)
  {
    TreeNode *q;
    q = declare();
    /* FuncDecl is not followed by SEMI(;). */
    if (!is_func_decl(q))
      match(SEMI);

    if (q != NULL)
    {
      if (t == NULL)
        t = p = q;
      else /* now p cannot be NULL either */
      {
        p->sibling = q;
        p = q;
      }
    }
  }
  return t;
}

/* declaration → var-declaration | fun-declaration */
static TreeNode *declare(void)
{
  if (token == ENDFILE)
    return NULL;

  TreeNode *t = NULL;
  ExpType type = type_spec();
  char *name = copyString(tokenString);
  int arr_size;

  match(ID);

  switch (token)
  {
  case SEMI: /* Variable declaration. ex) int a; */
    t = newStmtNode(VarDeclK);
    if (t != NULL)
    {
      t->attr.name = name;
      t->type = type;
      t->child[0] = newTypeNode(t->type);
    }
    break;
  case LBRACKET: /* Array declaration. ex) int arr[10]; */
    t = newStmtNode(ArrayDeclK);
    if (t != NULL)
    {
      t->attr.name = name;
      t->type = type;
      t->child[0] = newTypeNode(t->type);
    }
    match(LBRACKET);
    if (t != NULL)
    {
      t->arr_size = atoi(tokenString);
      t->child[1] = newArrSizeNode(t->arr_size);
    }
    match(NUM);
    match(RBRACKET);
    break;
  case LPAREN: /* Function declaration. ex) int sort(int arr[], int size) {...} */
    t = newStmtNode(FuncDeclK);
    if (t != NULL)
    {
      t->attr.name = name;
      t->type = type;
      t->child[0] = newTypeNode(t->type);
    }
    match(LPAREN);
    if (t != NULL)
      t->child[1] = params();
    match(RPAREN);
    if (t != NULL)
      t->child[2] = compound_stmt();
    break;
  default:
    // TODO
    break;
  }

  return t;
}

/* var-declaration → type-specifier ID; | type-specifier ID [NUM]; */
static TreeNode *var_declare(void)
{
  TreeNode *t = NULL;
  ExpType type = type_spec();
  char *name = copyString(tokenString);
  match(ID);
  if (token == SEMI) /* var */
  {
    t = newStmtNode(VarDeclK);
    if (t != NULL)
      t->attr.name = name;
  }
  else if (token == LBRACKET) /* array */
  {
    t = newStmtNode(ArrayDeclK);
    if (t != NULL)
    {
      t->attr.name = name;
      t->arr_size = atoi(tokenString);
    }
    match(NUM);
    match(RBRACKET);
  }
  else /* error */
  {    // TODO
  }

  match(SEMI);

  return t;
}

/* type-specifier → int | void */
static ExpType type_spec(void)
{
  switch (token)
  {
  case INT:
    token = getToken();
    return Integer;
  case VOID:
    token = getToken();
    return Void;
  default:
    syntaxError("unexpected token(type_spec) -> ");
    printToken(token, tokenString);
    token = getToken();
    return Void;
  }
}

/* fun-declaration → type-specifier ID ( params ) compound-stmt */
static TreeNode *fun_declare(void)
{
  TreeNode *t = newStmtNode(FuncDeclK);
  ExpType type = type_spec();
  char *name = copyString(tokenString);
  match(ID);
  match(LPAREN);
  t->child[0] = params();
  match(RPAREN);
  t->child[1] = compound_stmt();

  return t;
}

/* params → param-list | void */
static TreeNode *params(void)
{
  ExpType type;
  TreeNode *t;

  switch (token)
  {
  case VOID:
    t = newParamNode(Void);
    match(VOID);
    break;
  case INT: // TODO param-list
    break;
  default: // TODO Error
    break;
  }

  return t;
}

/* param-list → param-list , param | param */
static TreeNode *param_list(void)
{
  TreeNode *t = param();
  TreeNode *p = t;
  TreeNode *q;
  while (token == COMMA)
  {
    match(COMMA);
    q = param();
    if (q != NULL)
    {
      if (t == NULL)
        t = p = q;
      else /* now p cannot be NULL either */
      {
        p->sibling = q;
        p = q;
      }
    }
  }
  return t;
}

/* param → type-specifier ID | type-specifier ID [ ] */
static TreeNode *param(void)
{
  TreeNode *t;
  ExpType type = type_spec();
  char *name = copyString(tokenString);

  switch (token)
  {
    /* Var param */
  case COMMA:
  case RPAREN:
    t = newStmtNode(VarDeclK);
    break;

  /* Array param */
  case LBRACKET:
    t = newStmtNode(ArrayDeclK);
    break;
  default:
    // TODO Error
    break;
  }

  if (t != NULL)
  {
    t->attr.name = name;
    t->type = type;
    t->is_param = TRUE;
  }

  return t;
}

/* compound-stmt → { local-declarations statement-list } */
static TreeNode *compound_stmt(void)
{
  TreeNode *t = newStmtNode(CompoundK);
  match(LBRACE);
  t->child[0] = local_declare();
  // TODO statement-list
  match(RBRACE);
  return t;
}

/* local-declarations → local-declarations var-declaration | empty */
static TreeNode *local_declare(void)
{
  if (token == RBRACE) /* empty */
    return NULL;

  /* local-declarations var-declaration */
  TreeNode *t = var_declare(); /* var_declare() include SEMI(;). */
  TreeNode *p = t;

  while (token != RBRACE)
  {
    TreeNode *q;
    q = var_declare(); /* var_declare() include SEMI(;). */
    if (q != NULL)
    {
      if (t == NULL)
        t = p = q;
      else /* now p cannot be NULL either */
      {
        p->sibling = q;
        p = q;
      }
    }
  }
  return t;
}

/* statement-list → statement-list statement | empty */
static TreeNode *stmt_list(void)
{ // TODO
  return NULL;
}

/* statement → expression-stmt | compound-stmt | selection-stmt | iteration-stmt | return-stmt */
static TreeNode *stmt(void)
{ // TODO
  return NULL;
}

/* expression-stmt → expression ; | ; */
static TreeNode *exp_stmt(void)
{
  if (token == SEMI) /* ; */
    return NULL;

  /* expression ; */
  TreeNode *t = exp();
  return t;
}

/* selction-stmt → if ( expression ) statement | if ( expression ) statement else statement */
static TreeNode *select_stmt(void)
{ // TODO
  return NULL;
}

/* iteration-stmt → while ( expression ) statement */
static TreeNode *iter_stmt(void)
{ // TODO
  return NULL;
}

/* return-stmt → return ; | return expression ; */
static TreeNode *return_stmt(void)
{ // TODO
  return NULL;
}

/* expression → var = expression | simple-expression */
static TreeNode *exp(void)
{
  TreeNode *t = NULL;
  if (token == ID) /* var */
  {
    t = newStmtNode(AssignK);
    if (t != NULL)
      t->child[0] = call();
    match(ASSIGN);
    TreeNode *p = exp();
    if (t != NULL)
      t->child[1] = p;
  }
  else /* simple-expression */
    t = simple_exp();

  return t;
}

/* var → ID | ID [ expression ] */
/* call() 내부에 구현 */
// static TreeNode *var(void)
// {
//   TreeNode *t = newExpNode(IdK);
//   char *name = copyString(tokenString);
//   if (t != NULL)
//     t->attr.name = name;
//   match(ID);
//   if (token == LBRACKET)
//   {
//     match(LBRACKET);
//     TreeNode *p = exp();
//     if (t != NULL)
//     {
//       t->arr_size = p->attr.val;
//       t->child[0] = newArrSizeNode(t->arr_size);
//     }
//     match(RBRACKET);
//   }

//   return t;
// }

/* simple-expression → additive-expression relop additive-expression | additive-expression */
static TreeNode *simple_exp(void)
{
  TreeNode *t = add_exp();
  while (token == LT || token == LTEQ || token == GT || token == GTEQ || token == EQ || token == NOTEQ)
  {
    TreeNode *p = newExpNode(OpK);
    if (p != NULL)
    {
      p->child[0] = t;
      p->attr.op = token;
      t = p;
      match(token);
      p->child[1] = add_exp();
    }
  }
  return t;
}

/* additive-expression → additive-expression addop term | term */
static TreeNode *add_exp(void)
{
  TreeNode *t = term();
  while (token == PLUS || token == MINUS)
  {
    TreeNode *p = newExpNode(OpK);
    if (p != NULL)
    {
      p->child[0] = t;
      p->attr.op = token;
      t = p;
      match(token);
      p->child[1] = term();
    }
  }
  return t;
}

/* term → term mulop factor */
static TreeNode *term(void)
{
  TreeNode *t = factor();
  while (token == TIMES || token == OVER)
  {
    TreeNode *p = newExpNode(OpK);
    if (p != NULL)
    {
      p->child[0] = t;
      p->attr.op = token;
      t = p;
      match(token);
      p->child[1] = factor();
    }
  }
  return t;
}

/* factor → ( expression ) | var | call | NUM */
static TreeNode *factor(void)
{
  TreeNode *t = NULL;
  if (token == LPAREN) /* ( expression ) */
  {
    match(LPAREN);
    t = exp();
    match(RPAREN);
  }
  else if (token == ID) /* var | call */
  {                     /* var | array | func */
    t = call();
  }
  else if (token == NUM) /* NUM */
  {
    t = newExpNode(ConstK);
    if (t != NULL)
      t->attr.val = atoi(tokenString);
    match(NUM);
  }
  return t;
}

/* relop → <= | < | >= | > | == | != */
static TreeNode *relop(void)
{
  TreeNode *t = newExpNode(OpK);
  if (token == LTEQ || token == LT || token == GTEQ || token == GT || token == EQ || token == NOTEQ)
  {
    if (t != NULL)
      t->attr.op = token;
    match(token);
  }
  return t;
}

/* addop → + | - */
static TreeNode *addop(void)
{
  TreeNode *t = newExpNode(OpK);
  if (token == PLUS || token == MINUS)
  {
    if (t != NULL)
      t->attr.op = token;
    match(token);
  }
  return t;
}

/* mulop → * | / */
static TreeNode *mulop(void)
{
  TreeNode *t = newExpNode(OpK);
  if (token == TIMES || token == OVER)
  {
    if (t != NULL)
      t->attr.op = token;
    match(token);
  }
  return t;
}

/* call → ID ( args ) */
static TreeNode *call(void)
{
  TreeNode *t = NULL;
  char *name = copyString(tokenString);
  match(ID);
  switch (token)
  {
    /* Variable call. */
  case SEMI:
  case ASSIGN:
  case LT:
  case LTEQ:
  case GT:
  case GTEQ:
  case EQ:
  case NOTEQ:
    t = newExpNode(VarCallK);
    if (t != NULL)
      t->attr.name = name;
    break;

    /* Array call. */
  case LBRACKET:
    t = newExpNode(ArrayCallK);
    match(LBRACKET);
    if (t != NULL)
    {
      t->attr.name = name;
      t->arr_size = atoi(tokenString);
    }
    match(NUM);
    match(RBRACKET);
    break;

    /* Function call. */
  case LPAREN:
    // TODO
    break;
  default:
    // TODO
    break;
  }

  return t;
}

/* args → arg-list | empty */
static TreeNode *args(void)
{ // TODO
  return NULL;
}

/* arg-list → arg-list , expression | expression */
static TreeNode *arg_list(void)
{ // TODO
  return NULL;
}

/* A BNF for TINY. */
// static TreeNode * stmt_sequence(void);
// static TreeNode * statement(void);
// static TreeNode * if_stmt(void);
// static TreeNode * assign_stmt(void);
// static TreeNode * repeat_stmt(void);
// static TreeNode * read_stmt(void);
// static TreeNode * write_stmt(void);

// TreeNode * stmt_sequence(void)
// { TreeNode * t = statement();
//   TreeNode * p = t;
//   while ((token!=ENDFILE) && (token!=END) &&
//          (token!=ELSE) && (token!=UNTIL))
//   { TreeNode * q;
//     match(SEMI);
//     q = statement();
//     if (q!=NULL) {
//       if (t==NULL) t = p = q;
//       else /* now p cannot be NULL either */
//       { p->sibling = q;
//         p = q;
//       }
//     }
//   }
//   return t;
// }

// TreeNode * statement(void)
// { TreeNode * t = NULL;
//   switch (token) {
//     case IF : t = if_stmt(); break;
//     case REPEAT : t = repeat_stmt(); break;
//     case ID : t = assign_stmt(); break;
//     case READ : t = read_stmt(); break;
//     case WRITE : t = write_stmt(); break;
//     default : syntaxError("unexpected token -> ");
//               printToken(token,tokenString);
//               token = getToken();
//               break;
//   } /* end case */
//   return t;
// }

// TreeNode * if_stmt(void)
// { TreeNode * t = newStmtNode(IfK);
//   match(IF);
//   if (t!=NULL) t->child[0] = exp();
//   match(THEN);
//   if (t!=NULL) t->child[1] = stmt_sequence();
//   if (token==ELSE) {
//     match(ELSE);
//     if (t!=NULL) t->child[2] = stmt_sequence();
//   }
//   match(END);
//   return t;
// }

// TreeNode * repeat_stmt(void)
// { TreeNode * t = newStmtNode(RepeatK);
//   match(REPEAT);
//   if (t!=NULL) t->child[0] = stmt_sequence();
//   match(UNTIL);
//   if (t!=NULL) t->child[1] = exp();
//   return t;
// }

// TreeNode * assign_stmt(void)
// { TreeNode * t = newStmtNode(AssignK);
//   if ((t!=NULL) && (token==ID))
//     t->attr.name = copyString(tokenString);
//   match(ID);
//   match(ASSIGN);
//   if (t!=NULL) t->child[0] = exp();
//   return t;
// }

// TreeNode * read_stmt(void)
// { TreeNode * t = newStmtNode(ReadK);
//   match(READ);
//   if ((t!=NULL) && (token==ID))
//     t->attr.name = copyString(tokenString);
//   match(ID);
//   return t;
// }

// TreeNode * write_stmt(void)
// { TreeNode * t = newStmtNode(WriteK);
//   match(WRITE);
//   if (t!=NULL) t->child[0] = exp();
//   return t;
// }

// TreeNode * exp(void)
// { TreeNode * t = simple_exp();
//   if ((token==LT)||(token==EQ)) {
//     TreeNode * p = newExpNode(OpK);
//     if (p!=NULL) {
//       p->child[0] = t;
//       p->attr.op = token;
//       t = p;
//     }
//     match(token);
//     if (t!=NULL)
//       t->child[1] = simple_exp();
//   }
//   return t;
// }

// TreeNode * simple_exp(void)
// { TreeNode * t = term();
//   while ((token==PLUS)||(token==MINUS))
//   { TreeNode * p = newExpNode(OpK);
//     if (p!=NULL) {
//       p->child[0] = t;
//       p->attr.op = token;
//       t = p;
//       match(token);
//       t->child[1] = term();
//     }
//   }
//   return t;
// }

// TreeNode * term(void)
// { TreeNode * t = factor();
//   while ((token==TIMES)||(token==OVER))
//   { TreeNode * p = newExpNode(OpK);
//     if (p!=NULL) {
//       p->child[0] = t;
//       p->attr.op = token;
//       t = p;
//       match(token);
//       p->child[1] = factor();
//     }
//   }
//   return t;
// }

// TreeNode * factor(void)
// { TreeNode * t = NULL;
//   switch (token) {
//     case NUM :
//       t = newExpNode(ConstK);
//       if ((t!=NULL) && (token==NUM))
//         t->attr.val = atoi(tokenString);
//       match(NUM);
//       break;
//     case ID :
//       t = newExpNode(IdK);
//       if ((t!=NULL) && (token==ID))
//         t->attr.name = copyString(tokenString);
//       match(ID);
//       break;
//     case LPAREN :
//       match(LPAREN);
//       t = exp();
//       match(RPAREN);
//       break;
//     default:
//       syntaxError("unexpected token -> ");
//       printToken(token,tokenString);
//       token = getToken();
//       break;
//     }
//   return t;
// }

/****************************************/
/* the primary function of the parser   */
/****************************************/
/* Function parse returns the newly
 * constructed syntax tree
 */
TreeNode *parse(void)
{
  TreeNode *t;
  token = getToken();
  // t = stmt_sequence();
  t = declare_list();
  if (token != ENDFILE)
    syntaxError("parse(): Code ends before file\n");
  return t;
}
