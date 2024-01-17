

typedef struct AVLNode { 
  int                   data;
  struct AVLNode*       left;
  struct AVLNode*       right;
  int                   height;
} AVLNode;

void NewAVLNode(AVLNode** leafPointer, const int data);
void AVLRotationLeft(AVLNode** treePointer);
void AVLRotationRight(AVLNode** treePointer);
void InsertToAVL(AVLNode** treePointer, const int data);
void PrintAVLTreeInOrder(const AVLNode* treeNode);
void DeleteAVLTree(AVLNode* treeNode);
bool InAVLTree(const AVLNode* treeNode, const int data);