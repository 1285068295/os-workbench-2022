#include <stdio.h>

typedef struct STUDENT
{
  int class;
  int age;
  char *name; //姓名
} Student;

void change(Student stu)
{
  stu.class -= 1;
  stu.age -= 1;
  stu.name = "xxxxx";
}


int main()
{

  Student *stu;
  for (int i = 1; i < 10; i++)
  {
    Student s = {10 *i, 20*i, "zhangsan"};
    change(s);
    printf("s.class=%d  s.age=%d s.name=%s\n", s.class, s.age, s.name);
    stu = &s;
  }
  printf("stu.class=%d  stu.age=%d stu.name=%s\n", stu->class, stu->age, stu->name);
  change(*stu);
  printf("stu.class=%d  stu.age=%d stu.name=%s\n", stu->class, stu->age, stu->name);
  return 0;
}

