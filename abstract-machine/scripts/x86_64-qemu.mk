include $(AM_HOME)/scripts/isa/x86_64.mk
include $(AM_HOME)/scripts/platform/qemu.mk

AM_SRCS := x86/qemu/start64.S \
           x86/qemu/trap64.S \
           x86/qemu/trm.c \
           x86/qemu/cte.c \
           x86/qemu/ioe.c \
           x86/qemu/vme.c \
           x86/qemu/mpe.c
# -S 在模拟器初始化完成 (CPU Reset) 后暂停
# -s 启动 gdb 调试服务器，可以使用 gdb 调试模拟器中的程序

# $(QEMU) -gdb tcp::1737 -S -serial stdio -smp $(smp) -drive format=raw,file=./build/kernel-$(ARCH) -no-reboot -no-shutdown
run: build-arg
	@qemu-system-x86_64  -S -s $(QEMU_FLAGS)
