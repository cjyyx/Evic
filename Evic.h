#ifndef EVIC_H
#define EVIC_H
#include <stdlib.h>

#define MAX_TOKEN_LEN 16

#define EVIC_DEBUG_MODE 0

/* 字符对象 */
typedef struct
{
    int TokenClass;
    void *next;

    double number;
    char op[4];
    char token[MAX_TOKEN_LEN];
    double *val;
    double (*pfunc)(double x);
} Token;

/* 表达式对象 */
typedef struct
{
    /* 指向头个字符 */
    Token *head;
    /* 是否被编译为后缀表达式 */
    int compiled;
} EVIC_expr;

/* 类似于eval对表达式求值
expr是表达式字符串，reBuf存放结果
成功反1，失败反0 */
int EVIC_Eval(char *expr, double *rtBuf);

/* 创建并初始化表达式对象
将中缀表达式切割成一个个字符或数字对象，存放在表达式对象中
成功反1，失败反0 */
int EVIC_Cut(char *expr, EVIC_expr **ec_expr);

/* 打印表达式，存在块中，记得free
失败反NULL */
char *EVIC_Print(EVIC_expr *ec_expr);

/* 将表达式的字符与变量联系
成功反1，失败反0 */
int EVIC_ValueLink(EVIC_expr *ec_expr, char *tk, double *val);

/* 将表达式的字符与变量联系
成功反1，失败反0 */
int EVIC_ConstLink(EVIC_expr *ec_expr, char *tk, double ct);

/* 将表达式的字符与函数联系
成功反1，失败反0 */
int EVIC_FuncLink(EVIC_expr *ec_expr, char *tk, double (*pfunc)(double x));

/* 完成字符的联系，转换为后缀表达式
成功反1，失败反0 */
int EVIC_Compile(EVIC_expr **ec_expr);

/* 对表达式计算
成功反1，失败反0 */
int EVIC_Calculate(EVIC_expr *ec_expr, double *rt);

/* 释放表达式空间
成功反1，失败反0 */
int EVIC_Free(EVIC_expr *ec_expr);

#endif