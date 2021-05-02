#include <stdio.h>
#include "fs_rbtree.h"

#define rb_parent(r) ((r)->parent)
#define rb_color(r) ((r)->color)
#define rb_is_red(r) ((r)->color == RED)
#define rb_is_black(r) ((r)->color == BLACK)
#define rb_set_black(r)     \
    do                      \
    {                       \
        (r)->color = BLACK; \
    } while (0)
#define rb_set_red(r)     \
    do                    \
    {                     \
        (r)->color = RED; \
    } while (0)
#define rb_set_parent(r, p) \
    do                      \
    {                       \
        (r)->parent = (p);  \
    } while (0)
#define rb_set_color(r, c) \
    do                     \
    {                      \
        (r)->color = (c);  \
    } while (0)

rb_root *create_rbtree()
{
    rb_root *root = (rb_root *)malloc(sizeof(rb_root));
    root->node = NULL;
    return root;
}

static void preorder(rb_node* tree)
{
    if (tree)
    {
        printf("%ld ", tree->key);
        preorder(tree->left);
        preorder(tree->right);
    }
}
void preorder_rbtree(rb_root *root)
{
    if (root)
    {
        preorder(root->node);
    }
}

static void inorder(rb_node* tree)
{
    if (tree)
    {

        preorder(tree->left);
        printf("%ld ", tree->key);
        preorder(tree->right);
    }
}

void inorder_rbtree(rb_root *root)
{
    if (root)
    {
        inorder(root->node);
    }
}

static void postorder(rb_node* tree)
{
    if (tree)
    {

        preorder(tree->left);
        preorder(tree->right);
        printf("%ld ", tree->key);
    }
}

void postorder_rbtree(rb_root *root)
{
    if (root)
    {
        postorder(root->node);
    }
}

static rb_node* search(rb_node* tree, type key)
{
    if (!tree || tree->key == key)
    {
        return tree;
    }
    if (key < tree->key)
    {
        return search(tree->left, key);
    }
    else
    {
        return search(tree->right, key);
    }
}

int rbtree_search(rb_root *root, type key)
{
    if (root)
        return search(root->node, key) ? 0 : -1;
}

static rb_node* iterative_search(rb_node* tree, type key)
{
    while (tree && tree->key != key)
    {
        if (key < tree->key)
        {
            tree = tree->left;
        }
        else
        {
            tree = tree->right;
        }
    }
    return tree;
}

int iterative_rbtree_search(rb_root *root, type key)
{
    if (root)
    {
        return iterative_search(root->node, key)?0:-1;
    }
}

static rb_node* mininum(rb_node* tree)
{
    if (!tree)
    {
        return NULL;
    }
    while (tree->left != NULL)
    {
        tree = tree->left;
    }
    return tree;
}

int rbtree_mininum(rb_root *root, type *val)
{
    rb_node* node;
    if (root)
    {
        node = mininum(root->node);
    }
    if (!node)
    {
        return -1;
    }
    *val = node->key;
    return 0;
}

static rb_node* maxinum(rb_node* tree)
{
    if (!tree)
    {
        return NULL;
    }
    while (tree->right)
    {
        tree = tree->right;
    }
    return tree;
}

int rbtree_maxinum(rb_root *root, type *val)
{
    rb_node* node;
    if (root)
        node = maxinum(root->node);
    if (!node)
        return NOK;

    *val = node->key;
    return OK;
}

static rb_node* rbtree_successsor(rb_node* tree)
{
    if (tree->right)
        return mininum(tree->right);

    rb_node* x = tree->parent;
    while (x && (tree == x->right))
    {
        tree = x;
        x = x->parent;
    }
    return x;
}

static rb_node* rbtree_predecesssor(rb_node* x)
{
    if (x->left)
    {
        return maxinum(x->left);
    }

    rb_node* y = x->parent;
    while (y && (x == y->left))
    {
        x = y;
        y = y->parent;
    }
    return y;
}

static void rbtree_left_rotate(rb_root *root, rb_node* x)
{
    rb_node* y = x->right;

    x->right = y->left;
    if (y->left != NULL)
        y->left->parent = x;
    y->parent = x->parent;

    if (x->parent == NULL)
    {
        root->node = y;
    }
    else
    {
        if (x->parent->left == x)
            x->parent->left = y;
        else
            x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

static void rbtree_right_rotate(rb_root *root, rb_node* y)
{
    rb_node* x = y->left;
    y->left = x->right;
    if (x->right != NULL)
    {
        x->right->parent = y;
    }

    x->parent = y->parent;
    if (y->parent == NULL)
    {
        root->node = x;
    }
    else
    {
        if (y == y->parent->right)
            y->parent->right = x;
        else
            y->parent->left = x;
    }

    x->right = y;
    y->parent = x;
}

static void rbtree_insert_fixup(rb_root *root, rb_node* node)
{
    rb_node* parent = NULL;
    rb_node* gparent = NULL;

    while ((parent = rb_parent(node)) && rb_is_red(parent))
    {
        gparent = rb_parent(parent);

        if (parent == gparent->left)
        {
            //uncle node is red,left
            {
                rb_node* uncle = gparent->right;
                if (uncle && rb_is_red(uncle))
                {
                    rb_set_black(uncle);
                    rb_set_black(parent);
                    rb_set_red(gparent);
                    node = gparent;
                    continue;
                }
            }

            //uncle is black, right
            if (parent->right == node)
            {
                rb_node* tmp;
                rbtree_left_rotate(root, parent);
                tmp = parent;
                parent = node;
                node = tmp;
            }
            rb_set_black(parent);
            rb_set_red(gparent);
            rbtree_right_rotate(root, gparent);
        }
        else
        {
            //uncle is red
            {
                rb_node* uncle = gparent->left;
                if (uncle && rb_is_red(uncle))
                {
                    rb_set_black(uncle);
                    rb_set_black(parent);
                    rb_set_red(gparent);
                    node = gparent;
                    continue;
                }
            }
            if (parent->left == node)
            {
                rb_node* tmp;
                rbtree_right_rotate(root, parent);
                tmp = parent;
                parent = node;
                node = tmp;
            }
            rb_set_black(parent);
            rb_set_red(gparent);
            rbtree_left_rotate(root, gparent);
        }
    }
    rb_set_black(root->node);
}

static void rbtree_insert(rb_root *root, rb_node* node)
{
    rb_node* y = NULL;
    rb_node* x = root->node;

    // 1. 将红黑树当作一颗二叉查找树，将节点添加到二叉查找树中。
    while (x != NULL)
    {
        y = x;
        if (node->key < x->key)
            x = x->left;
        else
            x = x->right;
    }
    rb_parent(node) = y;

    if (y != NULL)
    {
        if (node->key < y->key)
            y->left = node; // 情况2：若“node所包含的值” < “y所包含的值”，则将node设为“y的左孩子”
        else
            y->right = node; // 情况3：(“node所包含的值” >= “y所包含的值”)将node设为“y的右孩子”
    }
    else
    {
        root->node = node; // 情况1：若y是空节点，则将node设为根
    }

    // 2. 设置节点的颜色为红色
    node->color = RED;

    // 3. 将它重新修正为一颗二叉查找树
    rbtree_insert_fixup(root, node);
}

static rb_node* create_rbtree_node(type key, rb_node* parent, rb_node* left, rb_node* right)
{
    rb_node* p;

    if ((p = (rb_node* )malloc(sizeof(rb_node))) == NULL)
        return NULL;
    p->key = key;
    p->left = left;
    p->right = right;
    p->parent = parent;
    p->color = BLACK; // 默认为黑色

    return p;
}

int insert_rbtree(rb_root *root, type key)
{
    rb_node* node; // 新建结点

    // 不允许插入相同键值的节点。
    // (若想允许插入相同键值的节点，注释掉下面两句话即可！)
    if (search(root->node, key) != NULL)
        return -1;

    // 如果新建结点失败，则返回。
    if ((node = create_rbtree_node(key, NULL, NULL, NULL)) == NULL)
        return -1;

    rbtree_insert(root, node);

    return 0;
}

static void rbtree_delete_fixup(rb_root *root, rb_node* node, rb_node* parent)
{
    rb_node* other;

    while ((!node || rb_is_black(node)) && node != root->node)
    {
        if (parent->left == node)
        {
            other = parent->right;
            if (rb_is_red(other))
            {
                // Case 1: x的兄弟w是红色的
                rb_set_black(other);
                rb_set_red(parent);
                rbtree_left_rotate(root, parent);
                other = parent->right;
            }
            if ((!other->left || rb_is_black(other->left)) &&
                (!other->right || rb_is_black(other->right)))
            {
                // Case 2: x的兄弟w是黑色，且w的俩个孩子也都是黑色的
                rb_set_red(other);
                node = parent;
                parent = rb_parent(node);
            }
            else
            {
                if (!other->right || rb_is_black(other->right))
                {
                    // Case 3: x的兄弟w是黑色的，并且w的左孩子是红色，右孩子为黑色。
                    rb_set_black(other->left);
                    rb_set_red(other);
                    rbtree_right_rotate(root, other);
                    other = parent->right;
                }
                // Case 4: x的兄弟w是黑色的；并且w的右孩子是红色的，左孩子任意颜色。
                rb_set_color(other, rb_color(parent));
                rb_set_black(parent);
                rb_set_black(other->right);
                rbtree_left_rotate(root, parent);
                node = root->node;
                break;
            }
        }
        else
        {
            other = parent->left;
            if (rb_is_red(other))
            {
                // Case 1: x的兄弟w是红色的
                rb_set_black(other);
                rb_set_red(parent);
                rbtree_right_rotate(root, parent);
                other = parent->left;
            }
            if ((!other->left || rb_is_black(other->left)) &&
                (!other->right || rb_is_black(other->right)))
            {
                // Case 2: x的兄弟w是黑色，且w的俩个孩子也都是黑色的
                rb_set_red(other);
                node = parent;
                parent = rb_parent(node);
            }
            else
            {
                if (!other->left || rb_is_black(other->left))
                {
                    // Case 3: x的兄弟w是黑色的，并且w的左孩子是红色，右孩子为黑色。
                    rb_set_black(other->right);
                    rb_set_red(other);
                    rbtree_left_rotate(root, other);
                    other = parent->left;
                }
                // Case 4: x的兄弟w是黑色的；并且w的右孩子是红色的，左孩子任意颜色。
                rb_set_color(other, rb_color(parent));
                rb_set_black(parent);
                rb_set_black(other->left);
                rbtree_right_rotate(root, parent);
                node = root->node;
                break;
            }
        }
    }
    if (node)
        rb_set_black(node);
}

void rbtree_delete(rb_root *root, rb_node* node)
{
    rb_node* child, *parent;
    int color;

    // 被删除节点的"左右孩子都不为空"的情况。
    if ((node->left != NULL) && (node->right != NULL))
    {
        // 被删节点的后继节点。(称为"取代节点")
        // 用它来取代"被删节点"的位置，然后再将"被删节点"去掉。
        rb_node* replace = node;

        // 获取后继节点
        replace = replace->right;
        while (replace->left != NULL)
            replace = replace->left;

        // "node节点"不是根节点(只有根节点不存在父节点)
        if (rb_parent(node))
        {
            if (rb_parent(node)->left == node)
                rb_parent(node)->left = replace;
            else
                rb_parent(node)->right = replace;
        }
        else
            // "node节点"是根节点，更新根节点。
            root->node = replace;

        // child是"取代节点"的右孩子，也是需要"调整的节点"。
        // "取代节点"肯定不存在左孩子！因为它是一个后继节点。
        child = replace->right;
        parent = rb_parent(replace);
        // 保存"取代节点"的颜色
        color = rb_color(replace);

        // "被删除节点"是"它的后继节点的父节点"
        if (parent == node)
        {
            parent = replace;
        }
        else
        {
            // child不为空
            if (child)
                rb_set_parent(child, parent);
            parent->left = child;

            replace->right = node->right;
            rb_set_parent(node->right, replace);
        }

        replace->parent = node->parent;
        replace->color = node->color;
        replace->left = node->left;
        node->left->parent = replace;

        if (color == BLACK)
            rbtree_delete_fixup(root, child, parent);
        free(node);

        return;
    }

    if (node->left != NULL)
        child = node->left;
    else
        child = node->right;

    parent = node->parent;
    // 保存"取代节点"的颜色
    color = node->color;

    if (child)
        child->parent = parent;

    // "node节点"不是根节点
    if (parent)
    {
        if (parent->left == node)
            parent->left = child;
        else
            parent->right = child;
    }
    else
        root->node = child;

    if (color == BLACK)
        rbtree_delete_fixup(root, child, parent);
    free(node);
}

void delete_rbtree(rb_root *root, type key)
{
    rb_node* z, *node;

    if ((z = search(root->node, key)) != NULL)
        rbtree_delete(root, z);
}

static void rbtree_destroy(rb_node* tree)
{
    if (tree == NULL)
        return;

    if (tree->left != NULL)
        rbtree_destroy(tree->left);
    if (tree->right != NULL)
        rbtree_destroy(tree->right);

    free(tree);
}

void destroy_rbtree(rb_root *root)
{
    if (root != NULL)
        rbtree_destroy(root->node);

    free(root);
}

static void rbtree_print(rb_node* tree, type key, int direction)
{
    if (tree != NULL)
    {
        if (direction == 0) // tree是根节点
            printf("%2ld(B) is root\n", tree->key);
        else // tree是分支节点
            printf("%2ld(%s) is %2ld's %6s child\n", tree->key, rb_is_red(tree) ? "R" : "B", key, direction == 1 ? "right" : "left");

        rbtree_print(tree->left, tree->key, -1);
        rbtree_print(tree->right, tree->key, 1);
    }
}

void print_rbtree(rb_root *root)
{
    if (root != NULL && root->node != NULL)
        rbtree_print(root->node, root->node->key, 0);
}