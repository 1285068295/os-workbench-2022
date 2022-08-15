#include "thread.h"
#include "common.h"


static void entry(int tid) { pmm->alloc(128); }
static void goodbye()      { printf("End.\n"); }



void test_0()
{
  pmm->init();
  for (int i = 0; i < 4; i++)
    // 创建4个线程来跑用例
    create(entry);
  join(goodbye);
}


/**
 * @brief 批量测试
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        exit(1);
    }
    printf("abcd"); 


    switch (atoi(argv[1]))
    {
    case 0:
        do_test_0();
        break;
    case 1:
        do_test_1();
        break;
    }
}