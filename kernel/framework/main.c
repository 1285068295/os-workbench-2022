#include <kernel.h>
#include <klib.h>

//***************************************锁相关的实现**********************************************************
int locked = 0;
void lock()
{
    // 2）内存操作数约束（Memory operand constraint, m）
    // 当我们不想通过寄存器中转，而是直接操作内存时，可以用"m"来约束。例如：
    // asm volatile ( "lock; decl %0" : "=m" (counter) : "m" (counter));
    // 该指令实现原子减一操作，输入、输出操作数均直接来自内存（也正因如此，才能保证操作的原子性）。
    
    // 教科书实现
    while (atomic_xchg(&locked, 1));
}


void unlock()
{
    // 教科书实现
    atomic_xchg(&locked, 0);
}

//***************************************锁相关的实现********************************************************** 

// // 内存分配与释放
void *kalloc(size_t size);
void kfree(void *ptr);


void pmm_test()
{

    alloc(1);
    alloc(5);
    alloc(10);
    alloc(32);
    alloc(4096);
    alloc(4096);
    while (1)
    {
        alloc(4096);
    }
    
}

int main() {

  // os->init()，其完成操作系统所有部分的初始化。
  // os_init()运行在系统启动后的第一个处理器上， 中断处于关闭状态;
  // 此时系统中的其他处理器尚未被启动（其他处理还没有指定rip的值）


  // os->init();

  // mpe() 就是将不同的cup的rip指向指定的位置  从而cup不停的从rip取指令执行

  // os->run()是所有处理器的入口，在初始化完成后，框架代码调用mpe_init(os->run)，启动所有处理器执行。
  // 原始框架代码中，os->run只是打印Hello World之后就开始死循环;
  // 你之后可以在os->run中添加各种测试代码


  // mpe_init(os->run);  // 启动多处理器

 
 
  for (const char* s = "hello world nju os.\n"; *s; s++)
  {
    putch(*s);
  }
  

  return 1;
}
