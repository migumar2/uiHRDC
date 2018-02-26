#ifdef __cplusplus
namespace lz77index{
extern "C" {
#endif
#ifndef __TREELIST__
#define __TREELIST__

typedef struct TNode{
   struct TNode* left;
   struct TNode* right;
   struct TNode* prev;
   struct TNode* next;
   int val;   
}TreeList;

TreeList* newTreeList();
void freeTreeList(TreeList* l);
int nextValue(TreeList* tl,int val);
int prevValue(TreeList* tl,int val);
void addValue(TreeList* tl,int val);
#endif
#ifdef __cplusplus
}
}
#endif

