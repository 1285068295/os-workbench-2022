#include <stdio.h>

static void fun(void)
{
    printf("hello from fun.\n");
}


/**
 * @brief static extern 语法学习 https://zhuanlan.zhihu.com/p/109049970
 * 
 * @return int 
 */
int main(void)
{
    fun();
    // fun1();
    // fun2();

    return 0;
}