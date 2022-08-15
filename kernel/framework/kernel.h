#include <am.h>

/**
 * @brief MODULE用来声明一个模块，而使用MODULE_DEF来真正的定义这个模块
 *        os是一个操作系统的模块，可以看成是使用C实现的面向对象的编程，可曾强代码的可读性
 *        ##是C语言用来拼接标识符的机制，sys ## tem，可以得到system
 * 
 *        如果你阅读上述代码感到障碍，你可能需要补充：
 *        (1) typedef 类型别名定义, (2) extern 关键字, (3) C11 结构体初始化的知识
 */


#define MODULE(mod) \
  typedef struct mod_##mod##_t mod_##mod##_t; \
  extern mod_##mod##_t *mod; \
  struct mod_##mod##_t

#define MODULE_DEF(mod) \
  extern mod_##mod##_t __##mod##_obj; \
  mod_##mod##_t *mod = &__##mod##_obj; \
  mod_##mod##_t __##mod##_obj


/*
// 上面的宏替换后容易理解
#define MODULE(mod_param) \
  typedef struct mod_##mod_param##_t mod_##mod_param##_t; \
  extern mod_##mod_param##_t *mod_param; \
  struct mod_##mod_param##_t

#define MODULE_DEF(mod_param) \
  extern mod_##mod_param##_t __##mod_param##_obj; \
  mod_##mod_param##_t *mod_param = &__##mod_param##_obj; \
  mod_##mod_param##_t __##mod_param##_obj
*/

MODULE(os) {
  void (*init)();
  void (*run)();
};

// MODULE(os)展开后如下
// typedef struct mod_os_t mod_os_t; 
// extern mod_os_t *os; 
// struct mod_os_t {
//   void (*init)();
//   void (*run)();
// };



MODULE(pmm) {
  void  (*init)();
  void *(*alloc)(size_t size);
  void  (*free)(void *ptr);
};



 
/**************************************锁函数定义****************************************************/
// L1 pmm
void lock();
void unlock();
void alloc(int sz);

/**
 * @brief 妙哉
 * WARNING: CANNOT BREAK/RETURN in atomic{}
 *         (lock(), 0) 逗号表达式 总是会先取后面的值 
 */
 #define atomic \
  for(int __i = (lock(), 0); __i < 1; __i++, unlock())
// atomic {body}  ==>                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 

// lock();   success 进入后面的执行
//    i = 0
//    check i < 1 => yes
//    body
//    i++
// unlock();
// check i < 1 => no  exit loop


/**************************************锁函数定义****************************************************/
 