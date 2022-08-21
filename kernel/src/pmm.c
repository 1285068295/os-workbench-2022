#include <common.h>


/**
 * @brief 最简单的实现  不去处理已分配的内存  只管分配 不管回收
 *        堆区使用静态的pos变量 来记录上次分配的结束地址
 *        three easy pieces 说了 malloc不是系统的函数调用 而是library库
 * 
 *        有时间了在去实现详细的kalloc和free （当下先完成os的整体实验2022-08-19）
 */
static void *kalloc(size_t size)
{
  void* ret;
  atomic{
      // 自制系统不允许分配大于16M的空间
      assert(size <= (16 * (1 << 20)));

      // pos 用static修饰 下次再执行这段代码时  pos保留上次执行地结果

      /**
       * @brief 解释
       * int* p = 0x0001 表示从地址0x0001开始 向后4个字节（int类型数据）为指针执行的值
       * char* p = 0x0001表示从地址0x0001开始 向后1个字节（char类型数据）为指针执行的值
       * 
       */
      static char* pos ;
      if (!pos)
        pos = heap.start;

      // 计算要分配的空间大小
      int sz = 1;
      while (sz < size)
      {
        sz *= 2;
      }
      // assert(min_pow2(size) == sz)

      /**
       * @brief pos对齐操作 
       * 
       * 例如 start = 100  size=4k  ---> sz =4k
       * 如果在 pos 加上 sz 之前 不能正常 sz，则先调整到能整除sz的位置
       * 能整除的位置就是这次分配size的堆空间的起始地址，
       * 同时将pos+sz后，pos作为下次分配空间的新的地址的起始地址来计算
       */
      while ((intptr_t)pos % sz != 0)
      {
        pos++;
      }

      ret = pos;
      pos += sz;

      // Got 125 MiB heap: [0x300000, 0x800 0000)
      // 必须保证有足够的空间可分配
      panic_on(pos > (char *)heap.end, "full no free heap space");
  }

  return ret;
}


/**
 * @brief 
 * 
 * @param ptr 
 */
static void kfree(void *ptr) {

    // nothing

}

/**
 * @brief 分配内存
 * 
 * @param sz 
 */
void alloc(int sz)
{

  uintptr_t a = (uintptr_t)kalloc(sz);
 
  // 提取最右的1  地址对齐
  // x = 100100100  lowbit(x) = 100
  uintptr_t align = a & -a; // align = lowbit(a);

  if(a > 0x7e00000){

    printf("CPU #%d Alloc %d -> %p align = %d\n", cpu_current(), sz, a, align);
  }

  assert(a && align >= sz);
}

#ifndef TEST
// 框架代码中的 pmm_init (在 AbstractMachine 中运行)
static void pmm_init() {
  uintptr_t pmsize = ((uintptr_t)heap.end - (uintptr_t)heap.start);
  printf("Got %d MiB heap: [%p, %p)\n", pmsize >> 20, heap.start, heap.end);
}
#else
// 测试代码的 pmm_init ()
static void pmm_init() {
  char *ptr  = malloc(HEAP_SIZE);
  heap.start = ptr;
  heap.end   = ptr + HEAP_SIZE;
  printf("Got %d MiB heap: [%p, %p)\n", HEAP_SIZE >> 20, heap.start, heap.end);
}
#endif

// 
// malloc https://link.zhihu.com/?target=https%3A//mp.weixin.qq.com/s%3F__biz%3DMzg4OTYzODM4Mw%3D%3D%26amp%3Bmid%3D2247485718%26amp%3Bidx%3D1%26amp%3Bsn%3D1c6f4c3809d109e1a7c177cce605c5ab%26amp%3Bchksm%3Dcfe99596f89e1c8099fb6f5c7731343ba8fd0007ee6963f43ca36d9b1b0a6264f585da6da5c3%26token%3D2096033094%26lang%3Dzh_CN%23rd


/**
 * @brief 示例化mpp模块，与os模块实例化类似
 *        Makefile中的-c -o 替换后 为了能够 清楚的观察宏的使用  真正运行行项目需要使用上面的-c -o 选项
 */
MODULE_DEF(pmm) = {
  .init  = pmm_init,
  .alloc = kalloc,
  .free  = kfree,
};
