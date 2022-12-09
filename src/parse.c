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
 * A BNF grammar for C- is as follows: (http://www.ii.uib.no/~wolter/teaching/h09-inf225/project/Syntax-C-Minus.pdf)
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
static TreeNode *declare_list(void);      /* declaration-list → declaration-list declaration | declaration */
static TreeNode *declare(void);           /* declaration → var-declaration | fun-declaration */
static TreeNode *var_declare(void);       /* var-declaration → type-specifier ID; | type-specifier ID [NUM]; */
static ExpType type_spec(void);           /* type-specifier → int | void */
static TreeNode *fun_declare(void);       /* fun-declaration → type-specifier ID ( params ) compound-stmt */
static TreeNode *params(void);            /* params → param-list | void */
static TreeNode *param_list(void);        /* param-list → param-list , param | param */
static TreeNode *param(void);             /* param → type-specifier ID | type-specifier ID [ ] */
static TreeNode *compound_stmt(void);     /* compound-stmt → { local-declarations statement-list } */
static TreeNode *local_declare(void);     /* local-declarations → local-declarations var-declaration | empty */
static TreeNode *stmt_list(void);         /* statement-list → statement-list statement | empty */
static TreeNode *stmt(void);              /* statement → expression-stmt | compound-stmt | selection-stmt | iteration-stmt | return-stmt */
static TreeNode *expr_stmt(void);         /* expression-stmt → expression ; | ; */
static TreeNode *select_stmt(void);       /* selction-stmt → if ( expression ) statement | if ( expression ) statement else statement */
static TreeNode *iter_stmt(void);         /* iteration-stmt → while ( expression ) statement */
static TreeNode *return_stmt(void);       /* return-stmt → return ; | return expression ; */
static TreeNode *expr(void);              /* expression → var = expression | simple-expression */
static TreeNode *var(void);               /* var → ID | ID [ expression ] */
static TreeNode *simple_expr(TreeNode *); /* simple-expression → additive-expression relop additive-expression | additive-expression */
static TreeNode *add_expr(TreeNode *);    /* additive-expression → additive-expression addop term | term */
static TreeNode *term(TreeNode *);        /* term → term mulop factor ( 수정: term → term mulop factor | factor ) */
static TreeNode *factor(TreeNode *);      /* factor → ( expression ) | var | call | NUM */
static TreeNode *relop(void);             /* relop → <= | < | >= | > | == | != */
static TreeNode *addop(void);             /* addop → + | - */
static TreeNode *mulop(void);             /* mulop → * | / */
static TreeNode *call(void);              /* call → ID ( args ) */
static TreeNode *args(void);              /* args → arg-list | empty */
static TreeNode *arg_list(void);          /* arg-list → arg-list , expression | expression */

/* consume COMMENT token & check next token */
static int check(TokenType);

/* is the TreeNode a function declaration? */
static int is_func_decl(TreeNode *);

/* is the TreeNode a function call? */
static int is_func_call(TreeNode *);

static void syntaxError(char *message)
{
  static int error_cnt = 0;
  if (++error_cnt > 8)
  {
    fprintf(stderr, "infinite loop\n");
    exit(-1);
  }
  fprintf(listing, "\n>>> ");
  fprintf(listing, "Syntax error at line %d: %s", lineno, message);
  Error = TRUE;
}

static void match(TokenType expected)
{
  if (check(expected))
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
/* consume COMMENT token & check next token */
static int check(TokenType expected)
{
  while (token == COMMENT)
    token = getToken();
  return token == expected;
}

/* is the TreeNode a function declaration? */
static int is_func_decl(TreeNode *t)
{
  return t != NULL &&
         t->nodekind == StmtK &&
         t->kind.stmt == FuncDeclK;
}

/* is the TreeNode a function call? */
static int is_func_call(TreeNode *t)
{
  return t != NULL &&
         t->nodekind == ExpK &&
         t->kind.exp == FuncCallK;
}

/* declaration-list → declaration-list declaration | declaration */
static TreeNode *declare_list(void)
{
  TreeNode *t = declare();
  TreeNode *p = t;

  /* FuncDecl is not followed by SEMI(;). */
  if (!is_func_decl(t))
    match(SEMI);

  while (check(ENDFILE) == FALSE)
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
  if (check(ENDFILE))
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
      t->child[0] = newTypeNode(IntegerArray);
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
    syntaxError("unexpected token ( declare() ) -> ");
    printToken(token, tokenString);
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

  switch (token)
  {
  case SEMI: /* var */
    t = newStmtNode(VarDeclK);
    if (t != NULL)
    {
      t->attr.name = name;
      t->child[0] = newTypeNode(type);
    }
    break;
  case LBRACKET: /* array */
    t = newStmtNode(ArrayDeclK);
    match(LBRACKET);
    if (t != NULL)
    {
      t->attr.name = name;
      t->arr_size = atoi(tokenString);
      t->child[0] = newTypeNode(IntegerArray);
      t->child[1] = newArrSizeNode(t->arr_size);
    }
    match(NUM);
    match(RBRACKET);
    break;
  default:
    syntaxError("unexpected token ( var_declare() ) -> ");
    printToken(token, tokenString);
    break;
  }

  match(SEMI);

  return t;
}

/* type-specifier → int | void */
static ExpType type_spec(void)
{
  check(token);
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
    // token = getToken();
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

  check(token);
  switch (token)
  {
  case VOID:
    match(VOID);
    t = newParamNode(Void);
    set_name(t, "void");
    break;
  case INT:
    t = param_list();
    break;
  default:
    syntaxError("unexpected token ( params() ) -> ");
    printToken(token, tokenString);
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
  while (check(COMMA))
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

/**
 * param → type-specifier ID | type-specifier ID [ ]
 * Here, type-specifier must be INT.
 */
static TreeNode *param(void)
{
  TreeNode *t;
  match(INT);
  char *name = copyString(tokenString);
  match(ID);

  switch (token)
  {
    /* Var param */
  case COMMA:
  case RPAREN:
    t = newParamNode(Integer);
    set_name(t, name);
    // if (t != NULL)
    //   t->attr.name = name;
    break;

  /* Array param */
  case LBRACKET:
    t = newParamNode(IntegerArray);
    set_name(t, name);
    // if (t != NULL)
    //   t->attr.name = name;
    match(LBRACKET);
    match(RBRACKET);
    break;
  default:
    syntaxError("unexpected token ( factor() ) -> ");
    printToken(token, tokenString);
    break;
  }

  if (t != NULL)
  {
    t->attr.name = name;
  }

  return t;
}

/* compound-stmt → { local-declarations statement-list } */
static TreeNode *compound_stmt(void)
{
  TreeNode *t = newStmtNode(CompoundK);
  match(LBRACE);
  t->child[0] = local_declare();
  t->child[1] = stmt_list();
  match(RBRACE);
  return t;
}

/* local-declarations → local-declarations var-declaration | empty */
static TreeNode *local_declare(void)
{
  if (check(INT) == FALSE && check(VOID) == FALSE) /* empty */
    return NULL;

  /* local-declarations var-declaration */
  TreeNode *t = var_declare();
  TreeNode *p = t;

  while (check(INT) || check(VOID))
  {
    TreeNode *q;
    q = var_declare();
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
{
  if (check(RBRACE)) /* empty */
    return NULL;

  TreeNode *t = stmt();
  TreeNode *p = t;

  while (check(RBRACE) == FALSE)
  {
    TreeNode *q;
    q = stmt();
    if (q != NULL)
    {
      if (t == NULL)
        t = p = q;
      else
      {
        p->sibling = q;
        p = q;
      }
    }
  }
  return t;
}

/* statement → expression-stmt | compound-stmt | selection-stmt | iteration-stmt | return-stmt */
static TreeNode *stmt(void)
{
  TreeNode *t = NULL;
  check(token);
  switch (token)
  {
  case LBRACE: /* compound-stmt */
    t = compound_stmt();
    break;
  case IF: /* selection-stmt */
    t = select_stmt();
    break;
  case WHILE: /* iteration-stmt */
    t = iter_stmt();
    break;
  case RETURN: /* return-stmt */
    t = return_stmt();
    break;
  default: /* expression-stmt */
    t = expr_stmt();
    break;
  }
  return t;
}

/* expression-stmt → expression ; | ; */
static TreeNode *expr_stmt(void)
{
  TreeNode *t = NULL;
  check(token);
  switch (token)
  {
  case SEMI: /* ; */
    match(SEMI);
    break;
  default: /* expression ; */
    t = expr();
    match(SEMI);
    break;
  }

  return t;
}

/* selction-stmt → if ( expression ) statement | if ( expression ) statement else statement */
static TreeNode *select_stmt(void)
{
  TreeNode *t = newStmtNode(IfK);
  match(IF);
  match(LPAREN);
  TreeNode *e = expr();
  match(RPAREN);
  TreeNode *s = stmt();

  if (t != NULL)
  {
    t->child[0] = e;
    t->child[1] = s;
  }

  if (check(ELSE))
  {
    TreeNode *q = newStmtNode(ElseK);
    match(ELSE);
    s = stmt();
    if (t != NULL)
      t->sibling = q;
    // t->child[2] = q;
    if (q != NULL)
      q->child[0] = s;
  }

  return t;
}

/* iteration-stmt → while ( expression ) statement */
static TreeNode *iter_stmt(void)
{
  TreeNode *t = newStmtNode(WhileK);
  match(WHILE);
  match(LPAREN);
  if (t != NULL)
    t->child[0] = expr();
  match(RPAREN);
  if (t != NULL)
    t->child[1] = stmt();

  return t;
}

/* return-stmt → return ; | return expression ; */
static TreeNode *return_stmt(void)
{
  TreeNode *t = newStmtNode(ReturnK);
  match(RETURN);
  switch (token)
  {
  case SEMI: /* return void */
    if (t != NULL)
      t->child[0] = newTypeNode(Void);
    break;
  default: /* return expression */
    if (t != NULL)
      t->child[0] = expr();
  }

  match(SEMI);
  return t;
}

/**
 * expression → var = expression | simple-expression
 */
static TreeNode *expr(void)
{
  TreeNode *t = NULL;

  if (check(ID))
  {
    /* In this case, "expression" starts with "var". */

    TreeNode *v = call();

    if (token == ASSIGN) /* var = expression */
    {
      if (is_func_call(v))
      {
        syntaxError("assign statement cannot start with func call.\n");
        fprintf(listing, "\t\tattempted to assign value to: %s()\n", v->attr.name);
      }
      t = newStmtNode(AssignK);
      match(ASSIGN);
      if (t != NULL)
        t->child[0] = v;

      TreeNode *q = expr();
      if (t != NULL)
        t->child[1] = q;
    }
    else /* "simple-expression" which starts with "ID". */
      t = simple_expr(v);
  }
  else /* "simple-expression" which does not start with "ID". */
    t = simple_expr(NULL);

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

/**
 * simple-expression → additive-expression relop additive-expression | additive-expression
 */
static TreeNode *simple_expr(TreeNode *start)
{
  check(token);

  TreeNode *t = newSimpleExpNode();
  int trim_flag = TRUE;
  if (t != NULL)
    t->child[0] = add_expr(start);

  if (is_relop(token))
  {
    trim_flag = FALSE;
    TreeNode *relop = newExpNode(OpK);
    if (relop != NULL)
      relop->attr.op = token;
    match(token);

    if (t != NULL)
    {
      t->child[1] = relop;
      t->child[2] = add_expr(NULL);
    }
  }

  if (trim_flag)
    t = t->child[0];

  return t;
}

/**
 * additive-expression → additive-expression addop term | term
 * t = newExpNode(AddExpK)로 새로운 additive-expression node를 생성하고,
 * t->child[0]에 term node를 추가한다.
 * 이후의 모든 addop, term node는 t->child[0]의 sibling으로 연결한다.
 */
static TreeNode *add_expr(TreeNode *start)
{
  check(token);

  TreeNode *t = newAddExpNode();
  TreeNode *term_tail = term(start);
  int trim_flag = TRUE;
  if (t != NULL)
    t->child[0] = term_tail;

  while (is_addop(token)) /* additive-expression addop term */
  {
    trim_flag = FALSE;
    TreeNode *op = addop();
    TreeNode *p = term(NULL);
    if (term_tail != NULL && op != NULL)
    {
      op->sibling = p;
      term_tail->sibling = op;
      term_tail = p; /* term_tail can be null. */
    }
  }

  if (trim_flag)
    t = t->child[0];

  return t;
}

/**
 * term → term mulop factor ( 수정: term → term mulop factor | factor )
 * t = newExpNode(TermK)로 새로운 term node를 생성하고,
 * t->child[0]에 factor node를 추가한다.
 * 이후의 모든 mulop, factor는 t->child[0]의 sibling으로 연결한다.
 */
static TreeNode *term(TreeNode *start)
{
  check(token);

  TreeNode *t = newExpNode(TermK);
  TreeNode *factor_tail = factor(start);
  int trim_flag = TRUE;
  if (t != NULL)
    t->child[0] = factor_tail;

  while (is_mulop(token))
  {
    trim_flag = FALSE;
    TreeNode *op = mulop();
    TreeNode *p = factor(NULL);
    if (factor_tail != NULL && op != NULL)
    {
      factor_tail->sibling = op;
      op->sibling = p;
      factor_tail = p; /* factor_tail can be NULL. */
    }
  }

  if (trim_flag)
    t = t->child[0];

  return t;
}

/**
 * factor → ( expression ) | var | call | NUM
 * if START != NULL, then tokens for "var | call" are already consumed(parsed).
 */
static TreeNode *factor(TreeNode *start)
{
  check(token);

  if (start != NULL) /* var | call */
    return start;

  TreeNode *t = NULL;
  switch (token)
  {
  case LPAREN: /* ( expression ) */
    match(LPAREN);
    t = expr();
    match(RPAREN);
    break;
  case NUM: /* NUM */
    t = newConstExpNode(atoi(tokenString));
    match(NUM);
    break;
  default:
    syntaxError("unexpected token ( factor() ) -> ");
    printToken(token, tokenString);
    break;
  }

  return t;
}

/* relop → <= | < | >= | > | == | != */
static TreeNode *relop(void)
{
  check(token);

  TreeNode *t = newExpNode(OpK);
  if (!is_relop(token))
    syntaxError("relop is expected.");
  else
  {
    if (t != NULL)
      t->attr.op = token;
  }

  match(token);
  return t;
}

/* addop → + | - */
static TreeNode *addop(void)
{
  check(token);

  TreeNode *t = newExpNode(OpK);
  if (!is_addop(token))
    syntaxError("addop is expected.");
  else
  {
    if (t != NULL)
      t->attr.op = token;
  }

  match(token);
  return t;
}

/* mulop → * | / */
static TreeNode *mulop(void)
{
  check(token);

  TreeNode *t = newExpNode(OpK);
  if (!is_mulop(token))
    syntaxError("relop is expected.");
  else
  {
    if (t != NULL)
      t->attr.op = token;
  }

  match(token);
  return t;
}

/**
 * call → ID ( args )
 * var → ID | ID [ expression ]
 *
 * However, call() must distinguish [var | array | func] call by checking token.
 * VarCallK | ArrayCallK | FuncCallK
 */
static TreeNode *call(void)
{
  TreeNode *t = NULL;
  char *name = copyString(tokenString);
  match(ID);

  switch (token)
  {
  case LPAREN: /* Function */
    t = newExpNode(FuncCallK);
    match(LPAREN);
    if (t != NULL)
    {
      t->attr.name = name;
      t->child[0] = args(); // TODO
    }
    match(RPAREN);
    break;
  case LBRACKET: /* Array */
    t = newExpNode(ArrayCallK);
    match(LBRACKET);
    if (t != NULL)
    {
      t->attr.name = name;
      t->child[0] = expr();
    }
    match(RBRACKET);
    break;
  default: /* Variable */
    t = newExpNode(VarCallK);
    if (t != NULL)
      t->attr.name = name;
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
