/****************************************************/
/* File: util.h                                     */
/* Utility functions for the TINY compiler          */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#ifndef _UTIL_H_
#define _UTIL_H_

/* Procedure printToken prints a token
 * and its lexeme to the listing file
 */
void printToken(TokenType, const char *);

/* Function newStmtNode creates a new statement
 * node for syntax tree construction
 */
TreeNode *newStmtNode(StmtKind);

/* Function newExpNode creates a new expression
 * node for syntax tree construction
 */
TreeNode *newExpNode(ExpKind);

/**
 * [HW2] Jiho Rhee
 */

/**
 * Create new node for TYPE keyword.
 * This node will be a child of VarDeclK or ArrayDeclK or FuncDeclK.
 */
TreeNode *newTypeNode(ExpType);

/**
 * Create new node for ARRAY SIZE.
 * This node will be a child of ArrayDeclK.
 */
TreeNode *newArrSizeNode(int);

/**
 * Create new node for PARAM.
 * This node will be a child of FuncDeclK, FuncCallK.
 */
TreeNode *newParamNode(ExpType);

/**
 * Check if given op is relop.
*/
int is_relop(TokenType);

/**
 * Check if given op is addop.
*/
int is_addop(TokenType);

/**
 * Check if given op is mulop.
*/
int is_mulop(TokenType);

/* Function copyString allocates and makes a new
 * copy of an existing string
 */
char *copyString(char *);

/* procedure printTree prints a syntax tree to the
 * listing file using indentation to indicate subtrees
 */
void printTree(TreeNode *);

#endif
