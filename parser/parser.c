 #include "parser.h"

// Function to count the number of logical nodes (&& and ||)
int countLogicalNodes(Token* tokens) {
    int count = 0;
    Token* current = tokens;
    while (current != NULL) {
        if (current->type == TOKEN_LOGICAL_AND || current->type == TOKEN_LOGICAL_OR) {
            count++;
        }
        current = current->next;
    }
    return count;
}

// Function to create a StartNode and allocate memory for logical nodes if present
StartNode* createAndSetupStartNode(Token* tokens) {
    StartNode* startNode = (StartNode*)malloc(sizeof(StartNode));
    if (!startNode) {
        perror("Failed to allocate memory for StartNode");
        exit(EXIT_FAILURE);
    }

    // Initially set the hasLogical flag to false and no children
    startNode->hasLogical = false;
    startNode->childCount = 0;
    startNode->children = NULL;

    // Count the number of logical nodes
    int logicalNodeCount = countLogicalNodes(tokens);

    // If logical nodes are present, allocate memory for the array of children
    if (logicalNodeCount > 0) {
        startNode->hasLogical = true;
        startNode->childCount = logicalNodeCount;
    } else {
        startNode->childCount = 1;
    }
	startNode->children = (ASTNode**)malloc(sizeof(ASTNode*) * startNode->childCount);
	if (!startNode->children) {
		perror("Failed to allocate memory for logical node pointers in StartNode");
		free(startNode);
		exit(EXIT_FAILURE);
	}
    return startNode;
}

// Function to create a logical node (&&, ||, or HOLDER)
LogicalNode* createLogicalNode(NodeType type) {
    LogicalNode* node = (LogicalNode*)malloc(sizeof(LogicalNode));
    if (!node) {
        perror("Failed to allocate memory for LogicalNode");
        exit(EXIT_FAILURE);
    }
    node->type = type;
    node->leftInput = NULL;
    node->leftOutput = NULL;
    node->rightInput = NULL;
    node->rightOutput = NULL;
    node->left = NULL;
    node->right = NULL;
    return node;
}

// Function to add logical nodes to the StartNode's children array, or create a HOLDER node
void populateLogicalNodes(StartNode* startNode, Token* tokens) {
    if (startNode->hasLogical) {
        // Allocate logical nodes based on the tokens
        int index = 0;
        Token* currentToken = tokens;
        while (currentToken != NULL && index < startNode->childCount) {
            if (currentToken->type == TOKEN_LOGICAL_AND || currentToken->type == TOKEN_LOGICAL_OR) {
                NodeType type = (currentToken->type == TOKEN_LOGICAL_AND) ? NODE_LOGICAL_AND : NODE_LOGICAL_OR;
                startNode->children[index] = (ASTNode*)createLogicalNode(type);
                index++;
            }
            currentToken = currentToken->next;
        }
    } else {
        // No logical operators found, so create a HOLDER logical node
        startNode->children[0] = (ASTNode*)createLogicalNode(NODE_LOGICAL_HOLDER);
    }
}

// Function to get the string representation of the node type
const char* getNodeTypeString(NodeType type) {
    switch (type) {
        case NODE_COMMAND: return "COMMAND";
        case NODE_LOGICAL_AND: return "LOGICAL_AND";
        case NODE_LOGICAL_OR: return "LOGICAL_OR";
        case NODE_PIPE: return "PIPE";
        case NODE_LOGICAL_HOLDER: return "LOGICAL_HOLDER";
        default: return "UNKNOWN";
    }
}

// Function to print logical nodes stored in the StartNode
void printLogicalNodes(const StartNode* startNode) {
    if (startNode->hasLogical) {
        printf("StartNode has %d logical nodes:\n", startNode->childCount);
        for (int i = 0; i < startNode->childCount; ++i) {
            LogicalNode* logicalNode = (LogicalNode*)(startNode->children[i]);
            printf("Node %d: Type: %s\n", i, getNodeTypeString(logicalNode->type));
        }
    } else {
        printf("StartNode has a logical holder node.\n");
        if (startNode->childCount == 1) {
            LogicalNode* holderNode = (LogicalNode*)(startNode->children[0]);
            printf("Holder Node: Type: %s\n", getNodeTypeString(holderNode->type));
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

int main() {
	Token *token = lexer();
	StartNode *StartNode = createAndSetupStartNode(token);
	populateLogicalNodes(StartNode, token);

	printLogicalNodes(StartNode);

	free_lexer(&token);
    return 0;
}
