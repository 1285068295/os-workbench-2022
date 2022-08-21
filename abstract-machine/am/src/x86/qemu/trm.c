#include "x86-qemu.h"

Area heap = {};
int __am_ncpu = 0;

int main(const char *args);

static void call_main(const char *args) {
  halt(main(args));
}

void _start_c(char *args) {
  if (boot_record()->is_ap) {
    __am_othercpu_entry();
  } else {
    __am_bootcpu_init();
    stack_switch_call(stack_top(&CPU->stack), call_main, (uintptr_t)args);
  }
}

void __am_bootcpu_init() {
  heap = __am_heap_init();
  __am_lapic_init();
  __am_ioapic_init();
  __am_percpu_init();
}

void __am_percpu_init() {
  __am_percpu_initgdt();
  __am_percpu_initlapic();
  __am_percpu_initirq();
}

void putch(char ch) {
  #define COM1 0x3f8
  outb(COM1, ch);
}

void halt(int code) {
  const char *hex = "0123456789abcdef";
  const char *fmt = "CPU #$ Halt (40).\n";
  cli();
  __am_stop_the_world();
  for (const char *p = fmt; *p; p++) {
    char ch = *p;
    switch (ch) {
      case '$':
        putch(hex[cpu_current()]);
        break;
      case '0': case '4':
        putch(hex[(code >> (ch - '0')) & 0xf]);
        break;
      default:
        putch(ch);
    }
  }
  outw(0x604, 0x2000); // offer of qemu :)
  while (1) hlt();
}
/**
 * @brief Makefile中的-c -o 替换后 为了能够 清楚的观察宏的使用  真正运行行项目需要使用上面的-c -o 选项
 * 
 * @return Area 
 */
Area __am_heap_init() {
  extern char end;
  // 有关端口映射的说明 https://blog.csdn.net/baidu_37973494/article/details/82390383
  // ???
  outb(0x70, 0x34);
  // lo = 0
  uint32_t lo = inb(0x71);

  // ???
  outb(0x70, 0x35);
  // hi = 8
  uint32_t hi = inb(0x71) + 1;
  // 宏定义 RANGE(st, ed)       (Area) { .start = (void *)(st), .end = (void *)(ed) }
  // 转换后  (Area) { .start = (void *)(ROUNDUP(&end, 1 << 20)),  
  //                  .end   = (void *)((uintptr_t)((lo | hi << 8) << 16)) }

  // 利用宏初始化 堆空间大小 125MB
  return RANGE(ROUNDUP(&end, 1 << 20), (uintptr_t)((lo | hi << 8) << 16));
}

void __am_lapic_init() {
  for (char *st = (char *)0xf0000; st != (char *)0xffffff; st ++) {
    if (*(volatile uint32_t *)st == 0x5f504d5f) {
      uint32_t mpconf_ptr = ((volatile MPDesc *)st)->conf;
      MPConf *conf = (void *)((uintptr_t)(mpconf_ptr));
      __am_lapic = (void *)((uintptr_t)(conf->lapicaddr));
      for (volatile char *ptr = (char *)(conf + 1);
           ptr < (char *)conf + conf->length; ptr += 8) {
        if (*ptr == '\0') {
          ptr += 12;
          panic_on(++__am_ncpu > MAX_CPU, "cannot support > MAX_CPU processors");
        }
      }
      return;
    }
  }
  bug();
}

void __am_percpu_initgdt() {
#if __x86_64__
  SegDesc *gdt = CPU->gdt;
  TSS64 *tss = &CPU->tss;
  gdt[SEG_KCODE] = SEG64(STA_X | STA_R,                      DPL_KERN);
  gdt[SEG_KDATA] = SEG64(STA_W,                              DPL_KERN);
  gdt[SEG_UCODE] = SEG64(STA_X | STA_R,                      DPL_USER);
  gdt[SEG_UDATA] = SEG64(STA_W,                              DPL_USER);
  gdt[SEG_TSS]   = SEG16(STS_T32A,      tss, sizeof(*tss)-1, DPL_KERN);
  bug_on((uintptr_t)tss >> 32);
  set_gdt(gdt, sizeof(gdt[0]) * (NR_SEG + 1));
  set_tr(KSEL(SEG_TSS));
#else
  SegDesc *gdt = CPU->gdt;
  TSS32 *tss = &CPU->tss;
  gdt[SEG_KCODE] = SEG32(STA_X | STA_R,   0,     0xffffffff, DPL_KERN);
  gdt[SEG_KDATA] = SEG32(STA_W,           0,     0xffffffff, DPL_KERN);
  gdt[SEG_UCODE] = SEG32(STA_X | STA_R,   0,     0xffffffff, DPL_USER);
  gdt[SEG_UDATA] = SEG32(STA_W,           0,     0xffffffff, DPL_USER);
  gdt[SEG_TSS]   = SEG16(STS_T32A,      tss, sizeof(*tss)-1, DPL_KERN);
  set_gdt(gdt, sizeof(gdt[0]) * NR_SEG);
  set_tr(KSEL(SEG_TSS));
#endif
}
