#include <stdio.h>
#include <setjmp.h>

int main()
{

    jmp_buf context;
    int status = setjmp(context);
    if (status == 0)
    {
        printf("setjmp = 0\n");
        longjmp(context, 1);
    }
    else
    {
         status = setjmp(context);
         printf("setjmp = 1\n");
         longjmp(context, 1);
    }

    return 0;
}
