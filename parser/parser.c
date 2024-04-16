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

Redirection* createRedirection(char* filename) {
    Redirection* redir = (Redirection*)malloc(sizeof(Redirection));
    if (!redir) {
        perror("Failed to allocate memory for Redirection");
        exit(EXIT_FAILURE);
    }
    redir->filename = strdup(filename);
    redir->next = NULL;
    return redir;
}

void addRedirection(Redirection** list, Redirection* redir) {
    if (*list == NULL) {
        *list = redir;
    } else {
        Redirection* current = *list;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = redir;
    }
}

ASTNode* buildCommandPipeTree(Token** currentToken) {
    ASTNode* root = NULL;
    ASTNode* currentCommand = NULL;

    while (*currentToken != NULL && (*currentToken)->type != TOKEN_LOGICAL_AND && (*currentToken)->type != TOKEN_LOGICAL_OR) {
        if ((*currentToken)->type == TOKEN_COMMAND) {
            ASTNode* commandNode = createASTNode(NODE_COMMAND, (*currentToken)->value);
            commandNode->inputs = NULL;
            commandNode->outputs = NULL;
            commandNode->appends = NULL;

            if (root == NULL) {
                root = commandNode;
            } else {
                ASTNode* rightmost = root;
                while (rightmost->right != NULL) {
                    rightmost = rightmost->right;
                }
                rightmost->right = commandNode;
            }

            currentCommand = commandNode;
        } else if ((*currentToken)->type == TOKEN_PIPE) {
            ASTNode* pipeNode = createASTNode(NODE_PIPE, "|");
            pipeNode->left = root;
            root = pipeNode;
            currentCommand = NULL;
        } else if (currentCommand != NULL && ((*currentToken)->type == TOKEN_REDIRECTION_IN ||
                                              (*currentToken)->type == TOKEN_REDIRECTION_OUT ||
                                              (*currentToken)->type == TOKEN_REDIRECTION_APPEND)) {
            Redirection* newRedir = createRedirection((*currentToken)->value);
            if ((*currentToken)->type == TOKEN_REDIRECTION_IN) {
                addRedirection(&currentCommand->inputs, newRedir);
            } else if ((*currentToken)->type == TOKEN_REDIRECTION_OUT) {
                addRedirection(&currentCommand->outputs, newRedir);
            } else if ((*currentToken)->type == TOKEN_REDIRECTION_APPEND) {
                addRedirection(&currentCommand->appends, newRedir);
            }
        }

        *currentToken = (*currentToken)->next;
    }

    return root;
}

void generateAndAttachBTree(StartNode* startNode, Token* tokens) {
    if (!startNode->hasLogical) {
        // If there are no logical operators, build the entire tree and attach to the logical HOLDER node.
        LogicalNode* holder = startNode->children[0];
        holder->left = buildCommandPipeTree(&tokens); // Since no logical ops, the tree will be directly under the HOLDER.
    } else {
        // If logical operators are present, build separate trees and attach to the correct side of logical nodes.
        int count = -1;

        while (tokens != NULL) {
            // Check for logical operator to increment count and decide where to attach the tree.
            if (tokens->type == TOKEN_LOGICAL_AND || tokens->type == TOKEN_LOGICAL_OR) {
                count++;
                tokens = tokens->next; // Skip the logical operator token.
            } else {
                if (count == -1) {
                    // Attach the first command sequence to the left of the first logical node.
                    startNode->children[0]->left = buildCommandPipeTree(&tokens);
                } else if (count == 0) {
                    // Attach the second command sequence to the right of the first logical node.
                    startNode->children[0]->right = buildCommandPipeTree(&tokens);
                } else {
                    // For all subsequent logical nodes, attach the command sequence to the left.
                    startNode->children[count]->left = buildCommandPipeTree(&tokens);
                }
            }
        }
    }
}

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
