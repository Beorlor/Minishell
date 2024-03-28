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
	startNode->children = (LogicalNode**)malloc(sizeof(LogicalNode*) * startNode->childCount);
	if (!startNode->children) {
		perror("Failed to allocate memory for logical node pointers in StartNode");
		free(startNode);
		exit(EXIT_FAILURE);
	}
    return startNode;
}

// Initialize function for an AST node
ASTNode* createASTNode(NodeType type, char* value) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) {
        perror("Failed to allocate memory for ASTNode");
        exit(EXIT_FAILURE);
    }
    node->type = type;
    node->value = strdup(value); // Make a duplicate of the string to avoid potential issues.
    node->left = NULL;
    node->right = NULL;
    node->Input = NULL;
    node->Output = NULL;
    node->Append = NULL;
    return node;
}

// Function to create a logical node (&&, ||, or HOLDER)
LogicalNode* createLogicalNode(NodeType type) {
    LogicalNode* node = (LogicalNode*)malloc(sizeof(LogicalNode));
    if (!node) {
        perror("Failed to allocate memory for LogicalNode");
        exit(EXIT_FAILURE);
    }
    node->type = type;
    node->left = NULL;
    node->right = NULL;
    return node;
}


// Function to add logical nodes to the StartNode's children array, or create a HOLDER node
void addLogicalNodeToStartNode(StartNode* startNode, Token* tokens) {
    if (startNode->hasLogical) {
        // Allocate logical nodes based on the tokens
        int index = 0;
        Token* currentToken = tokens;
        while (currentToken != NULL && index < startNode->childCount) {
            if (currentToken->type == TOKEN_LOGICAL_AND || currentToken->type == TOKEN_LOGICAL_OR) {
                NodeType type = (currentToken->type == TOKEN_LOGICAL_AND) ? NODE_LOGICAL_AND : NODE_LOGICAL_OR;
                startNode->children[index] = createLogicalNode(type);
                index++;
            }
            currentToken = currentToken->next;
        }
    } else {
        // No logical operators found, so create a HOLDER logical node
        startNode->children[0] = createLogicalNode(NODE_LOGICAL_HOLDER);
    }
}

// void assignRedirections(ASTNode* commandNode, Token* token) {
//     // Loop through tokens and assign redirections to the appropriate command node.
//     while (token != NULL) {
//         switch (token->type) {
//             case TOKEN_REDIRECTION_IN:
//                 commandNode->Input = token->value;
//                 break;
//             case TOKEN_REDIRECTION_OUT:
//                 commandNode->Output = token->value;
//                 break;
//             case TOKEN_REDIRECTION_APPEND:
//                 commandNode->Append = token->value;
//                 break;
//             default:
//                 // For non-redirection tokens, you would typically create or traverse command nodes.
//                 // This is simplified and should be expanded based on your AST generation logic.
//                 break;
//         }
//         token = token->next;
//     }
// }

// TOKEN_PAREN,
//     TOKEN_COMMAND,
// 	TOKEN_PIPE

// void	generateAndAttachBTree(StartNode* startNode, Token* tokens)
// {
// 	if (!startNode->hasLogical)
// 	{
// 		LogicalNode* holder = (LogicalNode*)startNode->children[0];
// 		for (Token* current = tokens; current != NULL; current = current->next)
// 		{


// 		}
// 	}
// 	else
// 	{
// 		LogicalNode* currentLogicalNode = NULL;
// 		int count = -1;

// 		for (Token* current = tokens; current != NULL; current = current->next) {
// 			// When encountering logical operators, adjust the current logical node and count accordingly
// 			if (current->type == TOKEN_LOGICAL_AND || current->type == TOKEN_LOGICAL_OR) {
// 				count++; // Move to the next logical section
// 				if (count > 0 && count < startNode->childCount) {
// 					// For subsequent logical nodes, only update currentLogicalNode beyond the first
// 					currentLogicalNode = (LogicalNode*)startNode->children[count];
// 				}
// 				continue; // Skip to next token after adjusting logical context
// 			}

// 			// Assign redirections based on count
// 			if (count == -1)
// 			{

// 			}
// 			else if (count == 0)
// 			{

// 			}
// 			else if (currentLogicalNode != NULL)
// 			{

// 			}
// 		}
//     }
// }


int main() {
    // Your lexer function should be implemented to tokenize the input.
    Token *tokens = lexer();  // Assuming this is implemented elsewhere.

    // Create the starting node for the AST.
    StartNode *startNode = createAndSetupStartNode(tokens);  // Assuming this is implemented to create a start node from tokens.
	addLogicalNodeToStartNode(startNode, tokens);
    // Generate the AST and attach it to the logical nodes.
    //generateAndAttachBTree(startNode, tokens);  // Assuming this is implemented to generate the AST.

    // Print the subtrees of the logical nodes in the specified order.
    printLogicalSubtrees(startNode);

    // Assuming you have a function to free the tokens.
    free_lexer(&tokens);  // Assuming this is implemented to free the list of tokens.

    return 0;
}
