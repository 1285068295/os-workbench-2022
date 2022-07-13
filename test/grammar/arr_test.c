#include <stdio.h>
 
// 数组使用学习 https://blog.csdn.net/Laoynice/article/details/79196993

// int a[10];//静态数组，在堆栈上创建

// int n;
// //C语言的malloc函数进行书写
// int *p = (int *)malloc(n * sizeof(int));

int test1(int *p){
	for(int i=0;i<5;i++){
		printf("test1 %d \n",p[i]);//我们在这里还可以用）*(p+i)来输出数组中的值
	}	
}
  
int test2(int a[]){
	for(int i=0;i<5;i++){
		printf("test2 %d \n",a[i]);
	}
}

/**
 * @brief 只传数组的情况下 函数内部不能得到数组的长度！！！
 * 
 * @param a 
 * @return int 
 */
int test3(int a[]){
    // 这里的n始终是2
    // a是函 数参数，到了本函数中，a只是一个指针（地址，系统在本函数运行时，
    // 是不知道a所表示的地址有多大的数据存储空间，这里只是告诉函数：一个数据 空间首地址 ），
    // 所以，sizoef(a)的结果是指针变量a占内存的大小 ，一般在64位机上是8个字节。 int类型是4个字节，
    // 所以，结果永远是2
    // int n = sizeof(a)/sizeof(int);
    int n = 3;
    for (int i = 0; i < n; i++)
    {
        printf("test3 %d\n", a[i]);
        a[i]++;
    }
}

/**
 * @brief 要在函数内存得到数组参数的长度 必须作为参数传进来
 * 
 * @param a 
 * @param len 
 * @return int 
 */
int test4(int a[], int len)
{
    for (int i = 0; i < len; i++)
    {
        printf("test4 %d\n", a[i]);
        a[i]++;
    }
}


// 编译不能通过 多维数组的定义必须有一个除第一个之外的所有维度的边界 如下test5
// void test5(int a[][]){
//     for (int i = 0; i < 5; i++)
//     {
//         for (int j = 0; j < 5; j++)
//         {
//             printf("%d ", a[i][j]);
//         }
//     }
// }

// 修正后的test5
void test5(int a[][5]){
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            printf("%d ", a[i][j]);
        }
    }
}


/**
 * @brief 二维数组参数使用
 * 
 * @param m 
 * @param n 
 * @param p 
 */
void test6(int  m,int  n,int **p){//m,n是行和列，
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            printf("test6 %d \n", *((int *)p + n * i + j));
        }
    }
}
 

/**
 * @brief 批量初始化数组元素
 * 
 */
void test7(){
    int a[5] = {[0 ... 4] = 1};
    for (int i = 0; i < sizeof(a) / sizeof(int); i++)
    {
        printf("test7 %d\n", a[i]);
    }
}


int main(){
	int a[5] = {1,2,3,4,5},*p;
	p = a;
	test1(a);
    test1(p);
    test2(a);
    test2(p);
    test3(a);
    test3(p);
    int n = sizeof(a)/sizeof(int);
    test4(a, n);
    test4(p, n);
    
    
    test4(p, n);



    int b[5][5], i, j;
    for (i = 0; i < 5; i++)
    {
        for (j = 0; j < 5; j++)
        {
            b[i][j] = i * 5 + (j + 1);
        }
    }
    test6(5, 5, (int **)b);
    // test6(5, 5, b);  // 这里会编译报错


    test7();

    // 变量数组
   int len = 10;
   int arr[len];
   for (int i = 0; i < len; i++)
   {
        printf("%d\n", arr[i]);
   }
    

}