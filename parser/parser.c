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
    node->value = value; // Make a duplicate of the string to avoid potential issues.
    node->left = NULL;
    node->right = NULL;
    node->inputs = NULL;
    node->outputs = NULL;
    node->appends = NULL;
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
    redir->filename = filename;
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
    Redirection* tempInputs = NULL;
    Redirection* tempOutputs = NULL;
    Redirection* tempAppends = NULL;

    while (*currentToken != NULL) {
        switch ((*currentToken)->type) {
            case TOKEN_COMMAND:
                // Create a new command node and attach any pending redirections
                currentCommand = createASTNode(NODE_COMMAND, (*currentToken)->value);
                currentCommand->inputs = tempInputs;
                currentCommand->outputs = tempOutputs;
                currentCommand->appends = tempAppends;
                tempInputs = tempOutputs = tempAppends = NULL; // Clear the temporary storage

                if (root == NULL) {
                    root = currentCommand; // First command in the sequence
                } else {
                    // Attach the current command to the rightmost part of the tree
                    ASTNode* rightmost = root;
                    while (rightmost->right != NULL) {
                        rightmost = rightmost->right;
                    }
                    rightmost->right = currentCommand;
                }
                break;

            case TOKEN_PIPE:
            case TOKEN_LOGICAL_AND:
            case TOKEN_LOGICAL_OR:
                // Finalize the last command with redirections before moving on
                if (currentCommand != NULL) {
                    currentCommand->inputs = tempInputs;
                    currentCommand->outputs = tempOutputs;
                    currentCommand->appends = tempAppends;
                } else {
                    // No current command to attach to, free redirections
                    freeRedirectionList(&tempInputs);
                    freeRedirectionList(&tempOutputs);
                    freeRedirectionList(&tempAppends);
                }

                if ((*currentToken)->type == TOKEN_PIPE) {
                    // Create a new pipe node and reset the current command
                    ASTNode* pipeNode = createASTNode(NODE_PIPE, "|");
                    pipeNode->left = root;
                    root = pipeNode;
                    currentCommand = NULL;
                } else {
                    // For logical operators, simply exit the loop
                    return root;
                }
                // Reset redirection lists
                tempInputs = tempOutputs = tempAppends = NULL;
                break;

            case TOKEN_REDIRECTION_IN:
            case TOKEN_REDIRECTION_OUT:
            case TOKEN_REDIRECTION_APPEND:
                // Handle redirection tokens
                Redirection* newRedir = createRedirection((*currentToken)->value);
                if ((*currentToken)->type == TOKEN_REDIRECTION_IN) {
                    addRedirection(&tempInputs, newRedir);
                } else if ((*currentToken)->type == TOKEN_REDIRECTION_OUT) {
                    addRedirection(&tempOutputs, newRedir);
                } else if ((*currentToken)->type == TOKEN_REDIRECTION_APPEND) {
                    addRedirection(&tempAppends, newRedir);
                }
                break;

            default:
                // Possibly handle unexpected token types or errors
                break;
        }

        *currentToken = (*currentToken)->next; // Move to the next token
    }

    // If ending without a logical operator, handle any remaining command redirections
    if (currentCommand != NULL) {
        currentCommand->inputs = tempInputs;
        currentCommand->outputs = tempOutputs;
        currentCommand->appends = tempAppends;
    } else {
        // Clean up any remaining redirections if no final command is found
        freeRedirectionList(&tempInputs);
        freeRedirectionList(&tempOutputs);
        freeRedirectionList(&tempAppends);
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
    Token *tokens = lexer();
    StartNode *startNode = createAndSetupStartNode(tokens);
    addLogicalNodeToStartNode(startNode, tokens);
    generateAndAttachBTree(startNode, tokens);

    printEntireAST(startNode);

    free_lexer(&tokens);

    return EXIT_SUCCESS;
}
