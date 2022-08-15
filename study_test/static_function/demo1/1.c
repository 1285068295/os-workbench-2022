#include <stdio.h>
#include "2.h"
 
/**
 * @brief 静态函数在外部使用的方法
 *        利用了函数指针
 * 
 */
void main()
{
	struct stru stru1;
	use_my_static(&stru1);
	stru1.f();
}