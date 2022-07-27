#include "co.h"
#include <stdlib.h>
#include <setjmp.h>
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define K 1024
/**
 * @brief 协程longjmp 返回值
 */
#define __LONG_JUMP_STATUS (1)


/**
 * @brief 协程的堆栈
 */
#define STACK_SIZE (64 * K)


enum co_status {
  CO_NEW = 1, // 新创建，还未执行过
  CO_RUNNING, // 已经执行过
  CO_WAITING, // 在 co_wait 上等待
  CO_DEAD,    // 已经结束，但还未释放资源
};

struct co { 
  const char *name;       // 协程名字
  void (*func)(void *);   // co_start 指定的入口地址和参数
  void *arg;              // 协程执行参数

  enum co_status status;  // 协程的状态
  struct co *    waiter;  // 是否有其他协程在等待当前协程
  jmp_buf        context; // 寄存器现场 (setjmp.h)
  uint8_t        stack[STACK_SIZE]; // 协程的堆栈
};


/**
 * @brief 用双向环形链表存所有的协程信息
 *        有点类似于java AQS的实现
 */
typedef struct CO_NODE {
	struct co *coroutine;

	struct CO_NODE *pre, *next;
} co_node;

/**
 * @brief static使用参考 https://blog.csdn.net/guotianqing/article/details/79828100 
 */
static co_node* node_list;


/**
 * @brief 我们维护了 “当前运行的协程” 的指针 (这段代码非常类似于操作系统中，为每一个 CPU 维护一个 “当前运行的进程”)
 *        这样，在 co_yield 时，我们就知道要将寄存器现场保存到哪里。
 *        全局变量
 */
struct co *current;

/**
 * @brief   实际上最开始执行的main函数流，也是一个协程。因此我们需要在开始执行main之前，为其创建一个协程。
 *          这里定义__attribute__((constructor))属性函数，从其申请一个协程即可 需要注意的是，
 *          和普通的协程不一样，实际上这个协程在创建的时候，就已经是CO_RUNNING的状态了，不需要在调用其他的函数了。
 *          
 *          main函数执行前先执行的是这个函数，创建main协程 是为了方便wait方法切换
 */
static __attribute__((constructor)) void co_constructor(void) {

	current = co_start("main", NULL, NULL);
	current->status = CO_RUNNING;
}

/**
 * @brief 切换栈，即让选中协程的所有堆栈信息在自己的堆栈中，而非调用者的堆栈。保存调用者需要保存的寄存器，并调用指定的函数
 *        分配堆栈是容易的：堆栈直接嵌入在 struct co 中即可，在 co_start 时初始化即可。
 *        但麻烦的是如何让 co_start 创建的协程，切换到指定的堆栈执行。
 *        AbstractMachine 的实现中有一个精巧的 stack_switch_call (x86.h)，可以用于切换堆栈后并执行函数调用，且能传递一个参数。
 * @param sp 
 * @param entry 
 * @param arg 
 */
static inline void stack_switch_call(void *sp, void *entry, void* arg) {
	asm volatile (
#if __x86_64__
			"movq %%rcx, 0(%0); movq %0, %%rsp; movq %2, %%rdi; call *%1"
			: : "b"((uintptr_t)sp - 16), "d"((uintptr_t)entry), "a"((uintptr_t)arg)
#else
			"movl %%ecx, 4(%0); movl %0, %%esp; movl %2, 0(%0); call *%1"
			: : "b"((uintptr_t)sp - 8), "d"((uintptr_t)entry), "a"((uintptr_t)arg) 
#endif
			);
}


/**
 * @brief  从调用的指定函数返回，并恢复相关的寄存器。此时协程执行结束，以后再也不会执行该协程的上下文。
 *         这里需要注意的是，其和上面并不是对称的，因为调用协程给了新创建的选中协程的堆栈，
 *         则选中协程以后就在自己的堆栈上执行，永远不会返回到调用协程的堆栈。
 */
static inline void restore_return() {
	asm volatile (
#if __x86_64__
			"movq 0(%%rsp), %%rcx" : : 
#else
			"movl 4(%%esp), %%ecx" : :  
#endif
			);
}


/***************************************************************************************************************/


/**
 * @brief 双向链表插入节点
 *        如果node_list == NULL，则创建一个新的双向循环链表即可，并返回
 *        如果node_list != NULL, 则在node_list和node_list->pre之间插入（node_list->pre 是链表的尾节点）
 */
void insert_co_node(struct co *coroutine)
{ 
    co_node* n = (co_node*)malloc(sizeof(co_node));
    n->coroutine = coroutine;

    if (!node_list)
    {
      node_list = n;
      node_list->pre = n;
      node_list->next = n;  
    }
    else
    {
      // 注意设置的先后顺寻
      n->next = node_list;
      n->pre = node_list->pre; // node_list->pre 是链表的尾节点
      node_list->pre->next = n;
      node_list->pre = n;  
    }
}



/**
 * @brief 双向链表删除节点
 *        删除的节点就是 co_list 指向的节点
 * 
 *        不能再free 之后再次访问设置NULL 这样操作是非法的
 *        所以设置了remove的返回值 根据返回值来确定是否删除了所有的节点
 * 
 */
co_node* remove_co_node()
{ 
    if(node_list == NULL)
    {
      return NULL;
    }
   
    // 只有一个节点情况
    if(node_list == node_list->next)
    {
      // free(node_list->coroutine->name);
      free(node_list->coroutine);
      free(node_list);
      // 返回空方便判断是否双向链表全部删除了
      return NULL;
    }
    else{
      co_node* head = node_list;
      node_list->next->pre = node_list->pre;
      node_list->pre->next = node_list->next;
      node_list = head->next;
      // free(head->coroutine->name);
      free(head->coroutine);
      free(head);
      // 返回新的节点
      return node_list;
    }
}







/***************************************************************************************************************/


/**
 * @brief 创建一个新的协程，并返回一个指向 struct co 的指针 (类似于 pthread_create)下·
 *        1 新创建的协程从函数 func 开始执行，并传入参数 arg。新创建的协程不会立即执行，
 *          而是调用 co_start 的协程继续执行。（如果a协程调用co_start 创建了b协程，此时b协程不开始执行，而是返回a协程继续执行）
 *        2 使用协程的应用程序不需要知道 struct co 的具体定义，因此请把这个定义留在 co.c 中；
 *          框架代码中并没有限定 struct co 结构体的设计，所以你可以自由发挥。
 *        3 co_start 返回的 struct co 指针需要分配内存。我们推荐使用 malloc() 分配。
 * 
 * @param name 
 * @param func 
 * @param arg 
 * @return struct co* 
 */
struct co *co_start(const char *name, void (*func)(void *), void *arg)
{
  struct co *coroutine = (struct co *)malloc(sizeof(struct co));
  assert(coroutine);

  // assert(name);   // name必须有值
  // coroutine->name = (char *)malloc(sizeof(char) * strlen(name) +1);
  // strcpy(coroutine->name, name);

  coroutine->name = name;
  coroutine->func = func;
  coroutine->arg = arg;
  coroutine->status = CO_NEW;
  coroutine->waiter = NULL;

  insert_co_node(coroutine);
  
  return coroutine;
}


/**
 * @brief 实现协程的切换。协程运行后一直在 CPU 上执行，
 *        直到 func 函数返回或调用 co_yield 使当前运行的协程暂时放弃执行。
 *        co_yield 时若系统中有多个可运行的协程时 (包括当前协程)，你应当随机选择下一个系统中可运行的协程。
 *        co_yield() 之后切换到的协程是随机的 (可能切换到它自己)
 * 
 *        #include <setjmp.h> 
 *        int setjmp(jmp_buf env);
 *        setjmp 函数的功能是将函数在此处的上下文保存在 jmp_buf 结构体中，以供 longjmp 从此结构体中恢复。
 *               参数 env 即为保存上下文的 jmp_buf 结构体变量；
 *               如果直接调用该函数，返回值为 0； 若该函数从 longjmp 调用返回，返回值为非零，由 longjmp 函数提供。
 *               根据函数的返回值，我们就可以知道 setjmp 函数调用是第一次直接调用，还是由其它地方跳转过来的。
 * 
 *        void longjmp(jmp_buf env, int val); 
 *        longjmp 函数的功能是从 jmp_buf 结构体中恢复由 setjmp 函数保存的上下文，该函数不返回，而是从 setjmp 函数中返回。
 *                参数 env 是由 setjmp 函数保存过的上下文。
 *                参数 val 表示从 longjmp 函数传递给 setjmp 函数的返回值，如果 val 值为0， setjmp 将会返回1，否则返回 val。
 *                longjmp 不直接返回，而是从 setjmp 函数中返回，longjmp 执行完之后，程序就像刚从 setjmp 函数返回一样。 
 *        
 *        java 如何实现线程挂起的？？？操作系统帮助？？？TODO
 */
void co_yield() {

  // setjmp 保存当前协程的执行上下文环境
  // printf("co_yield  current = %s\n", current->name);
  int status = setjmp(current->context);
  if (status == 0)
  {
    // 在 co_yield() 被调用时，setjmp 保存寄存器现场后会立即返回 0，
    // 此时我们需要选择下一个待运行的协程 (相当于修改 current)，并切换到这个协程运行。
    
    // 此时的 node_list指向的就是 current
    // 从链表向下循环找到下一个可执行的协程（如果命中了这个if 说明至少有一个协程（当前协程）是CO_NEW状态）
    do
    {
      node_list = node_list->next;
      current = node_list->coroutine;
    } while (!(current->status == CO_NEW || current->status == CO_RUNNING));

    // 切换的新的 current 协程上执行
    // setjmp = 0 表示当前协程第一次调用co_yield，通过node_list得到下一个协程的状态分两种情况
    if (current->status == CO_RUNNING)
    {
      
      // func()
      // {
      //    ...       1
      //    ...       2
      //    ..        3
      //    co_yield  4
      //    ...       5
      //    ...       6
      //    ...       7
      // }   

      // 新的协程之前已经setjmp过  从第5行继续向后执行() 实际是从co_yield 内部的setjmp位置出继续向后执行
      longjmp(current->context, __LONG_JUMP_STATUS);
    }
    else
    {

      // 当前协程是刚创建出来的没有 执行过setjmp
      // 两种情况 1 切换的新协程还是自己  从第5行继续向后执行
      //         2 切换的之后的协程是新的协程  从第1行开始执行
      // func()
      // {
      //    ...       1
      //    ...       2
      //    ..        3
      //    co_yield  4
      //    ...       5
      //    ...       6
      //    ...       7
      // }   


      ((struct co volatile *)current)->status = CO_RUNNING; //这里如果直接赋值，编译器会和后面的覆写进行优化

      // 栈由高地址向低地址生长
      printf("stack_switch_call  current = %s\n", current->name);
      stack_switch_call(current->stack + STACK_SIZE, current->func, current->arg);
      //恢复相关寄存器
      restore_return();


      //此时协程已经完成执行
      current->status = CO_DEAD;

      if (current->waiter)
      {
        current->waiter->status = CO_RUNNING;
      }
      co_yield();
    }
  }
  else
  {
    // setjmp 是由另一个 longjmp 返回的，此时一定是因为某个协程调用 co_yield()，
    // 此时代表了寄存器现场的恢复，因此不必做任何操作，直接返回即可。
    assert(status && current->status == CO_RUNNING); //此时一定是选中的进程通过longjmp跳转到的情况执行到这里
  }
}
 

/**
 * @brief 表示当前协程需要等待，直到 co 协程的执行完成才能继续执行 (类似于 pthread_join)。
 *        1 在被等待的协程结束后、 co_wait() 返回前，co_start 分配的 struct co 需要被释放。
 *          如果你使用 malloc()，使用 free() 释放即可。
 *        2 因此，每个协程只能被 co_wait 一次 (使用协程库的程序应当保证除了初始协程外，
 *          其他协程都必须被 co_wait 恰好一次，否则会造成内存泄漏)。
 * 
 *        co_wait 执行的时候，有两种不同的可能性
 *        1 此时协程已经结束 (func 返回)，这是完全可能的。此时，co_wait 应该直接回收资源。
 *        2 此时协程尚未结束，因此 co_wait 不能继续执行，必须调用 co_yield 切换到其他协程执行，直到协程结束后唤醒。
 * 
 * @param co 
 */
void co_wait(struct co *co) {
  // 结束时 必须使用free释放内存
  if (co->status != CO_DEAD)
  {
    co->waiter = current; // waiter是否有其他协程在等待当前协程
    current->status = CO_WAITING;
    co_yield ();
  }

  // 找到 co 对应的链表节点
  while (node_list->coroutine != co)
  {
    node_list = node_list->next;
  }

  remove_co_node();
}


/***************************************************************************************************************/


/**
 * @brief 当main函数执行结束后，所有协程都应该无条件结束。换句话说，也就是在main函数终止后，
 *        将所有的协程资源全部释放掉即可，也就是双向循环链接及其协程结构这些数据全部释放即可。
 *        这里同样通过__attribute__((destructor))属性的函数，执行上述操作即可
 * 
 *        协程在整个生命周期中没有调用 yield 方法，只有一个协程一条道走到头的情况 需要在这个方法释放main协程
 */
static __attribute__((destructor)) void co_destructor(void) {
  if (node_list == NULL)
  {
    return;
  }

  while (remove_co_node())
  {
    // 在remove方法内部不再
  }

}
