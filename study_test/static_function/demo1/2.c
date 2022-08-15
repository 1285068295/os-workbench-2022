#include "2.h"
#include <stdio.h>
static  void my_static()
{
	printf("我是外边的static的函数，我被调用了\n");
}

void use_my_static(struct stru *h)
{
	h->f=my_static;
};//作用要知道哦
 