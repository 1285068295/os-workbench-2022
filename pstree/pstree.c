#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <dirent.h>
#include <regex.h>


// 打印树的第三个个版本 整合了pstree01.c pstree02.c代码
// strace 学习 https://www.linuxidc.com/Linux/2018-01/150654.htm
 



/****************** 链表用两个结构体来处理比只是用一个node的情况 代码好些很多,树结构用一个node节点就行 *****************/


/**
 * @brief 字段的先后顺序会影响字段间是否有间隙
 * 
 */
typedef struct Tree_Node
{
    char *val;
    int child_size;             // 子节点的数量
    bool leaf_node;             // 是否为叶子节点
    int pid, ppid;
    struct Tree_Node *children; // 子节点
    struct Tree_Node *brothers; // 同级兄弟节点  方便释放节点空间
} tree_node;

/**
 * @brief 链表节点
 */
typedef struct Linked_Node
{
    char* val;
    struct Linked_Node *pre;
    struct Linked_Node *next;
} linked_node;


/**
 * @brief 链表结构
 * 
 */
typedef struct Linked_List
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


/********************************************辅助函数********************************************************/

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



/******************************************* 进程信息读取相关函数 ***************************************/

/**
 * @brief 正则表达式匹配得到所有的进程号（/proc文件夹下的数字文件夹）
 *        正则表达学习 https://www.cnblogs.com/qingergege/p/7359935.html    
 *        正则表达式匹配数字目录(进程目录)
 * 
 *        1 读取/proc/目录下的文件
 *        2 根据正则表达式得到数字文件夹（进程文件夹）
 *        3 读取进程文件夹下的status信息 得到父进程id
 *       
 * @return int 进程的总数量
 */
int filter(const struct dirent *dir)
{
    
    regex_t reg;

    // int regcomp (regex_t *compiled, const char *pattern, int cflags)
    // 这个函数把指定的正则表达式pattern编译成一种特定的数据格式compiled，这样可以使匹配更有效。
    // 函数regexec 会使用这个数据在目标文本串中进行模式匹配。执行成功返回０。 　
    // 参数如下
    // ①regex_t 是一个结构体数据类型，用来存放编译后的正则表达式，它的成员re_nsub 用来存储正则表达式中的子正则表达式的个数，子正则表达式就是用圆括号包起来的部分表达式。
    // ②pattern 是指向我们写好的正则表达式的指针。
    // ③cflags 有如下4个值或者是它们或运算(|)后的值：
    // REG_EXTENDED 以功能更加强大的扩展正则表达式的方式进行匹配。
    // REG_ICASE 匹配字母时忽略大小写。
    // REG_NOSUB 不用存储匹配后的结果。
    // REG_NEWLINE 识别换行符，这样'$'就可以从行尾开始匹配，'^'就可以从行的开头开始匹配。
    regcomp(&reg, "[1-9][0-9]*", REG_EXTENDED);

    // typedef struct
    // {
    //     regoff_t rm_so;  成员rm_so 存放匹配文本串在目标串中的开始位置；
    //     regoff_t rm_eo;  rm_eo 存放结束位置；
    // } regmatch_t;
    regmatch_t pmatch;

    bool match = 0;
    if (regexec(&reg, dir->d_name, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0)
    {
        match = 1;
    }
    regfree(&reg);
    return match;
}

/**
 * @brief 按照进程号的大小进行排序
 * 
 * @return int 
 */
int sort(){

}



/**
 * @brief 读取进程文件得到进程信息并构建树节点
 * 
 * @param filename 进程文件名
 * @return 当前进程的父进程id
 */
tree_node* get_pro_info(const char *filename)
{
    tree_node* node;
    char c[256];
    int ppid;

     // ptr->d_name 就是进程号 例如 ptr->d_name = 1
    // 读取/proc/1/status文件 得到pid ppid
    char proc_status[20] = {0};
    strcpy(proc_status, "/proc/");
    strcat(proc_status, filename);
    strcat(proc_status, "/status");

    FILE *fptr = fopen(proc_status, "r");
    while (fgets(c, sizeof(c), fptr) != NULL)
    {
        if (strncmp(c, "PPid", 4) == 0)
        {
            ppid = atoi(c + 5);
        }else if(strncmp(c, "Name", 4) == 0){
            // Name:\t init \n
            char name[256];
            int i = 0;
            while(c[i+6] !='\0'){
                name[i] = c[i+6];
                i++;
            }
            name[i-1] = '\0';
            node = create_tree_node(str_to_str(&name[0]));
        }
 
    }
    node->pid = atoi(filename);
    node->ppid = ppid;
    fclose(fptr);
    return node;
}
 

/**
 * @brief 
 * 
 * @param total 
 * @param entry 
 * @return tree_node* 
 */
tree_node* build_print_tree(int total, struct dirent **entry_list)
{
    tree_node** nodes = (tree_node**)malloc(sizeof(tree_node*) *total );
    for (int i = 0; i < total; i++)
    {
        struct dirent *entry = entry_list[i];
        tree_node* node = get_pro_info(entry->d_name);
        nodes[i] = node;
        printf("process name=%s ,pid=%d, ppid=%d       \n", node->val, node->pid, node->ppid);
        // free(node->val);
        // free(node);
        free(entry);
    }
    free(entry_list);

    // 把各个节点的子父关系维护好
    for (int i = 0; i < total; i++)
    {
        for (int j = 0; j < total; j++)
        {
            if (nodes[j]->ppid == nodes[i]->pid)
            {
                add_tree_node(nodes[i], nodes[j]);
            }
        }
    }

    // 构建树结构完成 释放掉空间
    tree_node* root = nodes[0];
   
    free(nodes);
    
    return root;
}


/********************************************树打印方法********************************************************/






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


 
/********************************************主函数********************************************************/
 
int main()
{
    struct dirent **entry_list;
    int total = scandir("/proc", &entry_list, filter, alphasort);
    if (total < 0)
    {
        perror("scandir");
        return EXIT_FAILURE;
    }

    tree_node* root = build_print_tree(total, entry_list);
    print_tree_horizontal(root);
    
    return 0;
}