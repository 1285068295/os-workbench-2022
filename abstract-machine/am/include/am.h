#ifndef AM_H__
#define AM_H__

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// 整个实验环境搭建 这个过程中没有人给指导确实要踩很多坑，不过还好 总算是挺过来了。
// 大致了解了如何使用vscode来构建大型C项目  加深了makefile构建项目的过程（执行并翻看了无数次 make -nB）
// 其实在L0中 静下心来读一读make的执行过程 就能大致了解OS L0实验的执行流程（不要被控制台的输出吓住）

// bear 命令会劫持make指令 把所有的.c 文件的gcc 编译参数收集出来  ---> 来自yjj的os课程2022版本的第18讲！！！
// 老师在第18讲课里面说出了我们的在pa实验中 不能用vscode的心痛之处。
// 有了这个命令  vscode 终于可以成为玩整体了，能够正确的识别宏定义了！！！
// 我是因为擅自修改makefile文件 导致bear make指令执行过程中 没有收集到全部的.c 文件 其中就有很关键 etc.c文件
// 导致am.h 中 #include ARCH_H 的宏 ARCH_H 始终无法被识别
// gcc -D xxx 在xxx这里可以定义gcc编译的宏  我们项目中是用的gcc -DARCH_H  中间没有空格


// 我掉进大坑里面 是我一直以为 Context 是操作系统层面的代码  后来才想明白 就算是操作系统中的代码 也必须要引入头文件才能找到对应的定义才行
// 我在qume源码目录中找了很久都没找到这个文件 我才恍然大悟 arch/x86_64-qemu.h  就是本地文件 abstract-machine/am/include/下！！！

// 使用bear make 指令生成complie.commands.json  注意查看是否是全部的 .c 的文件编译选项
// 需要注意的是 在执行make 指令是  只有第一次执行时会生成abstract-machine 下的build 文件夹
// 就是说 每次make clean 只会清理 amgame 目录的build 文件夹  又因为 make指令执行时 abstract-machine 的build 文件夹没有改变
// 所以我们每次执行make 指令时 只调用了gcc 编译了 amgame的目录  而abstract-machine 目录没有进行编译
// 所以bear 指令 不能拦截 astract-mechine 的gcc 编译指令！！！

//  （开始我以为是我修改Makefile文件导致没有生成的~~~~）导致etc.c 文件的编译选项没有生成在complie.commands.json
// 实际原因是 根本没有gcc 编译etc.c  等文件  所以根本不会生成这些文件的编译的宏信息
// 知道了这个原因 解决办法很简答 就是直接删除abstract-mechine 下build文件夹 再执行 bear命令 (或着在下载下来项目时
// 在执行make指令之前 先执行bear make 生成 complie.commands.json )
// 这样 complie.commands.json 就是完整的 编译指令 不然就只有 amgame 的.c文件的编译指令信息 确实了 abstract-mechine的.c 文件编译信息


#include ARCH_H // this macro is defined in $CFLAGS
                // examples: "arch/x86-qemu.h", "arch/native.h", ...

// Memory protection flags
#define MMAP_NONE  0x00000000 // no access
#define MMAP_READ  0x00000001 // can read
#define MMAP_WRITE 0x00000002 // can write

// Memory area for [@start, @end)
typedef struct {
  void *start, *end;
} Area;

// Arch-dependent processor context
typedef struct Context Context;

// An event of type @event, caused by @cause of pointer @ref
typedef struct {
  enum {
    EVENT_NULL = 0,
    EVENT_YIELD, EVENT_SYSCALL, EVENT_PAGEFAULT, EVENT_ERROR,
    EVENT_IRQ_TIMER, EVENT_IRQ_IODEV,
  } event;
  uintptr_t cause, ref;
  const char *msg;
} Event;

// A protected address space with user memory @area
// and arch-dependent @ptr
typedef struct {
  int pgsize;
  Area area;
  void *ptr;
} AddrSpace;

#ifdef __cplusplus
extern "C" {
#endif

// ----------------------- TRM: Turing Machine -----------------------
extern   Area        heap;
void     putch       (char ch);
void     halt        (int code) __attribute__((__noreturn__));

// -------------------- IOE: Input/Output Devices --------------------
bool     ioe_init    (void);
void     ioe_read    (int reg, void *buf);
void     ioe_write   (int reg, void *buf);
#include "amdev.h"

// ---------- CTE: Interrupt Handling and Context Switching ----------
bool     cte_init    (Context *(*handler)(Event ev, Context *ctx));
void     yield       (void);
bool     ienabled    (void);
void     iset        (bool enable);
Context *kcontext    (Area kstack, void (*entry)(void *), void *arg);

// ----------------------- VME: Virtual Memory -----------------------
bool     vme_init    (void *(*pgalloc)(int), void (*pgfree)(void *));
void     protect     (AddrSpace *as);
void     unprotect   (AddrSpace *as);
void     map         (AddrSpace *as, void *vaddr, void *paddr, int prot);
Context *ucontext    (AddrSpace *as, Area kstack, void *entry);

// ---------------------- MPE: Multi-Processing ----------------------
bool     mpe_init    (void (*entry)());
int      cpu_count   (void);
int      cpu_current (void);
int      atomic_xchg (int *addr, int newval);

#ifdef __cplusplus
}
#endif

#endif
