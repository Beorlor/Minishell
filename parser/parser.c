#include "parser.h"

ASTNode* createASTNode(NodeType type, char* value) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) {
        perror("Failed to allocate memory for ASTNode");
        exit(EXIT_FAILURE);
    }
    node->type = type;
    node->value = value;
    node->left = NULL;
    node->right = NULL;
    return node;
}

StartNode* createStartNode() {
    StartNode* startNode = (StartNode*)malloc(sizeof(StartNode));
    if (!startNode) {
        perror("Failed to allocate memory for StartNode");
        exit(EXIT_FAILURE);
    }
    startNode->children = NULL;
    startNode->childCount = 0;
    return startNode;
}

void addChildToStartNode(StartNode* startNode, ASTNode* child) {
    startNode->childCount++;
    startNode->children = realloc(startNode->children, sizeof(ASTNode*) * startNode->childCount);
    if (!startNode->children) {
        perror("Failed to reallocate memory for children in StartNode");
        exit(EXIT_FAILURE);
    }
    startNode->children[startNode->childCount - 1] = child;
}

void freeAST(ASTNode* node) {
    if (node) {
        freeAST(node->left);
        freeAST(node->right);
        free(node);
    }
}

void freeStartNode(StartNode* startNode) {
    for (int i = 0; i < startNode->childCount; i++) {
        freeAST(startNode->children[i]);
    }
    free(startNode->children);
    free(startNode);
}

void	free_lexer(Token **lexer)
{
	Token *current = *lexer;
	Token *next = (*lexer)->next;

	while (current->next)
	{
		next = current->next;
		free(current->value);
		free(current);
		current = next;
	}
	*lexer = NULL;
}

int main() {
	Token *lexer = lexer;


	free_lexer(&lexer);
    return 0;
}
