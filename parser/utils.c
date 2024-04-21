#include "parser.h"

#include "parser.h"

// Helper function to print redirection lists
void printRedirections(const Redirection* redir) {
    const char* sep = "";
    while (redir) {
        printf("%s%s", sep, redir->filename);
        sep = ", ";
        redir = redir->next;
    }
}

// Function to get a string representation of the node type
const char* getNodeTypeString(NodeType type) {
    switch (type) {
        case NODE_COMMAND: return "COMMAND";
        case NODE_PARENTHESE: return "PARENTHESE";
        case NODE_EMPTY_COMMAND: return "EMPTY COMMAND";
        case NODE_LOGICAL_AND: return "LOGICAL AND";
        case NODE_LOGICAL_OR: return "LOGICAL OR";
        case NODE_PIPE: return "PIPE";
        case NODE_LOGICAL_HOLDER: return "LOGICAL HOLDER";
        default: return "UNKNOWN";
    }
}

// Recursive function to print the AST nodes with indentation for better visualization
void printAST(const ASTNode* node, int level) {
    if (!node) return;

    // Indentation for current level
    char indent[4 * level + 1];
    memset(indent, ' ', 4 * level);
    indent[4 * level] = '\0';

    // Print left subtree first
    printAST(node->left, level + 1);

    // Print the current node
    printf("%s%s", indent, getNodeTypeString(node->type));
    if (node->value) {
        printf(": %s", node->value);
    }
    if (node->inputs || node->outputs || node->appends || node->here_doc) {
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
        if (node->here_doc) {
            printf("; HereDoc: ");
            printRedirections(node->here_doc);
        }
        printf("]");
    }
    printf("\n");

    // Print right subtree last
    printAST(node->right, level + 1);
}


// Function to print the entire AST including logical nodes
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

// !!!!!!!!!!!!!!!!!!!!!!!!!!! need to free the filename
void freeRedirectionList(Redirection** list) {
    if (list == NULL || *list == NULL) {
        return; // Safety check if list is NULL or already empty
    }

    Redirection* current = *list;
    Redirection* next = NULL;

    while (current != NULL) {
        next = current->next;
        //free(current->filename); // Free the filename string allocated with strdup
        free(current); // Free the redirection node itself
        current = next;
    }

    *list = NULL; // Set the list pointer to NULL after freeing
}
