#include <stdio.h>
#include <stdlib.h>

struct Student
{
    char* class;
};

struct Person
{
    char *name;
    struct Student stu;
};

int main()
{
    //   struct co *coroutine = (struct co *)malloc(sizeof(struct co));
    struct Person* p = (struct Person*) malloc(sizeof(struct Person));

    p->stu.class = "806";
    printf("p->stu.class = %s\n", p->stu.class);
    free(p);
    printf("sizeof(Person)=%ld\n",sizeof(struct Person));

    return 0;

}