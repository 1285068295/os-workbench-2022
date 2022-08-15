#include <stdio.h>
#include <stdlib.h>
#include <string.h>



/**
 * realloc() 函数学习
 * 参考博客 https://blog.csdn.net/hackerain/article/details/7954006
 * 
 * 
 */




/**
 * @brief 函数内部 不能修改指针的指向，但是可以改变指针指向地址的内容
 * 
 * @param str1 
 * @param str2 
 * @return char* 
 */
char* func1(char* str1, char* str2){

    // char* str = (char*) malloc(strlen(str1) + strlen(str2));
    // strcpy(str, str1);
    // strcat(str, str2);
    // // 这里可以释放？？？
    // // free(str1);
    // str1 = str;

    str1 =  (char*) malloc(4);
    strcpy(str1, "jjj");

    return str1;
}

/**
 * @brief 拼接字符串
 * 
 * @param str1 
 * @param str2 
 */
void func2(char* str1, char* str2){
   printf("func2 拼接前 String = %s,  Address = %p\n", str1, str1); 
   /* 重新分配内存 */
   str1 = (char *) realloc(str1, strlen(str1) + strlen(str2) + 1);
   strcat(str1, str2);
   printf("func2 拼接后 String = %s,  Address = %p\n", str1, str1);
}


/**
 * @brief 缩短字符串
 * 
 * @param str1 
 */
void func3(char* str){
    /* 重新分配内存 */
  
    str = (char *)realloc(str, 4);
    strcpy(str, "ooo");
    printf("func3 缩短后 len = %ld String = %s,  Address = %p\n", strlen(str), str, str);
}


/**
 * @brief malloc分配的字符串可以任意的修改
 * 
 * @param str 
 */
void change_str(char *str){
    str[0] = 'x';
}

int main()
{
    char *str1 = (char *)malloc(7);
    strcpy(str1, "runoob");
    printf("func1 String = %s,  Address = %p\n", str1, str1);

    free(str1);
    str1 = (char *)malloc(70000);
    strcpy(str1, "123456");
    printf("func1 String = %s,  Address = %p\n", str1, str1);
    free(str1);
    return (0);
}