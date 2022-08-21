#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>


#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)



/////////////////////////////////////////////////////////////////////////////////////

/* 功能函数声明 */
void myPrintf(char *s, ...);				// 需要实现的目标函数
void printNum(unsigned long num, int base); // 通用数字打印函数 
void printDeci(int dec);					// 打印十进制数
void printOct(unsigned oct);				// 打印八进制数
void printHex(unsigned hex);				// 打印十六进制数
void printAddr(unsigned long addr);			// 打印地址
void printStr(char *str);					// 打印字符串
void printFloat(double f);					// 打印浮点数

/*
 * 函数名: printNum()
 * 函数功能: 通用数字打印函数可以把整型值打印成
 *           10进制数,8进制数,2进制数,16进制数
 * 参数: 1.需要打印的整数,无符号长整型是为了兼容
 *         地址格式打印; 2.打印的进制
 *  返回值: 无
*/
void printNum(unsigned long num, int base)
{
  /* 递归结束条件 */
  if (num == 0)
  {
    return;
  }

  /* 继续递归 */
  printNum(num / base, base);

  /* 逆序打印结果 */
  putch("0123456789abcdef"[num % base]);
}

/*
 * 函数名: printDeci
 * 函数功能: 打印十进制数
 * 参数: 十进制整数
 * 返回值: 无
 */
void printDeci(int dec)
{
  /* 处理有符号整数为负数时的情况 */
  if (dec < 0)
  {
    putch('-');
    dec = -dec; // 该操作存在溢出风险:最小的负数没有对应的正数
  }

  /* 处理整数为时0的情况 */
  if (dec == 0)
  {
    putch('0');
    return;
  }
  else
  {
    printNum(dec, 10); // 打印十进制数
  }
}

/*
 * 函数名: printOct
 * 函数功能: 打印八进制整数
 * 参数: 无符号整数
 * 返回值: 无
 */
void printOct(unsigned oct)
{
  if (oct == 0) // 处理整数为0的情况
  {
    putch('0');
    return;
  }
  else
  {
    printNum(oct, 8); // 打印8进制数
  }
}

/*
 * 函数名: printHex
 * 函数功能: 打印十六进制整数
 * 参数: 无符号整数
 * 返回值: 无
 */
void printHex(unsigned hex)
{
  if (hex == 0) // 处理整数为0的情况
  {
    putch('0');
    return;
  }
  else
  {
    printNum(hex, 16); // 打印十六进制数
  }
}

/*
 * 函数名: printAddr
 * 函数功能: 打印地址
 * 参数: 待打印的地址
 * 返回值: 无
 */
void printAddr(unsigned long addr)
{
  /* 打印前导"0x" */
  putch('0');
  putch('x');

  /* 打印地址:格式和十六进制一样 */
  printNum(addr, 16);
}

/*
 * 函数名: printStr
 * 函数功能: 打印字符串
 * 参数: 字符串地址
 * 返回值: 无
 */
void printStr(char *str)
{
  int i = 0;

  while (str[i] != '\0')
  {
    putch(str[i++]);
  }
}

/*
 * 函数名: printFloat
 * 函数功能: 打印浮点数
 * 参数: 待打印浮点数
 * 返回值: 无
 */
void printFloat(double f)
{
  int temp;

  /* 先打印整数部分 */
  temp = (int)f;
  printNum(temp, 10);

  /* 分隔点 */
  putch('.');

  /* 打印小数部分 */
  f -= temp;
  if (f == 0)
  {
    /* 浮点型数据至少六位精度 */
    for (temp = 0; temp < 6; temp++)
    {
      putch('0');
    }
    return;
  }
  else
  {
    temp = (int)(f * 1000000);
    printNum(temp, 10);
  }
}


/**
 * @brief 暂时先不考虑线程安全问题
 */
int printf(const char *fmt, ...)
{
  // panic("Not implemented");

  int i = 0;

  /* 可变参第一步 */
  va_list va_ptr;

  /* 可变参第二部 */
  va_start(va_ptr, fmt);

  /* 循环打印所有格式字符串 */
  while (fmt[i] != '\0')
  {
    /* 普通字符正常打印 */
    if (fmt[i] != '%')
    {
      putch(fmt[i++]);
      continue;
    }

    /* 格式字符特殊处理 */
    switch (fmt[++i]) // i先++是为了取'%'后面的格式字符
    {
      /* 根据格式字符的不同来调用不同的函数 */
    case 'd':
      printDeci(va_arg(va_ptr, int));
      break;
    case 'o':
      printOct(va_arg(va_ptr, unsigned int));
      break;
    case 'x':
      printHex(va_arg(va_ptr, unsigned int));
      break;
    case 'c':
      putch(va_arg(va_ptr, int));
      break;
    case 'p':
      printAddr(va_arg(va_ptr, unsigned long));
      break;
    
    case 'f':
      panic("Not implemented printf %f");
      break;
    // double类型的编译会有问题 error: SSE register argument with SSE disabled
    /* 参考文档 Linux 内核添加浮点运算出现的问题 https://www.51cto.com/article/666401.html
     * 没明白的是为什么gcc识别到到 我这个stdio.c是内核态？？？
     */
    // case 'f':
    //   printFloat(va_arg(va_ptr, double));
    //   break;

    
    case 's':
      printStr(va_arg(va_ptr, char *));
      break;
    default:
      break;
    }

    i++; // 下一个字符
  }

  /* 可变参最后一步 */
  va_end(va_ptr);
  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////





















int vsprintf(char *out, const char *fmt, va_list ap) {
  panic("Not implemented");
}

int sprintf(char *out, const char *fmt, ...) {
  panic("Not implemented");
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
