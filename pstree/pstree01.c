#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <regex.h>
#include <string.h>
#include <stdlib.h>

// pstree 的实现第一个版本  最粗糙的实现


// /proc/pid/status，记录进程详细信息
// /proc/pid/task/tid/status，记录线程详细信息
// /proc/1/task/1  这里的1是主线程  task下面的其他目录（除1外是其他线程）

/**
 * @brief 得到进程的父进程ppid
 * 
 * @param filename 进程文件名
 * @return 当前进程的父进程id
 */
int get_ppid(const char *filename)
{
    char c[1000];
    int ppid;
    FILE *fptr = fopen(filename, "r");
    while (fgets(c, sizeof(c), fptr) != NULL)
    {
        if (strncmp(c, "PPid", 4) == 0)
        {
            ppid = atoi(c + 5);
            break;
        }
    }

    fclose(fptr);
    return ppid;
}

/**
 * @brief 正则表达式匹配得到所有的进程号（/proc文件夹下的数字文件夹）
 *        正则表达学习 https://www.cnblogs.com/qingergege/p/7359935.html    
 *  
 *        1 读取/proc/目录下的文件
 *        2 根据正则表达式得到数字文件夹（进程文件夹）
 *        3 读取进程文件夹下的status信息 得到父进程id
 *       
 * @return int 进程的总数量
 */
int get_process_num()
{
    DIR *dir;
    struct dirent *ptr;
    int process_num = 0; // 进程数量
    dir = opendir("/proc");

    int status, i;
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
    
    
    while ((ptr = readdir(dir)) != NULL)
    {
        if (regexec(&reg, ptr->d_name, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0)
        {
            process_num++;
            // printf("pid : %s\n", ptr->d_name);
            
            // ptr->d_name 就是进程号 例如 ptr->d_name = 1
            // 读取/proc/1/status文件 得到pid ppid
            char proc_status[20] = {0};

            // char*strcat(char* strDestination, const char* strSource);
            // strcat() 函数把 strSource 所指向的字符串追加到 strDestination 所指向的字符串的结尾，
            // 所以必须要保证 strDestination 有足够的内存空间来容纳两个字符串，否则会导致溢出错误。
            // 返回值：指向 strDestination 的指针。
            strcat(proc_status, "/proc/");
            strcat(proc_status, ptr->d_name);
            strcat(proc_status, "/status");
            int ppid = get_ppid(proc_status);
            // 打印子进程id 父进程id
            // printf("pid : %s \t ppid : %d   \n", ptr->d_name, ppid);
        }
    }
    closedir(dir);
    regfree(&reg);
    // 可以返回局部变量的值 但是不能返回地址 函数结束会销毁
    return process_num;
}


/**
 * @brief 获取子进程和父进程的对应关系
 * 
 * @param pid 
 * @param ppid 
 */
void init_pid_ppid(int* proc_pid, int* proc_ppid){
     DIR *dir;
    struct dirent *ptr;
    int process_num = 0; // 进程数量
    dir = opendir("/proc");

    int status, i;
    regex_t reg;
    regcomp(&reg, "[1-9][0-9]*", REG_EXTENDED);
    regmatch_t pmatch;
    
    while ((ptr = readdir(dir)) != NULL)
    {
        if (regexec(&reg, ptr->d_name, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0)
        {
            char proc_status[20] = {0};

            strcat(proc_status, "/proc/");
            strcat(proc_status, ptr->d_name);
            strcat(proc_status, "/status");
            int ppid = get_ppid(proc_status);
            
            proc_pid[process_num] =  atoi(ptr->d_name);
            proc_ppid[process_num] =  ppid;
            process_num++;
            // printf("pid : %s \t ppid : %d   \n", ptr->d_name, ppid);
        }
    }
    closedir(dir);
    regfree(&reg);
}

/**
 * @brief 获取init进程号（Linux的1号进程）
 *        返回的是 数组proc_pid的索引值
 */
int get_init_proc(int len, int* proc_pid){
    int init = 0;
    for (; init < len; init++)
    {
        if (proc_pid[init] == 1)
        {
            break;
        }
    }
    return init;
}

int main(){

    int process_num = get_process_num();

    // 构建进程 父进程图结构信息

    /**
     * @brief 用pid和ppid构建图结构
     *
     * ppids = {1, 1, 1, 3, 3, 6, 7}
     *          ↓  ↓  ↓  ↓  ↓  ↓  ↓
     *  pids = {2, 3, 4, 5, 6, 7, 8}
     */
    int proc_pid[100] = {0};
    int proc_ppid[100] = {0};
    init_pid_ppid(proc_pid, proc_ppid);

    int len = sizeof(proc_pid) / sizeof(int);

    // 找到父进程为0的进程就是 init进程
    int init_proc = get_init_proc(len, proc_pid);

    // 利用数组作为栈结构使用 栈中存数组的索引值
    int stack[100] = {};
    int top = 0;
    stack[top] = init_proc;

    // 为了直观的打印树形状 这里对每个进程的层级做下统计
    // int level[100] = {[0 ... 99] = -1};
    int level[100] = {0};

    // 只要栈中有数据 就进行深度优先遍历
    while (top >= 0)
    {
        

        // stack 中存入的数组索引 从栈中弹出一个进程号
        int top_proc_index = stack[top--];
        int current_proc = proc_pid[top_proc_index];
        // %-7d 左对齐
        printf("%-7d", current_proc);
        char leaf_node = 1;
        char if_level_add = 0;
        for (int i = 0; i < len; i++)
        {
            // 找到子进程在数组中索引
            if (proc_ppid[i] == current_proc)
            {
                leaf_node = 0;
                stack[++top] = i;
                level[i] = level[top_proc_index] + 1;
                if_level_add = 1;

            }
        }
        // 当前进程是叶子节点 需要换行
        if (leaf_node)
        {
            printf("\n");
            // 打印制表符 美化树结构  这里要打印下一次弹出栈的进程的层级
            for (int i = 0; i < level[stack[top]]; i++)
            {
                printf("%*c", -7, '-');
            }
        }
    }
    printf("\n");
}