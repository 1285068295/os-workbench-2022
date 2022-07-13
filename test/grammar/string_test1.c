#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void change_str(char* str){
    char* res = (char*)malloc(10* sizeof(char));
    strcpy(res, "xxxx");
    str = res;
}

void free_str(char* str){
    free(str);
}


int main()
{

    char s[] = "123";
    int num = atoi(s);
    printf("num=%d \n", num);

    char name[100];
    char *description;
    strcpy(name, "Zara Ali");

    /* 动态分配内存 */
    description = (char *)malloc(200 * sizeof(char));
    
    char *str = "123"; // str[3] ='\0'
    printf("len = %ld\n", strlen(str)); // len = 3
    for (int i = 0; i < strlen(str); i++)
    {
      printf("str[%d] = %c\n", i, str[i]); // len = 3
    }
    
    

    char *blankStr = (char *)malloc( 3 * sizeof(char) );
    for (int i = 0; i < 3; i++)
    {
       blankStr[i] = 'a' + i;
    }
    for (int i = 0; i < strlen(blankStr); i++)
    {
      printf("str[%d] = %c\n", i, blankStr[i]); // len = 3
    }

    // 直接释放会报错
    // char* free_str = "abcdef";
    // free(free_str);

    // 使用指针创建字符串：char *s = “hello”;等价于 const char *s = “hello”;
    // 使用数组创建字符串：char s[] = “hello”;
    // 它们的区别在于，使用指针创建的字符串只能读，使用数组创建的字符串不仅可以读，而且可以修改。

    char *p = (char *)malloc(sizeof(char *) * 3);
    p[0] = '1';
    p[1] = '2';
    printf("p=-%s-\n", p);// p=-12-


    strcpy(p,"abc");
    printf("p=-%s-\n", p);
    strcpy(p,"123");
    printf("p=-%s-\n", p);

    // char *t = "111";
    // t[1]= '2';// Segmentation fault
    // // printf("t=-%s-\n", t);

    char* null_str = NULL;
    
    printf("null_str=%s\n", null_str);// null_str=(null)
     printf("sizeof(NULL)=%ld\n", sizeof(null_str));// 8
    //printf("sizeof(NULL)=%ld\n", strlen(null_str));// Segmentation fault
     printf("sizeof(char)=%ld\n", sizeof(char));// 1

    char * n = "ghi";
    // free(n); munmap_chunk(): invalid pointer
    change_str(n);
    printf("n=%s\n", n);// n=ghi n不会改变

    
    
    char * nn = (char*)malloc(4 * sizeof(char));
    strcpy(nn, "abc");
    change_str(nn);
    printf("change_str(nn)=%s\n", nn);// change_str(nn)=abc
    free_str(nn);
    printf("free_str(nn)=%s\n", nn);// `free_str(nn)=`
    // free(nn);
    // printf("free(nn)=%s\n", nn);// 上面的free_str 已经释放了 free(): double free detected in tcache 2 Aborted

    return 1;
}