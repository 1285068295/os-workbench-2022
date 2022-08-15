#include <common.h>



static void *kalloc(size_t size)
{
  char* ret;
  atomic{
      // 自制系统不允许分配大于16M的空间
      assert(size <= (16 * (1 << 20)));

      static char *pos;
      // char *ret;
      if (!pos)
        pos = heap.start;

      // 计算要分配的空间大小
      int sz = 1;
      while (sz < size)
      {
        sz *= 2;
      }
      // assert(min_pow2(size) == sz)

      // pos对齐操作
      while ((intptr_t)pos % sz != 0)
      {
        pos++;
      }

      ret = pos;
      pos += sz;

      // 必须保证有足够的空间可分配
      panic_on(pos > (char *)heap.end, "full");
  }

  return ret;
}

static void kfree(void *ptr) {


}

/**
 * @brief 分配内存
 * 
 * @param sz 
 */
void alloc(int sz)
{

    uintptr_t  a = (uintptr_t)kalloc(sz);
    // 提取最右的1  地址对齐
    // x = 100100100  lowbit(x) = 100

    uintptr_t align = a & -a; // align = lowbit(a);

    printf("Alloc %d -> %p align = %d\n",sz, a, align);

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


/**
 * @brief 示例化mpp模块，与os模块实例化类似
 * 
 */
MODULE_DEF(pmm) = {
  .init  = pmm_init,
  .alloc = kalloc,
  .free  = kfree,
};
