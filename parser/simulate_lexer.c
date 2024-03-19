#include "parser.h"

Token* createToken(TokenType type, char* value) {
    Token* newToken = (Token*)malloc(sizeof(Token));
    if (!newToken) {
        perror("Failed to allocate memory for a new token");
        exit(EXIT_FAILURE);
    }
    newToken->type = type;
    newToken->value = value;
    newToken->next = NULL;
    return newToken;
}

void appendToken(Token** head, TokenType type, char* value) {
    Token* newToken = createToken(type, value);
    if (*head == NULL) {
        *head = newToken;
    } else {
        Token* current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newToken;
    }
}

const char* getTokenTypeName(TokenType type) {
    switch (type) {
        case TOKEN_PAREN: return "TOKEN_PAREN";
        case TOKEN_COMMAND: return "TOKEN_COMMAND";
        case TOKEN_LOGICAL_AND: return "TOKEN_LOGICAL_AND";
        case TOKEN_LOGICAL_OR: return "TOKEN_LOGICAL_OR";
        case TOKEN_PIPE: return "TOKEN_PIPE";
        case TOKEN_REDIRECTION_IN: return "TOKEN_REDIRECTION_IN";
        case TOKEN_REDIRECTION_OUT: return "TOKEN_REDIRECTION_OUT";
        case TOKEN_REDIRECTION_APPEND: return "TOKEN_REDIRECTION_APPEND";
        case TOKEN_WILDCARD: return "TOKEN_WILDCARD";
        default: return "UNKNOWN_TOKEN";
    }
}

void printTokens(Token* head) {
    while (head != NULL) {
        printf("Type: %s, Value: %s\n", getTokenTypeName(head->type), head->value);
        head = head->next;
    }
	printf("\n");
}

Token *lexer() {
    Token* tokens = NULL;

    // Simulate tokens for "ls -l | grep "txt" > files.txt"
    appendToken(&tokens, TOKEN_COMMAND, "ls -l");
    appendToken(&tokens, TOKEN_PIPE, "|");
    appendToken(&tokens, TOKEN_COMMAND, "grep \"txt\"");
    appendToken(&tokens, TOKEN_REDIRECTION_OUT, "files1.txt");
	appendToken(&tokens, TOKEN_LOGICAL_AND, "&&");
	appendToken(&tokens, TOKEN_REDIRECTION_IN, "files2.txt");
	appendToken(&tokens, TOKEN_LOGICAL_OR, "||");
	appendToken(&tokens, TOKEN_REDIRECTION_APPEND, "files3.txt");

    printTokens(tokens);

    // Free the allocated memory (not shown for brevity)

    return (tokens);
}
