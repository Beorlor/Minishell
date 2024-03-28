#ifndef PARSER
# define PARSER

# include <stdio.h>
# include <stdlib.h>
# include <stdbool.h>

// lexer
typedef enum {
    TOKEN_PAREN,
    TOKEN_COMMAND,
    TOKEN_LOGICAL_AND,
    TOKEN_LOGICAL_OR,
    TOKEN_PIPE,
    TOKEN_REDIRECTION_IN,
    TOKEN_REDIRECTION_OUT,
    TOKEN_REDIRECTION_APPEND,
    TOKEN_WILDCARD,
} TokenType;

typedef struct Token {
    TokenType type;
    char* value;
    struct Token* next;
} Token;

// parser
Token *lexer();

// Define node types
typedef enum {
    NODE_COMMAND,
	NODE_PARENTHESE,
    NODE_LOGICAL_AND,
    NODE_LOGICAL_OR,
    NODE_PIPE,
    NODE_LOGICAL_HOLDER, // Holder for logical structure
} NodeType;

// Define ASTNode structure
typedef struct ASTNode {
    NodeType type;
    char *value; // Stores the command or argument
    struct ASTNode *left; // Used for binary trees (commands and pipes)
    struct ASTNode *right; // Chaining arguments or operations
	char *Input;
	char *Output;
	char *Append;
} ASTNode;

typedef struct LogicalNode {
    NodeType type; // NODE_LOGICAL_AND, NODE_LOGICAL_OR, or NODE_LOGICAL_HOLDER
    struct ASTNode *left; // Left subtree (commands and pipes)
    struct ASTNode *right; // Right subtree (commands and pipes)
} LogicalNode;

// Define StartNode structure
typedef struct StartNode {
    LogicalNode **children; // Array of pointers to child nodes (logical AND/OR nodes)
    int childCount; // Number of children
    bool hasLogical; // Boolean to indicate if there are logical nodes
} StartNode;

#endif
