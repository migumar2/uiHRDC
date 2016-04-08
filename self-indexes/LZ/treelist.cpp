#include <stdlib.h>
#include <stdio.h>

#include "treelist.h"

#ifdef __cplusplus
namespace lz77index{
#endif

TreeList* newTreeList(){
   TreeList* l;
   l = (TreeList*)malloc(sizeof(TreeList));
   if(l){
      l->prev=NULL;
      l->next=NULL;
      l->left=NULL;
      l->right=NULL;
      l->val=-1;
   }
   return l;
}
TreeList* findValue(TreeList* l,int val){
   if(l->val>val && l->left!=NULL)return findValue(l->left,val);
   if(l->val<val && l->right!=NULL)return findValue(l->right,val);
  return l;/*val==l->val || the branch is NULL*/
}

/*TreeList* findValue(TreeList* l,int val){
    TreeList* next=NULL;
    while(1){
        if(l->val>val){
            next=l->left;
        }else if(l->val<val){
            next=l->right;
        }else{
            next = NULL;
        }
        if(next != NULL){
            l = next;
        }else{
            break;        
        }
    }
    return l;
}*/
void freeTreeList(TreeList* l){
   if(l!=NULL){
      freeTreeList(l->left);
      freeTreeList(l->right);
      /*freeTreeList(l->prev);*/
      /*freeTreeList(l->next);*/
      free(l);
   }
}
int nextValue(TreeList* tl,int val){
   TreeList* l=findValue(tl,val);
   TreeList* next;
   if(l->val<val){
      next=l->next;
   }else{
      next=l;
   }
   if(next==NULL)return -1;
   return next->val;
}
int prevValue(TreeList* tl,int val){
   TreeList* l=findValue(tl,val);
   TreeList* prev;
   if(l->val>val){
      prev=l->prev;
   }else{
      prev=l;
   }
   if(prev==NULL)return -1;
   return prev->val;
}
void swap(TreeList** a,TreeList** b){
   TreeList* t;
   t=*a;
   *a=*b;
   *b=t;
}
void addValue(TreeList* tl,int val){
   TreeList* l=findValue(tl,val);
   if(l->val<val){/*add to the right*/
      TreeList* node=newTreeList();
      node->val=val;
      l->right=node;
      node->prev=l;
      node->next=l->next;
      if(l->next!=NULL)
         l->next->prev=node;
      l->next=node;
   }else if(l->val>val){/*add to the left*/
      TreeList* node=newTreeList();
      node->val=val;
      l->left=node;
      node->next=l;
      node->prev=l->prev;
      if(l->prev!=NULL)
         l->prev->next=node;
      l->prev=node;
   }
}


/*int main(int argc,char** argv){
   int i;
   TreeList* l=newTreeList();
   l->val=0;
   for(i=1;i<argc;i++){printf("add %d\n",i);addValue(l,atoi(argv[i]));}
   for(i=0;i<20;i++){printf("next %d: %d\n",i,nextValue(l,i));}
   freeTreeList(l);   
   return EXIT_SUCCESS;
}*/
#ifdef __cplusplus
}
#endif
