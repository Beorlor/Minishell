#include "parser.h"

#include "parser.h"

// Helper function to print redirections nicely
void printRedirections(const Redirection* redir) {
    const char* sep = "";
    while (redir) {
        printf("%s%s", sep, redir->filename);
        sep = ", ";
        redir = redir->next;
    }
}

// Recursive function to print the AST nodes with indentation for better visualization
void printAST(const ASTNode* node, int level) {
    if (!node) return;

    // Print right subtree first (for right-to-left visual structure)
    printAST(node->right, level + 1);

    // Print spaces for current level indentation
    for (int i = 0; i < level; i++) printf("    ");

    // Print the node type and any associated values or redirections
    printf("%s", getNodeTypeString(node->type));
    if (node->type == NODE_COMMAND || node->type == NODE_PARENTHESE) {
        printf(": %s", node->value ? node->value : "NULL");
        if (node->inputs || node->outputs || node->appends) {
            printf(" [");
            if (node->inputs) {
                printf("In: ");
                printRedirections(node->inputs);
            }
            if (node->outputs) {
                printf("; Out: ");
                printRedirections(node->outputs);
            }
            if (node->appends) {
                printf("; Append: ");
                printRedirections(node->appends);
            }
            printf("]");
        }
    }
    printf("\n");

    // Print left subtree last (to maintain the correct order visually)
    printAST(node->left, level + 1);
}

// Central function to print the entire AST, including logical nodes
void printEntireAST(const StartNode* startNode) {
    if (!startNode || !startNode->children) {
        printf("No AST data available to display.\n");
        return;
    }

    printf("Complete Abstract Syntax Tree:\n");
    for (int i = 0; i < startNode->childCount; ++i) {
        LogicalNode* logicalNode = startNode->children[i];
        printf("Subtree %d:\n", i + 1);
        if (logicalNode->left) {
            printf("Left:\n");
            printAST(logicalNode->left, 1);
        }
        if (logicalNode->type != NODE_LOGICAL_HOLDER) {
            printf("Logical Node (%s):\n", getNodeTypeString(logicalNode->type));
        }
        if (logicalNode->right) {
            printf("Right:\n");
            printAST(logicalNode->right, 1);
        }
    }
}

void	free_lexer(Token **lexer)
{
	Token *current = *lexer;
	Token *next = (*lexer)->next;

	while (current->next)
	{
		next = current->next;
		//free(current->value);
		free(current);
		current = next;
	}
	*lexer = NULL;
}

void freeRedirectionList(Redirection** list) {
    if (list == NULL || *list == NULL) {
        return; // Safety check if list is NULL or already empty
    }

    Redirection* current = *list;
    Redirection* next = NULL;

    while (current != NULL) {
        next = current->next;
        free(current->filename); // Free the filename string allocated with strdup
        free(current); // Free the redirection node itself
        current = next;
    }

    *list = NULL; // Set the list pointer to NULL after freeing
}
