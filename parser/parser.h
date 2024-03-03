#ifndef PARSER
# define PARSER

# include <stdio.h>
# include <stdlib.h>

// lexer
typedef enum {
    TOKEN_PAREN_OPEN,
    TOKEN_PAREN_CLOSE,
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

typedef enum {
    NODE_COMMAND,
    NODE_LOGICAL_AND,
    NODE_LOGICAL_OR,
    NODE_PIPE,
    NODE_PARENTHESE, // For future use to handle grouped commands
} NodeType;

typedef struct ASTNode {
    NodeType type;
    char* value; // Stores the command or argument
    struct ASTNode* left; // Used for binary trees (commands and pipes)
    struct ASTNode* right; // Chaining arguments or operations
} ASTNode;

// change start node to match all the need (tablette jb)
typedef struct StartNode {
    struct ASTNode** children; // Array of pointers to child nodes (logical AND/OR nodes)
    int childCount; // Number of children
} StartNode;

#endif
