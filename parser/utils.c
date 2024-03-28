#include "parser.h"

// Function to get the string representation of the node type
const char* getNodeTypeString(NodeType type) {
    switch (type) {
        case NODE_COMMAND: return "COMMAND";
		case NODE_PARENTHESE : return "PARENTHESE";
        case NODE_LOGICAL_AND: return "LOGICAL_AND";
        case NODE_LOGICAL_OR: return "LOGICAL_OR";
        case NODE_PIPE: return "PIPE";
        case NODE_LOGICAL_HOLDER: return "LOGICAL_HOLDER";
        default: return "UNKNOWN";
    }
}


void printAST(const ASTNode* node) {
    if (node == NULL) {
        return;
    }
    printAST(node->left);

    // Print the command or logical operation
    printf("%s", getNodeTypeString(node->type));
    if (node->type == NODE_COMMAND || node->type == NODE_PARENTHESE) {
        printf(" (Value: %s", node->value);
        if (node->Input) printf(", Input: %s", node->Input);
        if (node->Output) printf(", Output: %s", node->Output);
        if (node->Append) printf(", Append: %s", node->Append);
        printf(")");
    }
    printf("\n");

    printAST(node->right);
}

void printLogicalSubtrees(const StartNode* startNode) {
    if (startNode == NULL || startNode->children == NULL) {
        return;
    }

    // Iterate over each logical node and print its subtrees and the node itself.
    for (int i = 0; i < startNode->childCount; ++i) {
        LogicalNode* logicalNode = startNode->children[i];

        printf("Subtree %d:\n", i + 1);

        // Print the left subtree of the current logical node.
        if (logicalNode->left) {
            printf("Left Subtree of Logical Node %d:\n", i + 1);
            printAST(logicalNode->left);
        }

        // Print the logical node itself if it's not a HOLDER (which serves as a placeholder for non-logical structures).
        if (logicalNode->type != NODE_LOGICAL_HOLDER) {
            printf("Logical Node %d: %s\n", i + 1, getNodeTypeString(logicalNode->type));
        }

        // Print the right subtree of the current logical node, if there's another logical operation, it links to the next subtree.
        if (logicalNode->right) {
            printf("Right Subtree of Logical Node %d:\n", i + 1);
            printAST(logicalNode->right);
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
