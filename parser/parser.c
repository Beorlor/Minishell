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

// Helper function to attach redirections to the previous command in the AST
void attachRedirectionsToPreviousCommand(ASTNode* root, Redirection* inputs, Redirection* outputs, Redirection* appends) {
    if (root == NULL) return;
    ASTNode* lastCommand = root;
    while (lastCommand->right != NULL) {
        lastCommand = lastCommand->right; // Find the last command node in the tree
    }
    if (lastCommand->type == NODE_COMMAND) {
        // If the last node is a command, attach the redirections
        lastCommand->inputs = inputs;
        lastCommand->outputs = outputs;
        lastCommand->appends = appends;
    }
}

ASTNode* buildCommandPipeTree(Token** currentToken) {
    ASTNode* root = NULL;
    ASTNode* currentCommand = NULL;
    Redirection* tempInputs = NULL;
    Redirection* tempOutputs = NULL;
    Redirection* tempAppends = NULL;

    while (*currentToken != NULL) {
        if ((*currentToken)->type == TOKEN_COMMAND) {
            // Handle command token
            ASTNode* commandNode = createASTNode(NODE_COMMAND, (*currentToken)->value);
            // Attach stored redirections to this command
            commandNode->inputs = tempInputs;
            commandNode->outputs = tempOutputs;
            commandNode->appends = tempAppends;
            // Reset temporary redirection pointers
            tempInputs = NULL;
            tempOutputs = NULL;
            tempAppends = NULL;

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
        } else if ((*currentToken)->type == TOKEN_PIPE || (*currentToken)->type == TOKEN_LOGICAL_AND || (*currentToken)->type == TOKEN_LOGICAL_OR) {
            // Handle pipe or logical operator token
            if (currentCommand == NULL) {
                // Attach redirections to the previous command if there are no commands between pipes
                attachRedirectionsToPreviousCommand(root, tempInputs, tempOutputs, tempAppends);
                // Reset temporary redirection pointers
                tempInputs = NULL;
                tempOutputs = NULL;
                tempAppends = NULL;
            }

            if ((*currentToken)->type == TOKEN_PIPE) {
                // Create a new pipe node
                ASTNode* pipeNode = createASTNode(NODE_PIPE, "|");
                pipeNode->left = root;
                root = pipeNode;
                currentCommand = NULL; // Reset current command as we're starting a new sub-tree
            } else {
                // Break out of the loop for logical operators, since we handle them elsewhere
                break;
            }
        } else if ((*currentToken)->type == TOKEN_REDIRECTION_IN || (*currentToken)->type == TOKEN_REDIRECTION_OUT || (*currentToken)->type == TOKEN_REDIRECTION_APPEND) {
            // Handle redirection token
            Redirection* newRedir = createRedirection((*currentToken)->value);
            if ((*currentToken)->type == TOKEN_REDIRECTION_IN) {
                addRedirection(&tempInputs, newRedir);
            } else if ((*currentToken)->type == TOKEN_REDIRECTION_OUT) {
                addRedirection(&tempOutputs, newRedir);
            } else if ((*currentToken)->type == TOKEN_REDIRECTION_APPEND) {
                addRedirection(&tempAppends, newRedir);
            }
        }

        *currentToken = (*currentToken)->next;
    }

    // Finalize the last command if the end of tokens is reached
    if (currentCommand != NULL) {
        currentCommand->inputs = tempInputs;
        currentCommand->outputs = tempOutputs;
        currentCommand->appends = tempAppends;
    } else {
        // If we ended on redirections without a command, we should free them
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
