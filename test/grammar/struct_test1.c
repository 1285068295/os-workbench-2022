#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct InitMember
{
    int first;
    double second;
    char *third;
    float four;
};

typedef struct _person {
    char* firstName;
    char* lastName;
    char* title;
    int age;
} Person;

void initializePerson(Person *person, const char* fn,
        const char* ln, const char* title, int age) {
    person->firstName = (char*) malloc(strlen(fn) + 1);
    strcpy(person->firstName, fn);
    person->lastName = (char*) malloc(strlen(ln) + 1);
    strcpy(person->lastName, ln);
    person->title = (char*) malloc(strlen(title) + 1);
    strcpy(person->title, title);
    person->age = age;
}



int main()
{

    // 初始化方式1
    struct InitMember test1 = {-10, 3.141590, "method one", 0.25};
    
    // 初始化方式2
    struct InitMember test2;
    test2.first = -10;
    test2.second = 3.141590;
    test2.third = "method two";
    test2.four = 0.25;
    
    // 初始化方式3
    struct InitMember test3 = {
        .second = 3.141590,
        .third = "method three",
        .first = -10,
        .four = 0.25};
    
    // 初始化方式4
    struct InitMember test4 = {
        second : 3.141590,
        third : "method three",
        first : -10,
        four : 0.25
    };


    Person* p   = (Person*) malloc(sizeof(Person));

    printf("p->title = %s\n", p->title); //  p->title = (null)
    p->title = (char *)malloc(sizeof(char) * 1);
    printf("p->title = %s\n", p->title); // `p->title =  `
    p->title = NULL;
    printf("p->title = %s\n", p->title); //  p->title = (null)
}