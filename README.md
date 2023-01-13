# 库Evic
类似TinyExpr，可以在C语言中实现eval函数。

具体效果参考Python、JavaScript中的eval函数。

# 函数
```c
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

```

# 求值过程
- EVIC_cut:将中缀表达式切割成一个个字符或数字对象
- EVIC_valueLink:将表达式的字符与变量联系
- EVIC_funLink:将表达式的字符与函数联系
- EVIC_compile:完成字符的联系，转换为后缀表达式，返回表达式对象
- EVIC_calculate:对表达式计算

# 快速开始

## 计算单独表达式
```c
#include "Evic.c"

int main(int argc, char const *argv[])
{
    // 结果存放在rt中
    double rt;

    // 对表达式(ln(e)*ln(e^2))^(20*sin(pi/(3*ln(e^2))))求值
    if (
        EVIC_Eval(
            "(ln(e)*ln(e^2))^(20*sin(pi/(3*ln(e^2))))", 
            &rt
            )
        )
    {
        printf("result: %lf\n", rt);
    }
    else
    {
        printf("eval failed\n");
    }
}
```

## 计算含变量的表达式
```c
#include "Evic.c"

int main(int argc, char const *argv[])
{
    EVIC_expr *ec_expr;
    char *info;
    int rt;
    double x, y, rrr;
    char expr[1024] = "x";

    rt = EVIC_Cut(expr, &ec_expr);
    if (rt == 0)
    {
        printf("cut failed\n");
    }
    else
    {
        info = EVIC_Print(ec_expr);
        printf(info);
        free(info);
    }

    EVIC_ValueLink(ec_expr, "x", &x);
    EVIC_ValueLink(ec_expr, "y", &y);

    rt = EVIC_Compile(&ec_expr);
    if (rt == 0)
    {
        printf("compile failed\n");
    }
    else
    {
        printf("------------------------\nresult:\n");
        info = EVIC_Print(ec_expr);
        printf(info);
        free(info);
    }

    x = 2;
    y = 20;
    rt = EVIC_Calculate(ec_expr, &rrr);
    if (rt == 0)
    {
        printf("calculat failed\n");
    }
    else
    {
        printf("------------------------\nresult: %lf", rrr);
    }

    EVIC_Free(ec_expr);
}

```
