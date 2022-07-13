#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>


// 打印树的第二个版本  先实现了树形状的打印（参考了java_vaersion的版本二） 未处理进程信息
// malloc 申请的内存空间被释放后, 内部存有的值依旧保留. 如果这片内存没有被
// 其他 malloc 获取并存值(操作系统和其他应用也有可能访存这片"无主"的内存), 其内含的值就不会变化.
// 原则上, 不应该访问已经被 free 的内存, 因为那里已经不归你管了, 其存储的值不可预测.


// 踩坑地方  
// 1 void func(char* p) func内部改变了p的地址  函数外部不能得到新的p的指针地址
//   如同void func(int a) func内部改变a 外部无法感知
// 2 由于踩坑1的存在 所以不要在子函数中释放参数上的指针 除非子函数返回值为参数上的新指针 
//   这样才能在主函数中释放参数上的指针
// 3 数组的原地就是内容，而指针的那里保存的是内容的地址。
// 4 下面给出四种返回字符串的方法：
// 4.1、 将字符串指针作为函数参数传入，并返回该指针。
// 4.2、 使用malloc函数动态分配内存，注意在主调函数中释放。
// 4.3、 返回一个静态局部变量。
// 4.4、 使用全局变量。
// 5  要使用 valgrind --leak-check=yes ./a.out 分析程序是否有内存泄漏

/****************** 链表用两个结构体来处理比只是用一个node的情况 代码好些很多,树结构用一个node节点就行 *****************/
typedef struct Tree_Node
{
    char *val;
    int child_size;             // 子节点的数量
    bool leaf_node;             // 是否为叶子节点
    struct Tree_Node *children; // 子节点
    struct Tree_Node *brothers; // 同级兄弟节点  方便释放节点空间
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
    int size;
    linked_node *first;
    linked_node *last;
} linked_list;


/********************************************链表相关方法********************************************************


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
 * @brief 初始化链表
 */
linked_list *init_linked_list(char *val)
{
    linked_node *new_node = create_linked_node(val);
    linked_list *list = (linked_list *)malloc(sizeof(linked_list));
    list->first = new_node;
    list->last = new_node;
    list->size = 1;
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
    }
    else
    {
        // 处理旧的last节点
        list->last->next = new_node;
        new_node->pre = list->last;
        list->last = new_node;
    }
    list->size++;
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
        linked_node* pre_last_node = last_node->pre;
        // 末尾节点
        free(last_node->val);
        free(last_node);// ==> free(last_node->pre->next)
        list->last = pre_last_node;
    }
    list->size--;
}

void free_linked_list(linked_list* list)
{
    if(list == NULL)
    {
        return;
    }
    while (list->size > 0)
    {
        linked_node *pre_last = list->last->pre;
        free(list->last->val);
        free(list->last);
        list->last = pre_last;
        list->size--;
    }
    free(list);
}

/********************************************树相关方法********************************************************/


/**
 * @brief 创建一个树节点
 *        需要注意的是 val必须是malloc得到的字符串
 */
tree_node* create_tree_node(char* val)
{
    tree_node* node = (tree_node*)malloc(sizeof(tree_node));
    node->val = (char*) malloc((strlen(val) + 1) * sizeof(char)  );
    strcpy(node->val, val);
    node->child_size = 0;
    node->leaf_node = 1;
    // children brothers 必须要设置
    node->brothers = NULL;
    node->children = NULL;
    // val参数必须要是malloc得到的字符串
    free(val);
    return node;
}


/**
 * @brief 初始化树的根节点
 */
tree_node* init_tree(char* val)
{
    return create_tree_node(val);
}

/**
 * @brief 添加一个节点
 */
void add_tree_node(tree_node* parent, tree_node* node)
{
    if (node == NULL)
    {
        return;
    }

    parent->leaf_node = 0;
    parent->child_size++;
    if (parent->children == NULL)
    {
        parent->children = node;
    }
    else
    {
        tree_node *child = parent->children;
        while (child->brothers)
        {
            child = child->brothers;
        }
        child->brothers = node;
    }
}

/**
 * @brief 添加一个节点
 */
tree_node* add_tree_node_val(tree_node* parent, char* val)
{
    tree_node* child = create_tree_node(val);
    add_tree_node(parent, child);
    return child;
}

/**
 * @brief 树的节点空间释放
 *        深度优先方式释放
 */
void free_tree_node(tree_node* head){

    if (head == NULL)
    {
        return;
    }
    tree_node* children = head->children;
    tree_node* brothers = head->brothers;
    free(head->val);
    free(head);
    // head->val = NULL;
    // head = NULL;

    // 释放兄弟节点
    free_tree_node(brothers);
    // 释放子节点
    free_tree_node(children);
}


/********************************************字符串处理相关方法********************************************************/


/**
 * @brief 将一个字符串所有的字符都转成空格
 *        节点上的字符串转成空白串  注意 str字符串必须要是malloc分配的才能修改
 *        注意 char *s = “hello”;等价于 const char *s = “hello”;
 */
char* to_blank_string(char* str)
{
    
    if (NULL == str || strcmp("", str) == 0)
    {
        // char* blank_str = (char*) malloc(sizeof(char));
        return "";
    }

    int len = strlen(str);//strlen(NULL) Segmentation fault
    for (int i = 0; i < len; i++)
    {
        str[i] = ' ';
    }
    return str;
}


/**
 * @brief 字符串拼接
 *        str1 必须时malloc的字符串
 *        这里必须要返回新的指针  因为原来的指针地址是作为参数传进来的
 *        外部不能得到新的地址值（参见 踩坑点1）
 */
char* append_str(char* str1, char* str2)
{
    // 重新分配内存 会自动释放str1内存
    char* str = (char *) malloc(strlen(str1) + strlen(str2) + 1);/*+1 for '\0' character */
    strcpy(str, str1);
    strcat(str, str2);
    free(str1);
    return str;
}

/********************************************树打印方法********************************************************/

/**
 * @brief 下一侧要打印的节点
 * 
 * @return true 
 * @return false 
 */
bool next_leve_sibling(){
    
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
char* traverse_nodes(char* res, linked_list* next_level_prefix, char* current_node_prefix,
                        tree_node* node, bool has_right_sibling)
{
    if (node == NULL)
    {
        return res;
    }
    res = append_str(res, current_node_prefix);
    res = append_str(res, node->val);
    if (node->leaf_node)
    {
        // 叶子节点换行
        res = append_str(res, "\n");
        return res;
    }

  
    // 子节点数不为0情况
    tree_node *child = node->children;
    for (size_t i = 0; i < node->child_size; i++)
    {   
        char* new_current_node_prefix = (char*)malloc(sizeof(char));
        strcpy(new_current_node_prefix, "");  // malloc之后必须得初始化
        // 下一层前缀需要追加串
        add_linked_node(next_level_prefix, has_right_sibling ? " │ " : "   ");
        
        if (node->child_size == 1)
        {
            
            new_current_node_prefix = append_str(new_current_node_prefix, "───");
            add_linked_node(next_level_prefix, to_blank_string(node->val));
        }
        else if (i == 0)
        {
            // 第一个节点
            new_current_node_prefix = append_str(new_current_node_prefix, "─┬─");
            add_linked_node(next_level_prefix, to_blank_string(node->val));
        }
        else
        {
            // 先加入next_level_prefix再进行遍历
            add_linked_node(next_level_prefix, to_blank_string(node->val));
            // 中间/末尾节点  需要拼接next_level_prefix
            linked_node *tmp = next_level_prefix->first;
            while (tmp)
            {
                new_current_node_prefix = append_str(new_current_node_prefix, tmp->val);
                tmp = tmp->next;
            }
            // 是否为末尾节点
            if (i != node->child_size - 1)
            {
                new_current_node_prefix = append_str(new_current_node_prefix, " ├─");
            }
            else
            {
                new_current_node_prefix = append_str(new_current_node_prefix, " └─");
            }
        }
        
        res = traverse_nodes(res, next_level_prefix, new_current_node_prefix, child, (node->child_size > 1 &&  i != node->child_size - 1));
        remove_last_node(next_level_prefix);
        remove_last_node(next_level_prefix);
        child = child->brothers;
        free(new_current_node_prefix);
    }
    return res;
}


/**
 * @brief  先序方式遍历树结构
 * 
 * @param root 树的根节点
 * @return char*   打印输出的字符串
 */
char* traverse_pre_order(tree_node* root)
{
    char *res = (char *)malloc(sizeof(char));/*+1 for '\0' character */
    strcpy(res, "");
    if (root == NULL)
    {
        return res;
    }
    res = append_str(res, root->val);

    // // 转空格
    char *root_blank_string = to_blank_string(root->val);
    linked_list *next_level_prefix = init_linked_list(root_blank_string);

    // 存在子节点 深度优先遍历
    tree_node* child = root->children;
   
    for (int i = 0; i < root->child_size; i++)
    {
        // 必须放在for内部
        char *current_node_prefix = (char *)malloc(sizeof(char));
        // 这里需要初始化 否则会直接append_str得到结果不对   
        // malloc之后 如果是之前用过的地址，strlen(current_node_prefix) ≠ 1
        strcpy(current_node_prefix, "");
        if (root->child_size <= 1)
        {
            // 子节点数不超过1
            current_node_prefix = append_str(current_node_prefix, "───");
        }
        else if (i == 0 && root->child_size > 1)
        {
            // 首个节点且子节点数大于1
            current_node_prefix = append_str(current_node_prefix, "─┬─");;
        }
        else if (i != root->child_size - 1) /*root->brothers == NULL */
        {
            // 中间的节点
            current_node_prefix = append_str(current_node_prefix, root_blank_string);
            current_node_prefix = append_str(current_node_prefix, " ├─");
        }
        else
        {
            // 末尾的节点
            current_node_prefix = append_str(current_node_prefix, root_blank_string);
            current_node_prefix = append_str(current_node_prefix, " └─");
        }
        
        // 递归处理
        res = traverse_nodes(res, next_level_prefix, current_node_prefix, child, (root->child_size > 1 && i != root->child_size - 1));
        // 释放内存
        free(current_node_prefix);
        child = child->brothers;
    }

    // 统一释放链表 树空间
    free_tree_node(root);  
    free_linked_list(next_level_prefix);  
    return res;
}


void print_tree_horizontal(tree_node* root)
{
    char* res = traverse_pre_order(root);
    printf("%s\n", res);
    free(res);
}


/********************************************测试用例********************************************************/

/**
 * @brief 数字转字符串
 */
char* num_to_str(int num)
{
    int len = 1;
    int tmp = num;
    while (tmp != 0)
    {
        len++;
        tmp /= 10;
    }
    char *str = malloc((len)* sizeof(char));
    // char str[len+1];
    str[--len] = '\0';
    while (len > 0)
    {
        str[--len] = num % 10 + '0';
        num /= 10;
    }
   
   return str;
}


/**
 * @brief 字符串常量转成malloc分配的字符
 */
char* str_to_str(char* const_str)
{
    char* str = (char*) malloc((strlen(const_str)+ 1) * sizeof(char));
    strcpy(str, const_str);
    return str;
}





// for test
tree_node* generate(int i, int max_level, int j, int max_brothers, int max_value)
{
    tree_node* head;
    if (i >= max_level) {
        tree_node* head = NULL;
        return head;
    }

    // rand() 会随机生成一个位于 0 ~ RAND_MAX 之间的整数。
  
    head = create_tree_node(num_to_str(rand() % max_value));

    for (int m = 0; m < max_brothers  && (rand() % 100 > 30); m++)
    {
        tree_node *children = generate(i + 1, max_level, m, max_brothers, max_value);
        add_tree_node(head, children);
    }
    return head;
}

// for test
tree_node* generate_rand_tree(int max_level, int max_brothers, int max_value)
{
    return generate(0, max_level, 0, max_brothers, max_value);
}

void print_tree_test_rand(){
    int max_level = 5;
    int max_brothers = 5;
    int max_value = 65535;
    int testTimes = 100;
    srand((unsigned)time(NULL));
    for (int i = 0; i < testTimes; i++)
    {
        tree_node* head = generate_rand_tree(max_level, max_brothers, max_value);
        print_tree_horizontal(head);

        printf("-----------------------------------------------------------------------------\n");

    }
}


void  print_tree_test1(){
    tree_node* root = init_tree(str_to_str("zhangsan"));
    print_tree_horizontal(root);
    printf("\n");
}
   
void print_tree_test2(){
    tree_node* node1 = init_tree(str_to_str("zhangsan"));
    tree_node* node2 = add_tree_node_val(node1, str_to_str("lisi"));
    tree_node* node3 = add_tree_node_val(node2, str_to_str("wangwu"));
    tree_node* node4 = add_tree_node_val(node3, str_to_str("zhaoliu"));
    print_tree_horizontal(node1);
    printf("\n");
}

void print_tree_test3(){
    tree_node* node1 = init_tree(str_to_str("zhangsan"));
    tree_node* node2 = add_tree_node_val(node1, str_to_str("lisi"));
    tree_node* node3 = add_tree_node_val(node2, str_to_str("wangwu"));
    tree_node* node4 = add_tree_node_val(node3, str_to_str("zhaoliu"));
    tree_node* node5 = add_tree_node_val(node2, str_to_str("zhouqi"));

    tree_node* node6 = add_tree_node_val(node1, str_to_str("fawaikuangtu"));
    tree_node* node7 = add_tree_node_val(node1, str_to_str("liubei"));
    tree_node* node8 = add_tree_node_val(node1, str_to_str("zhuge")); 

    tree_node* node9 = add_tree_node_val(node6, str_to_str("lusu")); 
    tree_node* node10 = add_tree_node_val(node6, str_to_str("huawei")); 
    tree_node* node11 = add_tree_node_val(node1, str_to_str("xiaomi")); 
    print_tree_horizontal(node1);
    printf("\n");
}

void print_tree_test4(){
    tree_node* node1 = init_tree(str_to_str("zhangsan"));
    tree_node* node2 = add_tree_node_val(node1, str_to_str("lisi"));
    tree_node* node3 = add_tree_node_val(node2, str_to_str("wangwu"));
    tree_node* node4 = add_tree_node_val(node2, str_to_str("zhaoliu"));
    tree_node* node5 = add_tree_node_val(node2, str_to_str("zhouqi"));

    tree_node* node6 = add_tree_node_val(node1, str_to_str("fawaikuangtu"));
    tree_node* node7 = add_tree_node_val(node1, str_to_str("liubei"));
    tree_node* node8 = add_tree_node_val(node1, str_to_str("zhuge")); 

    tree_node* node9 = add_tree_node_val(node6, str_to_str("lusu")); 
    tree_node* node10 = add_tree_node_val(node6, str_to_str("huawei")); 
    tree_node* node11 = add_tree_node_val(node1, str_to_str("xiaomi")); 
    print_tree_horizontal(node1);
    printf("\n");
}
 


void print_tree_test5(){
    tree_node* node1 = init_tree(str_to_str("32566"));
       
    tree_node* node2 = add_tree_node_val(node1, str_to_str("41444"));
    tree_node* node3 = add_tree_node_val(node2, str_to_str("32095"));
    tree_node* node4 = add_tree_node_val(node3, str_to_str("48472"));
    tree_node* node5 = add_tree_node_val(node3, str_to_str("38069"));

    tree_node* node6 = add_tree_node_val(node5, str_to_str("9482"));
    tree_node* node7 = add_tree_node_val(node5, str_to_str("44040"));
    tree_node* node8 = add_tree_node_val(node5, str_to_str("37811")); 

    // tree_node* node9 = add_tree_node_val(node2, "lusu"); 
    tree_node* node10 = add_tree_node_val(node3, str_to_str("huawei")); 
    // tree_node* node11 = add_tree_node_val(node1, "xiaomi"); 
    print_tree_horizontal(node1);
    printf("\n");
}





void print_tree_test6(){

    tree_node* node1 = init_tree(num_to_str(28096));
       
    tree_node* node2 = add_tree_node_val(node1, num_to_str(19567));
    tree_node* node3 = add_tree_node_val(node2, num_to_str(55065));
    tree_node* node4 = add_tree_node_val(node3, num_to_str(30454));

    tree_node* node5 = add_tree_node_val(node4, num_to_str(11926));
    tree_node* node6 = add_tree_node_val(node4, num_to_str(9482));
    tree_node* node7 = add_tree_node_val(node4, num_to_str(44040));

    tree_node* node8 = add_tree_node_val(node3, num_to_str(5502)); 

    tree_node* node9 = add_tree_node_val(node1, str_to_str("lusu")); 
    tree_node* node10 = add_tree_node_val(node1, str_to_str("huawei")); 
    // tree_node* node11 = add_tree_node_val(node1, "xiaomi"); 
    print_tree_horizontal(node1);
    printf("\n");
}



void print_tree_test7(){
    tree_node* node1 = init_tree(num_to_str(25784));
    tree_node* node3 = add_tree_node_val(node1, num_to_str(11892));
    tree_node* node4 = add_tree_node_val(node3, num_to_str(38807));
    tree_node* node5 = add_tree_node_val(node4, num_to_str(44427));
    tree_node* node6 = add_tree_node_val(node5, num_to_str(19248));
    tree_node* node17 = add_tree_node_val(node5, num_to_str(1));
    tree_node* node8 = add_tree_node_val(node3, num_to_str(35025));
 
    print_tree_horizontal(node1);
    printf("\n");
}

/********************************************主函数********************************************************/
 
int main()
{
    // print_tree_test1();
    // print_tree_test2();
    // print_tree_test3();
    // print_tree_test4();
    // print_tree_test5();
    // print_tree_test6();
    print_tree_test7();
    print_tree_test_rand();
    return 1;
}