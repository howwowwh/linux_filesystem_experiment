#include <stdio.h>
#include "fs_rbtree.h"

#define CHECK_INSERT 0 // "插入"动作的检测开关(0，关闭；1，打开)
#define CHECK_DELETE 0 // "删除"动作的检测开关(0，关闭；1，打开)
#define LENGTH(a) ((sizeof(a)) / (sizeof(a[0])))

void main()
{
    uint64_t a[] = {10, 40, 30, 60, 90, 70, 20, 50, 80};
    uint64_t i, ilen = LENGTH(a);
    rb_root *root = NULL;
    rb_node* node = NULL;

    root = create_rbtree();
    printf("== 原始数据: ");
    for (i = 0; i < ilen; i++)
        printf("%ld ", a[i]<<32);
    printf("\n");

    for (i = 0; i < ilen; i++)
    {
        insert_rbtree(root, a[i]<<32);
#if CHECK_INSERT
        printf("== 添加节点: %d\n", a[i]);
        printf("== 树的详细信息: \n");
        print_rbtree(root);
        printf("\n");
#endif
    }

    printf("== 前序遍历: ");
    preorder_rbtree(root);

    printf("\n== 中序遍历: ");
    inorder_rbtree(root);

    printf("\n== 后序遍历: ");
    postorder_rbtree(root);
    printf("\n");

    if (rbtree_mininum(root, &i) == 0)
        printf("== 最小值: %ld\n", i);
    if (rbtree_maxinum(root, &i) == 0)
        printf("== 最大值: %ld\n", i);
    printf("== 树的详细信息: \n");
    print_rbtree(root);
    printf("\n");

#if CHECK_DELETE
    for (i = 0; i < ilen; i++)
    {
        delete_rbtree(root, a[i]);

        printf("== 删除节点: %d\n", a[i]);
        if (root)
        {
            printf("== 树的详细信息: \n");
            print_rbtree(root);
            printf("\n");
        }
    }
#endif

    for(int i =0; i< LENGTH(a);i++) {
        node = iterative_rbtree_search(root, a[i]<<32);
        printf(" %ld"+!i,node->key>>32);
    }
    printf("\n");
    printf("%ld\n" ,sizeof(rb_node));
    
    destroy_rbtree(root);
}