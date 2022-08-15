// 自己新建的文件
#include <common.h>





 

/**
 * @brief 操作系统内核启动入口
 * 
 * @return int 
 */
int main22() {

  // os->init()，其完成操作系统所有部分的初始化。
  // os_init()运行在系统启动后的第一个处理器上， 中断处于关闭状态;
  // 此时系统中的其他处理器尚未被启动（其他处理还没有指定rip的值）


  // os->init();

  // mpe() 就是将不同的cup的rip指向指定的位置  从而cup不停的从rip取指令执行

  // os->run()是所有处理器的入口，在初始化完成后，框架代码调用mpe_init(os->run)，启动所有处理器执行。
  // 原始框架代码中，os->run只是打印Hello World之后就开始死循环;
  // 你之后可以在os->run中添加各种测试代码


  // mpe_init(os->run);  // 启动多处理器
  
 

  printf("ss");
 
  return 1;
}
