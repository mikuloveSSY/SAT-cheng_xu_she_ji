#pragma once
//防止多次编译导致重复定义
#include<stdio.h>
#include<stdlib.h>
#include<cstring>
#include<math.h>
#include<time.h>

#define OK 1;
#define FALSE 0;
#define ERROR -1;

//变元结构
typedef struct L{
    int x;//变元
    L* next;
}L;
//子句结构
typedef struct Clause{
    L Lhead;
    Clause* next;
}Clause;
//定义读入的SAT结构
typedef struct SAT{
    char name[20];
    int m;//变元数
    int n;//子句数
    Clause Chead;//数据
}SAT;

// //函数声明
// SAT* Readcnf(char* file);
// int Reoutput(SAT *sat);