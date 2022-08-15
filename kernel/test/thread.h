#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdatomic.h>
#include <assert.h>
#include <unistd.h>
#include <pthread.h>

#define NTHREAD 64
enum { T_FREE = 0, T_LIVE, T_DEAD, };
struct thread {
  int id, status;
  pthread_t thread;
  void (*entry)(int);
};

struct thread tpool[NTHREAD], *tptr = tpool;

void *wrapper(void *arg) {
  struct thread *thread = (struct thread *)arg;
  // 这里是调用函数指针（thread->entry即fn函数）进行运算  参数是 thread->id
  thread->entry(thread->id); 
  return NULL;
}


// 编译时需要增加 -lpthread  gcc a.c -lpthread

/**
 * @brief 创建一个入口函数是 fn 的线程，并立即开始执行
 * void fn(int tid) { ... }
 * 参数 tid 从 1 开始编号
 * 语义：在状态中新增 stack frame 列表并初始化为 fn(tid)
 * 
 * @param fn 
 */
void create(void *fn) {
  // 线程总数不能达到 NTHREAD
  assert(tptr - tpool < NTHREAD);
  *tptr = (struct thread) {
    .id = tptr - tpool + 1,
    .status = T_LIVE,
    .entry = fn,
  };

  // int pthread_create(pthread_t *thread,
  //                  const pthread_attr_t *attr,
  //                  void *(*start_routine) (void *),
  //                  void *arg);
  // 1) pthread_t *thread：传递一个 pthread_t 类型的指针变量，也可以直接传递某个 pthread_t 类型变量的地址。pthread_t 是一种用于表示线程的数据类型，每一个 pthread_t 类型的变量都可以表示一个线程。
  // 2) const pthread_attr_t *attr：用于手动设置新建线程的属性，例如线程的调用策略、线程所能使用的栈内存的大小等。大部分场景中，我们都不需要手动修改线程的属性，将 attr 参数赋值为 NULL，pthread_create() 函数会采用系统默认的属性值创建线程。
  //    pthread_attr_t 类型以结构体的形式定义在<pthread.h>头文件中，此类型的变量专门表示线程的属性。关于线程属性，您可以阅读《线程属性有哪些，如何自定义线程属性？》一文做详细地了解。
  // 3) void *(*start_routine) (void *)：以函数指针的方式指明新建线程需要执行的函数，该函数的参数最多有 1 个（可以省略不写），形参和返回值的类型都必须为 void* 类型。void* 类型又称空指针类型，表明指针所指数据的类型是未知的。使用此类型指针时，我们通常需要先对其进行强制类型转换，然后才能正常访问指针指向的数据。
  //    如果该函数有返回值，则线程执行完函数后，函数的返回值可以由 pthread_join() 函数接收。有关 phtread_join() 函数的用法，我们会在《获取线程函数返回值》一节给大家做详细讲解。
  // 4) void *arg：指定传递给 start_routine 函数的实参，当不需要传递任何数据时，将 arg 赋值为 NULL 即可。
  //    如果成功创建线程，pthread_create() 函数返回数字 0，反之返回非零值。
  pthread_create(&(tptr->thread), NULL, wrapper, tptr);
  ++tptr;
}


/**
 * @brief 等待所有运行线程的 fn 返回
 *        在 main 返回时会自动等待所有线程结束
 *        语义：在有其他线程未执行完时死循环，否则返回
 */
void join()
{
  for (int i = 0; i < NTHREAD; i++)
  {
    struct thread *t = &tpool[i];
    if (t->status == T_LIVE)
    {
      pthread_join(t->thread, NULL);
      t->status = T_DEAD;
    }
  }
}

__attribute__((destructor)) void cleanup() {
  join();
}
