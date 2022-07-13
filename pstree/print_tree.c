#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>



/****************** 链表用两个结构体来处理比只是用一个node的情况 代码好些很多,树结构用一个node节点就行 *****************/
typedef struct Tree_Node
{
    char *val;
    int child_size;              // 子节点的数量
    bool leaf_node;             // 是否为叶子节点
    struct Tree_Node *children; // 子节点
    struct Tree_Node *brothers; // 同级兄弟节点
} tree_node;

/**
 * @brief 链表节点
 */
typedef struct LinkedNode
{
    char* val;
    struct LinkedNode *pre;
    struct LinkedNode *next;
} linked_node;


/**
 * @brief 链表结构
 * 
 */
typedef struct LinkedList
{
    linked_node *first;
    linked_node *last;
} linked_list;

/**
 * @brief 创建一个链表节点
 */
linked_node* create_linked_node(char* val)
{
    linked_node* new_node = (linked_node*)malloc(sizeof(linked_node));
    new_node->val = (char *)malloc((strlen(val) + 1));
    strcpy(new_node->val, val);
    new_node->pre = NULL;
    new_node->next = NULL;
    return new_node;
}


/**
 * @brief 创建一个链表节点
 */
tree_node* create_tree_node(char* val)
{
    tree_node* node = (tree_node*)malloc(sizeof(tree_node));
    node->child_size = 0;
    node->leaf_node = 1;
    // children brothers 默认的就是NULL
    // head->children = NULL;
    // head->brothers = NULL;
    return node;
}

/**
 * @brief 初始化树的根节点
 */
tree_node* init_tree(char* val)
{
    tree_node* head = create_tree_node(val);
    return head;
}

/**
 * @brief 添加一个节点
 */
void add_tree_node(tree_node* parent, char* val)
{
    tree_node *node = create_tree_node(val);
    if (parent->children == NULL)
    {
        parent->children = (tree_node *)malloc(sizeof(tree_node));
        parent->children = node;
    }
    else
    {
        tree_node *child = parent->children;
        while (child->brothers)
        {
            child = child->brothers;
        }
        child->brothers = (tree_node *)malloc(sizeof(tree_node));
        child->brothers = node;
    }

    return ;
}

/**
 * @brief 树的叶子节点空间释放
 * 
 */
void free_tree_leaf_node(tree_node* node){
    // TODO
}



/**
 * @brief 初始化链表
 */
linked_list *init_linked_list(char *val)
{
    linked_node *new_node = create_linked_node(val);
    linked_list *list = (linked_list *)malloc(sizeof(linked_list));
    list->first = (linked_node *)malloc(sizeof(linked_node));
    list->last = (linked_node *)malloc(sizeof(linked_node));
    list->first = new_node;
    list->last = new_node;
    return list;
}


/**
 * @brief 链表末尾添加一个节点
 */
void add_linked_node(linked_list* list, char* str)
{
    // 构建一个新节点
    linked_node* new_node = create_linked_node(str);
    if(list == NULL){
      list = init_linked_list(str);
      return ;
    }
    else
    {
        new_node->pre = (linked_node *)malloc(sizeof(linked_node));
        // 处理旧的last节点
        list->last->next = (linked_node *)malloc(sizeof(linked_node));
        list->last->next = new_node;
        new_node->pre = list->last;
        list->last = new_node;
    }
}

/**
 * @brief 移除链表的末尾节点
 *        必须要保证list不为空
 * @param list 
 */
void remove_last_node(linked_list* list)
{
    linked_node *last_node = list->last;
    if (list->first == list->last)
    {
        free(last_node->val);
        // free(last_node->pre);
        // free(last_node->next);
        free(list->first);
        free(list->last);
    }
    else
    {
        // 前一个节点
        free(last_node->pre->next);
        // 末尾节点
        free(last_node->val);
        free(last_node->pre);

        list->last = list->last->pre;
        free(last_node);
    }
}



/**
 * @brief 将一个字符串所有的字符都转成空格
 *        节点上的字符串转成空白串
 *        注意 char *s = “hello”;等价于 const char *s = “hello”;
 */
char* to_blank_string(char* str)
{
    if (NULL == str || strcmp("", str) == 0)
    {
        return str;
    }
    char* blank_str = (char*) malloc(strlen(str)+ 1);
    int len = strlen(str);
    for (int i = 0; i < len; i++)
    {
        blank_str[i] = ' ';
    }
    free(str); 
    return blank_str;
}


/**
 * @brief 字符串拼接
 */
char *append_str(char *str1, char *str2)
{
    int len = strlen(str1) + strlen(str2) + 1;/*+1 for '\0' character */
    char *str = (char *)malloc(len); // sizeof(char) = 1
    strcat(str, str1);
    strcat(str, str2);
    // 这里的释放需要当心，如果str不是malloc分配的内存 使用free会报错
    free(str1);
    free(str2);
    return str;
}


/**
 * @brief 
 * @brief 递归处理树结构
 * 
 * @param res  最终的结果
 * @param next_level_prefix 当前节点紧邻的前缀
 * @param current_node_prefix （深度优先遍历）当前节点的子节点的前缀
 * @param node 
 * @param has_right_sibling 下一层节点打印前是否有' | '
 */
void traverseNodes(char* res, linked_list* next_level_prefix, char* current_node_prefix,
                        tree_node* node, bool has_right_sibling)
{
    if (node == NULL)
    {
        return;
    }
    append_str(res, current_node_prefix);
    append_str(res, node->val);

    if (node->leaf_node)
    {
        // 叶子节点换行
        char *p = (char *)malloc(2* sizeof(char *));
        strcpy(p, "\n");
        append_str(res, p);
        return;
    }

    // 重新分配内存
    free(current_node_prefix);
    current_node_prefix = (char *)malloc(4);

    // 子节点数不为0情况
    tree_node *child = node->children;
    for (size_t i = 0; i < node->child_size; i++)
    {
        // 下一层前缀需要追加串
        add_linked_node(next_level_prefix, has_right_sibling ? " │ " : "   ");
        
        if (node->child_size == 1)
        {
            current_node_prefix = strcpy(current_node_prefix, "───");
        }
        else if (i == 0)
        {
            // 第一个节点
            current_node_prefix = strcpy(current_node_prefix, "─┬─");
        }
        else
        {
            // 中间节点  需要拼接next_level_prefix
            linked_node *tmp = next_level_prefix->first;
            while (tmp)
            {
                append_str(current_node_prefix, tmp->val);
            }
            // 是否为末尾节点
            if (i != node->child_size - 1)
            {
                append_str(current_node_prefix, "├─");
            }
            else
            {
                append_str(current_node_prefix, " └─");
            }
        }
        traverseNodes(res, next_level_prefix, current_node_prefix, child, (i == node->child_size - 1));
        remove_last_node(next_level_prefix);
        remove_last_node(next_level_prefix);
    }
}


/**
 * @brief  先序方式遍历树结构
 * 
 * @param root 树的根节点
 * @return char*   打印输出的字符串
 */
char* traverse_pre_order(tree_node* root)
{
    if (root == NULL)
    {
        char *res = (char *)malloc(1);/*+1 for '\0' character */
        res = "";
        return res;
    }
    char *res = (char *)malloc(1);
    // 注意 两个字符必须都是 malloc得到的
    append_str(res, root->val);


    // 存在子节点 深度优先遍历
    tree_node* child = root->children;
    for (int i = 0; i < root->child_size; i++)
    {
        char *current_node_prefix = (char *)malloc(4 * sizeof(char));
        if (root->child_size <= 1)
        {
            // 子节点数不超过1
            current_node_prefix = strcpy(current_node_prefix, "───");
        }
        else if (i == 0 && root->child_size > 1)
        {
            // 首个节点且子节点数大于1
            current_node_prefix = strcpy(current_node_prefix, "─┬─");
        }
        else if (i != root->child_size - 1) /*root->brothers == NULL */
        {
            // 中间的节点
            current_node_prefix = strcpy(current_node_prefix, " ├─");
        }
        else
        {
            // 末尾的节点
            current_node_prefix = strcpy(current_node_prefix, " └─");
        }

        char *root_string = to_blank_string(root->val);

        linked_list *next_level_prefix = init_linked_list(root_string);
        // 递归处理
        traverseNodes(res, next_level_prefix, current_node_prefix, child, (i == root->child_size - 1));
        // 释放内存
        free(current_node_prefix);
        free(next_level_prefix);
        child = child->brothers;
    }

    return res;
}


void print_tree_horizontal(tree_node* root)
{
    printf("%s", traverse_pre_order(root));
}


/*****************************************************************************/
tree_node* create_tree_for_test(){
    tree_node* node1 = init_tree("11");
    add_tree_node(node1, "22");
    add_tree_node(node1, "33");
    add_tree_node(node1, "44");
    add_tree_node(node1, "55");
    return node1;
}


/*****************************************************************************/
int main()
{

    tree_node* head =create_tree_for_test();
    print_tree_horizontal(head);
    return 1;
}