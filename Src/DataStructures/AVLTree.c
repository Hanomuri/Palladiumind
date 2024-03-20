#include <stdio.h>
#include <stdlib.h>
#include "DataStructures/AVLTree.h"

#define tree (*treePointer)
#define getHeight(node) ( (node == NULL) ? 0 : node->height )
#define getBalance(node) ( getHeight(node->right) - getHeight(node->left) )
#define max(x, y) ( (x > y) ? x : y )

void NewAVLNode(AVLNode** leafPointer, const int data) {
  #define leaf (*leafPointer)
  leaf          = malloc(sizeof(struct AVLNode));
  leaf->data    = data;
  leaf->left    = NULL;
  leaf->right   = NULL;
  leaf->height  = 1;
}

void AVLRotationLeft(AVLNode** treePointer) {
  AVLNode* oldRoot = tree;

  tree = tree->right;
  oldRoot->right = tree->left;
  tree->left = oldRoot;

  oldRoot->height = max(getHeight(oldRoot->left), getHeight(oldRoot->right)) + 1;
  tree->height = max(getHeight(tree->left), getHeight(tree->right)) + 1;
}

void AVLRotationRight(AVLNode** treePointer) {
  AVLNode* oldRoot = tree;

  tree = tree->left;
  oldRoot->left = tree->right;
  tree->right = oldRoot;

  oldRoot->height = max(getHeight(oldRoot->left), getHeight(oldRoot->right)) + 1;
  tree->height = max(getHeight(tree->left), getHeight(tree->right)) + 1;
}

void InsertToAVL(AVLNode** treePointer, const int data) {
  if(tree == NULL) {
    NewAVLNode(&tree, data);
    return;
  }

  if(data < tree->data) {
    InsertToAVL(&tree->left, data);
  } else if(data > tree->data) {
    InsertToAVL(&tree->right, data);
  } else {
    return;
  }

  tree->height++;

  int balancedValue = getBalance(tree);

  if(balancedValue > 1 && data > tree->right->data) {
    AVLRotationLeft(&tree);
  } 
  if(balancedValue > 1 && data < tree->right->data) {
    AVLRotationRight(&(tree->right));
    AVLRotationLeft(&tree);
  } 
  if(balancedValue < -1 && data < tree->left->data) {
    AVLRotationRight(&tree);
  } 
  if(balancedValue < -1 && data > tree->left->data) {
    AVLRotationLeft(&(tree->left));
    AVLRotationRight(&tree);
  }
}

void PrintAVLTreeInOrder(const AVLNode* treeNode) {
  if(treeNode != NULL) {
    PrintAVLTreeInOrder(treeNode->left);
    printf("%d ", treeNode->data);
    PrintAVLTreeInOrder(treeNode->right);
  }
}

void DeleteAVLTree(AVLNode* treeNode) {
  if(treeNode == NULL) {
    return;
  }
  DeleteAVLTree(treeNode->left);
  DeleteAVLTree(treeNode->right);
  free(treeNode);
}

bool InAVLTree(const AVLNode* treeNode, const int data) {
  if(treeNode == NULL) {
    return false;
  }

  if(data < treeNode->data) {
    InAVLTree(treeNode->left, data);
  } else if(data > treeNode->data) {
    InAVLTree(treeNode->right, data);
  } else {
    return true;
  }
}