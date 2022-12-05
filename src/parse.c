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

/* function prototypes for recursive calls */
/**
 * [HW2] Jiho Rhee
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
static TreeNode *decl_list(void);
static TreeNode *decl(void);
static TreeNode *var_decl(void);
static ExpType type_spec(void);
static TreeNode *params(void);
static TreeNode *param_list(ExpType type);
static TreeNode *param(ExpType type);
static TreeNode *comp_stmt(void);
static TreeNode *local_decl(void);
static TreeNode *stmt_list(void);
static TreeNode *stmt(void);
static TreeNode *expr_stmt(void);
static TreeNode *sel_stmt(void);
static TreeNode *iter_stmt(void);
static TreeNode *ret_stmt(void);
static TreeNode *expr(void);
static TreeNode *simp_expr(TreeNode *f);
static TreeNode *add_expr(TreeNode *f);
static TreeNode *term(TreeNode *f);
static TreeNode *factor(TreeNode *f);
static TreeNode *call(void);
static TreeNode *args(void);
static TreeNode *args_list(void);

/* Parse rule for TINY. */
// static TreeNode * stmt_sequence(void);
// static TreeNode * statement(void);
// static TreeNode * if_stmt(void);
// static TreeNode * repeat_stmt(void);
// static TreeNode * assign_stmt(void);
// static TreeNode * read_stmt(void);
// static TreeNode * write_stmt(void);
// static TreeNode * exp(void);
// static TreeNode * simple_exp(void);
// static TreeNode * term(void);
// static TreeNode * factor(void);

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
    syntaxError("unexpected token -> ");
    printToken(token, tokenString);
    fprintf(listing, "      ");
  }
}

TreeNode *decl_list(void)
{
  TreeNode *t = decl();
  TreeNode *p = t;
  while (token != ENDFILE)
  {
    TreeNode *q;
    q = decl();
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

TreeNode *decl(void)
{
  TreeNode *t;
  ExpType type;
  char *name;

  type = type_spec();
  name = copyString(tokenString);
  match(ID);

  switch (token)
  {
  case SEMI: /* ; */
    t = newExpNode(VarDeclK);
    if (t != NULL)
    {
      t->attr.name = name;
      t->type = type;
    }
    match(SEMI);
    break;
  case LBRACKET: /* [ */
    t = newExpNode(VarArrayDeclK);
    if (t != NULL)
    {
      t->attr.name = name;
      t->type = type;
    }
    match(LBRACKET);
    if (t != NULL)
      t->arr_size = atoi(tokenString);
    match(NUM);
    match(RBRACKET);
    match(SEMI);
    break;
  case LPAREN: /* ( */
    t = newExpNode(FuncDeclK);
    if (t != NULL)
    {
      t->attr.name = name;
      t->type = type;
    }
    match(LPAREN);
    if (t != NULL)
      t->child[0] = params();
    match(RPAREN);
    if (t != NULL)
      t->child[1] = comp_stmt();
    break;
  default:
    syntaxError("unexpected token (decl) -> ");
    printToken(token, tokenString);
    token = getToken();
    break;
  }
  return t;
}

TreeNode *var_decl(void)
{
  TreeNode *t;
  ExpType type;
  char *name;

  type = type_spec();
  name = copyString(tokenString);
  match(ID);
  switch (token)
  {
  case SEMI:
    t = newExpNode(VarDeclK);
    if (t != NULL)
    {
      t->attr.name = name;
      t->type = type;
    }
    match(SEMI);
    break;
  case LBRACKET:
    t = newExpNode(VarArrayDeclK);
    if (t != NULL)
    {
      t->attr.name = name;
      t->type = type;
    }
    match(LBRACKET);
    if (t != NULL)
      t->arr_size = atoi(tokenString);
    match(NUM);
    match(RBRACKET);
    match(SEMI);
    break;
  default:
    syntaxError("unexpected token(var_decl) -> ");
    printToken(token, tokenString);
    token = getToken();
    break;
  }
  return t;
}

ExpType type_spec(void)
{
  fprintf(listing, "token index: %d\n", token); /* DEBUG */
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

TreeNode *params(void)
{
  ExpType type;
  TreeNode *t;

  type = type_spec();
  if (type == Void && token == RPAREN)
  {
    t = newExpNode(VarDeclK);
    t->is_param = TRUE;
    t->type = Void;
  }
  else
    t = param_list(type);
  return t;
}

TreeNode *param_list(ExpType type)
{
  TreeNode *t = param(type);
  TreeNode *p = t;
  TreeNode *q;
  while (token == COMMA)
  {
    match(COMMA);
    q = param(type_spec());
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

TreeNode *param(ExpType type)
{
  TreeNode *t;
  char *name;

  name = copyString(tokenString);
  match(ID);
  if (token == LBRACKET)
  {
    match(LBRACKET);
    match(RBRACKET);
    t = newExpNode(VarArrayDeclK);
  }
  else
    t = newExpNode(VarDeclK);
  if (t != NULL)
  {
    t->attr.name = name;
    t->type = type;
    t->is_param = TRUE;
  }
  return t;
}

TreeNode *comp_stmt(void)
{
  TreeNode *t = newStmtNode(CompStmtK);
  match(LBRACE);
  t->child[0] = local_decl();
  t->child[1] = stmt_list();
  match(RBRACE);
  return t;
}

TreeNode *local_decl(void)
{
  TreeNode *t = NULL;
  TreeNode *p;

  if (token == INT || token == VOID)
    t = var_decl();
  p = t;
  if (t != NULL)
  {
    while (token == INT || token == VOID)
    {
      TreeNode *q;
      q = var_decl();
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
  }
  return t;
}

TreeNode *stmt_list(void)
{
  TreeNode *t;
  TreeNode *p;

  if (token == RBRACE)
    return NULL;
  t = stmt();
  p = t;
  while (token != RBRACE)
  {
    TreeNode *q;
    q = stmt();
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

TreeNode *stmt(void)
{
  TreeNode *t;
  switch (token)
  {
  case LBRACE:
    t = comp_stmt();
    break;
  case IF:
    t = sel_stmt();
    break;
  case WHILE:
    t = iter_stmt();
    break;
  case RETURN:
    t = ret_stmt();
    break;
  case ID:
  case LPAREN:
  case NUM:
  case SEMI:
    t = expr_stmt();
    break;
  default:
    syntaxError("unexpected token(stmt) -> ");
    printToken(token, tokenString);
    token = getToken();
    return Void;
  }
  return t;
}

TreeNode *expr_stmt(void)
{
  TreeNode *t;

  if (token == SEMI)
    match(SEMI);
  else if (token != RBRACE)
  {
    t = expr();
    match(SEMI);
  }
  return t;
}

TreeNode *sel_stmt(void)
{
  TreeNode *t = newStmtNode(SelStmtK);

  match(IF);
  match(LPAREN);
  if (t != NULL)
    t->child[0] = expr();
  match(RPAREN);
  if (t != NULL)
    t->child[1] = stmt();
  if (token == ELSE)
  {
    match(ELSE);
    if (t != NULL)
      t->child[2] = stmt();
  }

  return t;
}

TreeNode *iter_stmt(void)
{
  TreeNode *t = newStmtNode(IterStmtK);

  match(WHILE);
  match(LPAREN);
  if (t != NULL)
    t->child[0] = expr();
  match(RPAREN);
  if (t != NULL)
    t->child[1] = stmt();
  return t;
}

TreeNode *ret_stmt(void)
{
  TreeNode *t = newStmtNode(RetStmtK);

  match(RETURN);
  if (token != SEMI && t != NULL)
    t->child[0] = expr();
  match(SEMI);
  return t;
}

TreeNode *expr(void)
{
  TreeNode *t;
  TreeNode *q = NULL;
  int flag = FALSE;

  if (token == ID)
  {
    q = call();
    flag = TRUE;
  }

  if (flag == TRUE && token == ASSIGN)
  {
    if (q != NULL && q->nodekind == ExpK && q->kind.exp == IdK)
    {
      match(ASSIGN);
      t = newExpNode(AssignK);
      if (t != NULL)
      {
        t->child[0] = q;
        t->child[1] = expr();
      }
    }
    else
    {
      syntaxError("attempt to assign to something not an lvalue\n");
      token = getToken();
    }
  }
  else
    t = simp_expr(q);
  return t;
}

TreeNode *simp_expr(TreeNode *f)
{
  TreeNode *t, *q;
  TokenType oper;
  q = add_expr(f);
  if (token == LT || token == LTEQ || token == GT || token == GTEQ || token == EQ || token == NOTEQ)
  {
    oper = token;
    match(token);
    t = newExpNode(OpK);
    if (t != NULL)
    {
      t->child[0] = q;
      t->child[1] = add_expr(NULL);
      t->attr.op = oper;
    }
  }
  else
    t = q;
  return t;
}

TreeNode *add_expr(TreeNode *f)
{
  TreeNode *t;
  TreeNode *q;

  t = term(f);
  if (t != NULL)
  {
    while (token == PLUS || token == MINUS)
    {
      q = newExpNode(OpK);
      if (q != NULL)
      {
        q->child[0] = t;
        q->attr.op = token;
        t = q;
        match(token);
        t->child[1] = term(NULL);
      }
    }
  }
  return t;
}

TreeNode *term(TreeNode *f)
{
  TreeNode *t;
  TreeNode *q;

  t = factor(f);
  if (t != NULL)
  {
    while (token == TIMES || token == OVER)
    {
      q = newExpNode(OpK);
      if (q != NULL)
      {
        q->child[0] = t;
        q->attr.op = token;
        t = q;
        match(token);
        t->child[1] = factor(NULL);
      }
    }
  }
  return t;
}

TreeNode *factor(TreeNode *f)
{
  TreeNode *t;

  if (f != NULL)
    return f;

  switch (token)
  {
  case LPAREN:
    match(LPAREN);
    t = expr();
    match(RPAREN);
    break;
  case ID:
    t = call();
    break;
  case NUM:
    t = newExpNode(ConstK);
    if (t != NULL)
    {
      t->attr.val = atoi(tokenString);
      t->type = Integer;
    }
    match(NUM);
    break;
  default:
    syntaxError("unexpected token(factor) -> ");
    printToken(token, tokenString);
    token = getToken();
    return Void;
  }
  return t;
}

TreeNode *call(void)
{
  TreeNode *t;
  char *name;

  if (token == ID)
    name = copyString(tokenString);
  match(ID);

  if (token == LPAREN)
  {
    match(LPAREN);
    t = newStmtNode(CallK);
    if (t != NULL)
    {
      t->attr.name = name;
      t->child[0] = args();
    }
    match(RPAREN);
  }
  else if (token == LBRACKET)
  {
    t = newExpNode(IdK);
    if (t != NULL)
    {
      t->attr.name = name;
      t->type = Integer;
      match(LBRACKET);
      t->child[0] = expr();
      match(RBRACKET);
    }
  }
  else
  {
    t = newExpNode(IdK);
    if (t != NULL)
    {
      t->attr.name = name;
      t->type = Integer;
    }
  }
  return t;
}

TreeNode *args(void)
{
  if (token == RPAREN)
    return NULL;
  else
    return args_list();
}

TreeNode *args_list(void)
{
  TreeNode *t;
  TreeNode *p;

  t = expr();
  p = t;
  if (t != NULL)
  {
    while (token == COMMA)
    {
      match(COMMA);
      TreeNode *q = expr();
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
  }
  return t;
}

// TreeNode *stmt_sequence(void)
// {
//   TreeNode *t = statement();
//   TreeNode *p = t;
//   while ((token != ENDFILE) && (token != END) &&
//          (token != ELSE) && (token != UNTIL))
//   {
//     TreeNode *q;
//     match(SEMI);
//     q = statement();
//     if (q != NULL)
//     {
//       if (t == NULL)
//         t = p = q;
//       else /* now p cannot be NULL either */
//       {
//         p->sibling = q;
//         p = q;
//       }
//     }
//   }
//   return t;
// }

// TreeNode *statement(void)
// {
//   TreeNode *t = NULL;
//   switch (token)
//   {
//   case IF:
//     t = if_stmt();
//     break;
//   case REPEAT:
//     t = repeat_stmt();
//     break;
//   case ID:
//     t = assign_stmt();
//     break;
//   case READ:
//     t = read_stmt();
//     break;
//   case WRITE:
//     t = write_stmt();
//     break;
//   default:
//     syntaxError("unexpected token -> ");
//     printToken(token, tokenString);
//     token = getToken();
//     break;
//   } /* end case */
//   return t;
// }

// TreeNode *if_stmt(void)
// {
//   TreeNode *t = newStmtNode(IfK);
//   match(IF);
//   if (t != NULL)
//     t->child[0] = exp();
//   match(THEN);
//   if (t != NULL)
//     t->child[1] = stmt_sequence();
//   if (token == ELSE)
//   {
//     match(ELSE);
//     if (t != NULL)
//       t->child[2] = stmt_sequence();
//   }
//   match(END);
//   return t;
// }

// TreeNode *repeat_stmt(void)
// {
//   TreeNode *t = newStmtNode(RepeatK);
//   match(REPEAT);
//   if (t != NULL)
//     t->child[0] = stmt_sequence();
//   match(UNTIL);
//   if (t != NULL)
//     t->child[1] = exp();
//   return t;
// }

// TreeNode *assign_stmt(void)
// {
//   TreeNode *t = newStmtNode(AssignK);
//   if ((t != NULL) && (token == ID))
//     t->attr.name = copyString(tokenString);
//   match(ID);
//   match(ASSIGN);
//   if (t != NULL)
//     t->child[0] = exp();
//   return t;
// }

// TreeNode *read_stmt(void)
// {
//   TreeNode *t = newStmtNode(ReadK);
//   match(READ);
//   if ((t != NULL) && (token == ID))
//     t->attr.name = copyString(tokenString);
//   match(ID);
//   return t;
// }

// TreeNode *write_stmt(void)
// {
//   TreeNode *t = newStmtNode(WriteK);
//   match(WRITE);
//   if (t != NULL)
//     t->child[0] = exp();
//   return t;
// }

// TreeNode *exp(void)
// {
//   TreeNode *t = simple_exp();
//   if ((token == LT) || (token == EQ))
//   {
//     TreeNode *p = newExpNode(OpK);
//     if (p != NULL)
//     {
//       p->child[0] = t;
//       p->attr.op = token;
//       t = p;
//     }
//     match(token);
//     if (t != NULL)
//       t->child[1] = simple_exp();
//   }
//   return t;
// }

// TreeNode *simple_exp(void)
// {
//   TreeNode *t = term();
//   while ((token == PLUS) || (token == MINUS))
//   {
//     TreeNode *p = newExpNode(OpK);
//     if (p != NULL)
//     {
//       p->child[0] = t;
//       p->attr.op = token;
//       t = p;
//       match(token);
//       t->child[1] = term();
//     }
//   }
//   return t;
// }

// TreeNode *term(void)
// {
//   TreeNode *t = factor();
//   while ((token == TIMES) || (token == OVER))
//   {
//     TreeNode *p = newExpNode(OpK);
//     if (p != NULL)
//     {
//       p->child[0] = t;
//       p->attr.op = token;
//       t = p;
//       match(token);
//       p->child[1] = factor();
//     }
//   }
//   return t;
// }

// TreeNode *factor(void)
// {
//   TreeNode *t = NULL;
//   switch (token)
//   {
//   case NUM:
//     t = newExpNode(ConstK);
//     if ((t != NULL) && (token == NUM))
//       t->attr.val = atoi(tokenString);
//     match(NUM);
//     break;
//   case ID:
//     t = newExpNode(IdK);
//     if ((t != NULL) && (token == ID))
//       t->attr.name = copyString(tokenString);
//     match(ID);
//     break;
//   case LPAREN:
//     match(LPAREN);
//     t = exp();
//     match(RPAREN);
//     break;
//   default:
//     syntaxError("unexpected token -> ");
//     printToken(token, tokenString);
//     token = getToken();
//     break;
//   }
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
  fprintf(listing, "token index: %d\n", token); /* DEBUG */
  t = decl_list();
  if (token != ENDFILE)
    syntaxError("Code ends before file\n");
  return t;
}
