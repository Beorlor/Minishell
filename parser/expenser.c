#include "parser.h"

void processBinaryTree(ASTNode* node, void (*processNode)(ASTNode*)) {
    if (node == NULL) return;

    // Process in-order to handle left dependencies first
    processBinaryTree(node->left, processNode);

    // Process this node
    processNode(node);

    // Process right subtree
    processBinaryTree(node->right, processNode);
}

void expandCommandTrees(StartNode* startNode, void (*processNode)(ASTNode*)) {
    if (!startNode->hasLogical) {
        // If there are no logical operators, process the entire tree under the HOLDER node.
        processBinaryTree(startNode->children[0]->left, processNode);
    } else {
        // If logical operators are present, process each tree attached to the logical nodes.
        for (int i = 0; i < startNode->childCount; i++) {
            if (startNode->children[i]->left) {
                processBinaryTree(startNode->children[i]->left, processNode);
            }
            if (i == 0 && startNode->children[i]->right) {
                // For the first logical node, process the right subtree as well.
                processBinaryTree(startNode->children[i]->right, processNode);
            }
        }
    }
}

void printCommandNode(ASTNode* node) {
    if (node->type == NODE_COMMAND || node->type == NODE_PARENTHESE || node->type == NODE_EMPTY_COMMAND) {
        printf("Processing Node: %s\n", node->value ? node->value : "Empty Command");
    }
}

void expandWildcards(ASTNode* node) {
    if (node && node->value) {
        // Example pseudo-function to expand wildcards
        char* expanded = expand_wildcard(node->value);
        free(node->value);
        node->value = expanded;
    }
}

void expandDot(ASTNode* node) {

}

void convertPathToAbsolute(ASTNode* node) {
    if (node && node->value) {
        char* absolutePath = convert_to_absolute_path(node->value);
        free(node->value);
        node->value = absolutePath;
    }
}

void replaceEnvVars(ASTNode* node) {
    if (node && node->value) {
        char* replaced = replace_env_vars(node->value);
        free(node->value);
        node->value = replaced;
    }
}

void detectBuiltInCommands(ASTNode* node) {
    if (node && node->value) {
        if (is_builtin_command(node->value)) {
            node->builtIn = true;
        }
    }
}


void expenser(StartNode* startNode)
{
	expandCommandTrees(startNode, printCommandNode);
}
