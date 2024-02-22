#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// BEHAVIOR : input a string and return a link list of t_token

typedef enum {
    TOKEN_LEFT_PAREN, // (
    TOKEN_RIGHT_PAREN, // )
    TOKEN_COMMAND, // Command name
    TOKEN_ARGUMENT, // Argument
    TOKEN_LOGICAL_AND, // &&
    TOKEN_LOGICAL_OR, // ||
    TOKEN_PIPE, // |
    TOKEN_REDIRECTION_IN, // <
    TOKEN_REDIRECTION_OUT, // >
    TOKEN_REDIRECTION_APPEND, // >>
    TOKEN_REDIRECTION_ERROR, // 2>
    TOKEN_HERE_DOCUMENT, // <<
	TOKEN_LIMITER, // Delimiter for here-document
    TOKEN_WILDCARD, // *
    TOKEN_ENV_VAR, // $VAR
    TOKEN_EXIT_STATUS, // $?
	TOKEN_STRING, // Generic string
    TOKEN_SINGLE_QUOTE_STRING, // '
    TOKEN_DOUBLE_QUOTE_STRING // "
} TokenType;

// pas encore tout definie
// si on rencontre "$PATH"
// on fait un token DOUBLE QUOTE avec le string dedans et on vera plus tard pour la gestion (il faudra reanaliser le string)
// ou alors on utilise double quote comme un indicateur et on le stock comme : TOKEN_DOUBLE_QUOTE_STRING / TOKEN_ENV_VAR / TOKEN_DOUBLE_QUOTE_STRING

typedef struct Token {
    TokenType type;
    char* value;
    struct Token* next;
} Token;

// Fonction pour créer un nouveau token
Token* createToken(TokenType type, const char* value) {
    Token* token = (Token*)malloc(sizeof(Token));
    token->type = type;
    token->value = value != NULL ? strdup(value) : NULL;
    token->next = NULL;
    return token;
}

// Fonction pour ajouter un token à la fin de la liste
void appendToken(Token** head, TokenType type, const char* value) {
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

// Fonction pour libérer la liste des tokens
void freeTokens(Token* head) {
    Token* tmp;
    while (head != NULL) {
        tmp = head;
        head = head->next;
        if (tmp->value != NULL) free(tmp->value);
        free(tmp);
    }
}

// Fonction pour afficher les tokens (pour démonstration)
void printTokens(const Token* head) {
    while (head != NULL) {
        printf("Token Type: %d, Value: %s\n", head->type, head->value ? head->value : "NULL");
        head = head->next;
    }
}

int main() {
    Token* head = NULL;

    // Construction de la liste des tokens
    appendToken(&head, TOKEN_LEFT_PAREN, NULL);
    appendToken(&head, TOKEN_COMMAND, "cat");
    appendToken(&head, TOKEN_ARGUMENT, "file1.txt");
    // Continuer à ajouter les tokens...

    // Afficher les tokens
    printTokens(head);

    // Libérer la liste des tokens
    freeTokens(head);

    return 0;
}
