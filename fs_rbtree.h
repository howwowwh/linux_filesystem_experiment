#ifndef __FS_RBTREE_H
#define __FS_RBTREE_H
#include"fs_common.h"
#define RED 0
#define BLACK 1

typedef uint64_t type;

typedef struct rbtree_node{
    uint8_t color;
    type key;
    struct rbtree_node* left;
    struct rbtree_node* right;
    struct rbtree_node* parent;
}rb_node;

typedef struct rb_root{
    rb_node* node;
}rb_root;

rb_root* create_rbtree();
void destroy_rbtree(rb_root* root);
int insert_rbtree(rb_root* root, type key);
void delete_rbtree(rb_root* root, type key);
void preorder_rbtree(rb_root* root);
void inorder_rbtree(rb_root* root);
void postorder_rbtree(rb_root* root);
int rbtree_search(rb_root* root, type key);
rb_node* iterative_rbtree_search(rb_root* root, type key);
int rbtree_mininum(rb_root* root, type* key);
int rbtree_maxinum(rb_root* root,type* ket);
void print_rbtree(rb_root* root);

#endif