#include <stdlib.h>
#include <stdio.h>
#include "bst.h"

//create the BST
BST* createBST(int (*cmp)(void*, void*),
               void (*print)(void*),
               void (*freeData)(void*)) {

    BST* tree = malloc(sizeof(BST));
    if (!tree) return NULL;
    //saves each func in the BST struct so we can access it from the tree
    tree->root = NULL;
    tree->compare = cmp;
    tree->print = print;
    tree->freeData = freeData;

    return tree;
}

//insert the node
BSTNode* bstInsert(BSTNode* root, void* data, int (*cmp)(void*, void*)) {
    if (root == NULL) {
        BSTNode* node = malloc(sizeof(BSTNode));
        if (!node) return NULL;
        node->data = data;
        node->left = NULL;
        node->right = NULL;
        return node;
    }
    //checking where to place a new node recursively
    int res = cmp(data, root->data);
    if (res < 0) root->left = bstInsert(root->left, data, cmp);
    if (res > 0) root->right = bstInsert(root->right, data, cmp);

    return root;
}

void* bstFind(BSTNode* root, void* data, int (*cmp)(void*, void*)) {
    if (root == NULL) return NULL;
    //the same principal as the prev func
    int res = cmp(data, root->data);
    if (res == 0) return root->data;
    if (res < 0) return bstFind(root->left, data, cmp);
    return bstFind(root->right, data, cmp);
}
//3 funcs that we saw in the tirgul but i made them generic
void bstInorder(BSTNode* root, void (*print)(void*)) {
    if (!root) return;
    bstInorder(root->left, print);
    print(root->data);
    bstInorder(root->right, print);
}

void bstPreorder(BSTNode* root, void (*print)(void*)) {
    if (!root) return;
    print(root->data);
    bstPreorder(root->left, print);
    bstPreorder(root->right, print);
}

void bstPostorder(BSTNode* root, void (*print)(void*)) {
    if (!root) return;
    bstPostorder(root->left, print);
    bstPostorder(root->right, print);
    print(root->data);
}

//postorder func that frees the child before the father
void bstFree(BSTNode* root, void (*freeData)(void*)) {
    if (!root) return;
    bstFree(root->left, freeData);
    bstFree(root->right, freeData);
    freeData(root->data);
    free(root);
}