#include <stdio.h>
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>

#include "Evic.h"

/* 创建表达式对象 */
EVIC_expr *NewExpr();

static int malloc_num, free_num;

enum
{
    DEFAULT_TOKEN,
    NUMBER_TOKEN,
    OP_TKOEN,
    UNKNOW_TOKEN,
    VALUE_TOKEN,
    FUNC_TOKEN
};

/* 创建新字符，存在块中 */
Token *NewToken();
/* 复制字符，存在块中 */
Token *Token_Copy(Token *ptk);
/* 字符值TODO */
int Token_Value(Token *ptk, double *rt);

/* 往表达式加入字符 */
int Expr_Expend(EVIC_expr *ec_expr, Token *ptk);
/* 返回表达式的末尾 */
Token *Expr_End(EVIC_expr *ec_expr);

/* 字符栈对象 */
typedef EVIC_expr *ExprStack;
/* 创建字符栈对象，存在块中，记得free
head指栈顶 */
ExprStack NewExprStack();
/* 复制元素并入栈 */
int ExprStack_Push(ExprStack est, Token *ptk);
/* 出栈 */
int ExprStack_Pop(ExprStack est, Token **ptk);

/* 定义了运算符的优先级 */
int OpPriority(char ope);

Token *NewToken()
{
    malloc_num++;
    Token *ptk = ((Token *)malloc(sizeof(Token)));
    ptk->next = NULL;
    ptk->TokenClass = DEFAULT_TOKEN;
    return ptk;
}

EVIC_expr *NewExpr()
{
    malloc_num++;
    return (EVIC_expr *)malloc(sizeof(EVIC_expr));
}

int Expr_Expend(EVIC_expr *ec_expr, Token *ptk)
{
    if (ec_expr->head == NULL)
    {
        ec_expr->head = ptk;
        return 1;
    }

    Token *pttk = ec_expr->head;
    for (; pttk->next != NULL; pttk = pttk->next)
        ;
    pttk->next = ptk;
    return 1;
}
Token *Expr_End(EVIC_expr *ec_expr)
{
    if (ec_expr->head == NULL)
    {
        return NULL;
    }

    Token *pttk = ec_expr->head;
    for (; pttk->next != NULL; pttk = pttk->next)
        ;
    return pttk;
}

int EVIC_Cut(char *expr, EVIC_expr **ec_expr)
{
    EVIC_expr *pt_expr = NewExpr();
    pt_expr->head = NULL;
    pt_expr->compiled = 0;

    char(*pch);
    int offset;
    double lft;
    Token *p_tk, *pt_tk;

    for (pch = expr; (*pch) != '\0';)
    {
        if (EVIC_DEBUG_MODE)
            printf("parse:|%s\n", pch);
        if ((*pch) >= '0' && (*pch) <= '9')
        {
            sscanf(pch, "%lf%n", &lft, &offset);
            pt_tk = NewToken();
            pt_tk->TokenClass = NUMBER_TOKEN;
            pt_tk->number = lft;
            Expr_Expend(pt_expr, pt_tk);
            pch += offset;
            continue;
        }
        else if ((*pch) == '-')
        {
            Token *pendtk = Expr_End(pt_expr);
            if (pendtk == NULL)
            {
                if ((*(pch + 1)) >= '0' && (*(pch + 1)) <= '9')
                {
                    sscanf(pch, "%lf%n", &lft, &offset);
                    pt_tk = NewToken();
                    pt_tk->TokenClass = NUMBER_TOKEN;
                    pt_tk->number = lft;
                    Expr_Expend(pt_expr, pt_tk);
                    pch += offset;
                    continue;
                }
                else if ((*(pch + 1)) >= 'a' && (*(pch + 1)) <= 'z')
                {
                    pt_tk = NewToken();
                    pt_tk->TokenClass = NUMBER_TOKEN;
                    pt_tk->number = -1.0;
                    Expr_Expend(pt_expr, pt_tk);
                    pt_tk = NewToken();
                    pt_tk->TokenClass = OP_TKOEN;
                    pt_tk->op[0] = '*';
                    pt_tk->op[1] = '\0';
                    Expr_Expend(pt_expr, pt_tk);
                    pch++;
                    continue;
                }
                else
                {
                    return 0;
                }
            }
            else if (pendtk != NULL)
            {
                if (pendtk->TokenClass == OP_TKOEN)
                {
                    if (strcmp(pendtk->op, "(") == 0)
                    {
                        if ((*(pch + 1)) >= '0' && (*(pch + 1)) <= '9')
                        {
                            sscanf(pch, "%lf%n", &lft, &offset);
                            pt_tk = NewToken();
                            pt_tk->TokenClass = NUMBER_TOKEN;
                            pt_tk->number = lft;
                            Expr_Expend(pt_expr, pt_tk);
                            pch += offset;
                            continue;
                        }
                        else if ((*(pch + 1)) >= 'a' && (*(pch + 1)) <= 'z')
                        {
                            pt_tk = NewToken();
                            pt_tk->TokenClass = NUMBER_TOKEN;
                            pt_tk->number = -1.0;
                            Expr_Expend(pt_expr, pt_tk);
                            pt_tk = NewToken();
                            pt_tk->TokenClass = OP_TKOEN;
                            pt_tk->op[0] = '*';
                            pt_tk->op[1] = '\0';
                            Expr_Expend(pt_expr, pt_tk);
                            pch++;
                            continue;
                        }
                        else
                        {
                            return 0;
                        }
                    }
                    else if (strcmp(pendtk->op, ")") == 0)
                    {
                        pt_tk = NewToken();
                        pt_tk->TokenClass = OP_TKOEN;
                        pt_tk->op[0] = *pch;
                        pt_tk->op[1] = '\0';
                        Expr_Expend(pt_expr, pt_tk);
                        pch += 1;
                        continue;
                    }
                    else
                    {

                        sscanf(pch, "%lf%n", &lft, &offset);
                        pt_tk = NewToken();
                        pt_tk->TokenClass = NUMBER_TOKEN;
                        pt_tk->number = lft;
                        Expr_Expend(pt_expr, pt_tk);
                        pch += offset;
                        continue;
                    }
                }
                else if ((pendtk->TokenClass == NUMBER_TOKEN) ||
                         (pendtk->TokenClass == UNKNOW_TOKEN))
                {
                    pt_tk = NewToken();
                    pt_tk->TokenClass = OP_TKOEN;
                    pt_tk->op[0] = *pch;
                    pt_tk->op[1] = '\0';
                    Expr_Expend(pt_expr, pt_tk);
                    pch += 1;
                    continue;
                }
                else
                {
                    return 0;
                }
            }
        }
        else if (
            ((*pch) == '(') ||
            ((*pch) == ')') ||
            ((*pch) == '+') ||
            ((*pch) == '*') ||
            ((*pch) == '/') ||
            ((*pch) == '^'))
        {
            pt_tk = NewToken();
            pt_tk->TokenClass = OP_TKOEN;
            pt_tk->op[0] = *pch;
            pt_tk->op[1] = '\0';
            Expr_Expend(pt_expr, pt_tk);
            pch += 1;
            continue;
        }
        else if ((*pch) >= 'a' && (*pch) <= 'z')
        {
            int t;
            pt_tk = NewToken();
            pt_tk->TokenClass = UNKNOW_TOKEN;
            for (t = 0; pch[t] >= 'a' && pch[t] <= 'z'; t++)
            {
                pt_tk->token[t] = pch[t];
            }
            pt_tk->token[t] = '\0';
            Expr_Expend(pt_expr, pt_tk);
            pch += t;
            continue;
        }
        else if ((*pch) == ' ')
        {
            pch++;
            continue;
        }
        else
        {
            return 0;
        }
    }

    *ec_expr = pt_expr;
    return 1;
}

char *EVIC_Print(EVIC_expr *ec_expr)
{
    char *info = (char *)malloc(sizeof(char) * 1024);
    info[0] = '\0';

    Token *pttk = ec_expr->head;
    char ts[64];

    strcat(info, "[");
    for (; pttk != NULL; pttk = pttk->next)
    {
        switch (pttk->TokenClass)
        {
        case NUMBER_TOKEN:
            sprintf(ts, "%.2lf,", pttk->number);
            strcat(info, ts);
            break;
        case OP_TKOEN:
            sprintf(ts, "%s,", pttk->op);
            strcat(info, ts);
            break;
        case UNKNOW_TOKEN:
            sprintf(ts, "%s,", pttk->token);
            strcat(info, ts);
            break;
        case VALUE_TOKEN:
            sprintf(ts, "%s(=%.2lf),", pttk->token, *(pttk->val));
            strcat(info, ts);
            break;
        case FUNC_TOKEN:
            sprintf(ts, "(linked)%s,", pttk->token);
            strcat(info, ts);
            break;
        default:
            break;
        }
    }
    strcat(info, "]\n");

    return info;
}

int EVIC_Free(EVIC_expr *ec_expr)
{
    if (ec_expr->head != NULL)
    {
        Token *pttk, *nextpttk;
        for (pttk = ec_expr->head; pttk != NULL; pttk = nextpttk)
        {
            nextpttk = pttk->next;
            free_num++;
            free(pttk);
        }
    }
    free_num++;
    free(ec_expr);

    return 1;
}

int EVIC_ValueLink(EVIC_expr *ec_expr, char *tk, double *val)
{
    if (ec_expr->head == NULL)
    {
        return 1;
    }

    Token *pttk = ec_expr->head;
    for (; pttk != NULL; pttk = pttk->next)
    {
        if (pttk->TokenClass == UNKNOW_TOKEN)
        {
            if (strcmp(pttk->token, tk) == 0)
            {
                pttk->TokenClass = VALUE_TOKEN;
                pttk->val = val;
            }
        }
    }
    return 1;
}

int EVIC_ConstLink(EVIC_expr *ec_expr, char *tk, double ct)
{
    if (ec_expr->head == NULL)
    {
        return 1;
    }

    Token *pttk = ec_expr->head;
    for (; pttk != NULL; pttk = pttk->next)
    {
        if (pttk->TokenClass == UNKNOW_TOKEN)
        {
            if (strcmp(pttk->token, tk) == 0)
            {
                pttk->TokenClass = NUMBER_TOKEN;
                pttk->number = ct;
            }
        }
    }
    return 1;
}

int EVIC_FuncLink(EVIC_expr *ec_expr, char *tk, double (*pfunc)(double x))
{
    if (ec_expr->head == NULL)
    {
        return 1;
    }

    Token *pttk = ec_expr->head;
    for (; pttk != NULL; pttk = pttk->next)
    {
        if (pttk->TokenClass == UNKNOW_TOKEN)
        {
            if (strcmp(pttk->token, tk) == 0)
            {
                pttk->TokenClass = FUNC_TOKEN;
                pttk->pfunc = pfunc;
            }
        }
    }
    return 1;
}

ExprStack NewExprStack()
{
    malloc_num++;
    ExprStack est = (ExprStack)malloc(sizeof(EVIC_expr));
    est->head = NULL;
    return est;
}

int ExprStack_Push(ExprStack est, Token *ptk)
{
    Token *tptk = Token_Copy(ptk);
    if (est->head == NULL)
    {
        est->head = tptk;
    }
    else
    {
        tptk->next = est->head;
        est->head = tptk;
    }
    return 1;
}

int ExprStack_Pop(ExprStack est, Token **ptk)
{
    if (est->head == NULL)
    {
        return 0;
    }
    else
    {
        (*ptk) = est->head;
        est->head = est->head->next;
        (*ptk)->next = NULL;
    }
    return 1;
}

int OpPriority(char ope)
{
    switch (ope)
    {
    case '+':
        return 1;
        break;
    case '-':
        return 1;
        break;
    case '*':
        return 2;
        break;
    case '/':
        return 2;
        break;
    case '^':
        return 3;
        break;
    default:
        return -1;
        break;
    }
}

int Token_Value(Token *ptk, double *rt)
{
    switch (ptk->TokenClass)
    {
    case NUMBER_TOKEN:
        *rt = ptk->number;
        break;
    case VALUE_TOKEN:
        *rt = *(ptk->val);
        break;
    default:
        return 0;
        break;
    }
    return 1;
}

Token *Token_Copy(Token *ptk)
{
    Token *tptk = NewToken();
    tptk->TokenClass = ptk->TokenClass;

    switch (ptk->TokenClass)
    {
    case NUMBER_TOKEN:
        tptk->number = ptk->number;
        break;
    case OP_TKOEN:
        strcpy(tptk->op, ptk->op);
        break;
    case UNKNOW_TOKEN:
        strcpy(tptk->token, ptk->token);
        break;
    case VALUE_TOKEN:
        strcpy(tptk->token, ptk->token);
        tptk->val = ptk->val;
        break;
    case FUNC_TOKEN:
        strcpy(tptk->token, ptk->token);
        tptk->pfunc = ptk->pfunc;
        break;
    default:
        break;
    }
    return tptk;
}

int EVIC_Compile(EVIC_expr **ec_expr)
{
    ExprStack opst, numst;
    Token *pttk, *tpttk, *tpnextk;

    if ((*ec_expr)->head == NULL)
    {
        return 1;
    }

    int rrr = 0;
    rrr += 1 - EVIC_ConstLink(*ec_expr, "e", M_E);
    rrr += 1 - EVIC_ConstLink(*ec_expr, "pi", M_PI);
    rrr += 1 - EVIC_FuncLink(*ec_expr, "sin", sin);
    rrr += 1 - EVIC_FuncLink(*ec_expr, "cos", cos);
    rrr += 1 - EVIC_FuncLink(*ec_expr, "tan", tan);
    rrr += 1 - EVIC_FuncLink(*ec_expr, "asin", asin);
    rrr += 1 - EVIC_FuncLink(*ec_expr, "acos", acos);
    rrr += 1 - EVIC_FuncLink(*ec_expr, "atan", atan);
    rrr += 1 - EVIC_FuncLink(*ec_expr, "sinh", sinh);
    rrr += 1 - EVIC_FuncLink(*ec_expr, "cosh", cosh);
    rrr += 1 - EVIC_FuncLink(*ec_expr, "tanh", tanh);
    rrr += 1 - EVIC_FuncLink(*ec_expr, "exp", exp);
    rrr += 1 - EVIC_FuncLink(*ec_expr, "ln", log);
    rrr += 1 - EVIC_FuncLink(*ec_expr, "log10", log10);
    rrr += 1 - EVIC_FuncLink(*ec_expr, "sqrt", sqrt);
    rrr += 1 - EVIC_FuncLink(*ec_expr, "abs", fabs);
    if (rrr > 0)
    {
        return 0;
    }

    for (pttk = (*ec_expr)->head; pttk != NULL; pttk = pttk->next)
    {
        if (pttk->TokenClass == UNKNOW_TOKEN)
        {
            return 0;
        }
    }

    opst = NewExprStack();
    numst = NewExprStack();

    for (pttk = (*ec_expr)->head; pttk != NULL; pttk = (Token *)(pttk->next))
    {

        /* 从左至右扫描一中缀表达式 */
        /* 若读取的是操作数，
        则判断该操作数的类型，并将该操作数存入操作数堆栈 */
        switch (pttk->TokenClass)
        {
        case NUMBER_TOKEN:
            ExprStack_Push(numst, pttk);
            break;

        case OP_TKOEN:
            switch (pttk->op[0])
            {
            case '(':
                /* 该运算符为左括号"("，
                则直接存入运算符堆栈 */
                ExprStack_Push(opst, pttk);
                break;
            case ')':
                /* 该运算符为右括号")"，
                则输出运算符堆栈中的运算符到操作数堆栈，直到遇到左括号为止 */
                while (ExprStack_Pop(opst, &tpttk))
                {
                    if (tpttk->TokenClass == OP_TKOEN)
                    {
                        if (tpttk->op[0] == '(')
                        {
                            free_num++;
                            free(tpttk);
                            break;
                        }
                    }
                    ExprStack_Push(numst, tpttk);
                    free_num++;
                    free(tpttk);
                }
                if (opst->head != NULL)
                {
                    if (opst->head->TokenClass == FUNC_TOKEN)
                    {
                        ExprStack_Pop(opst, &tpttk);
                        ExprStack_Push(numst, tpttk);
                        free_num++;
                        free(tpttk);
                    }
                }
                break;
            default:
                /* 该运算符为非括号运算符 */
                if (opst->head != NULL)
                {
                    if (opst->head->TokenClass == OP_TKOEN)
                    {
                        if (opst->head->op[0] == '(')
                        {
                            /* 若运算符堆栈栈顶的运算符为括号，
                            则直接存入运算符堆栈 */
                            ExprStack_Push(opst, pttk);
                        }
                        else if (
                            OpPriority((pttk->op)[0]) >
                            OpPriority((opst->head->op)[0]))
                        {
                            /* 若比运算符堆栈栈顶的运算符优先级高，
                            则直接存入运算符堆栈 */
                            ExprStack_Push(opst, pttk);
                        }
                        else if (
                            OpPriority((pttk->op)[0]) <=
                            OpPriority((opst->head->op)[0]))
                        {
                            /* 若比运算符堆栈栈顶的运算符优先级低或者相等，
                            则依次输出栈顶运算符到操作数堆栈，
                            直到一个优先级比它低的运算符或左括号为止
                            并将当前运算符压入运算符堆栈 */
                            while (opst->head != NULL)
                            {
                                if ((OpPriority((pttk->op)[0]) <=
                                     OpPriority((opst->head->op)[0])) ||
                                    ((opst->head->op)[0] != '('))
                                {
                                    ExprStack_Pop(opst, &tpttk);
                                    ExprStack_Push(numst, tpttk);
                                    free_num++;
                                    free(tpttk);
                                }
                                else
                                {
                                    break;
                                }
                            }
                            ExprStack_Push(opst, pttk);
                        }
                        else
                        {
                            return 0;
                        }
                    }
                    else
                    {
                        return 0;
                    }
                }
                else
                {
                    /* 栈空，入栈 */
                    ExprStack_Push(opst, pttk);
                }
                break;
            }
            break;

        case VALUE_TOKEN:
            ExprStack_Push(numst, pttk);
            break;

        case FUNC_TOKEN:
            tpnextk = (Token *)pttk->next;
            if (pttk->next != NULL)
            {
                if (tpnextk->TokenClass == OP_TKOEN)
                {
                    if ((tpnextk->op)[0] == '(')
                    {
                        ExprStack_Push(opst, pttk);
                        ExprStack_Push(opst, tpnextk);
                        pttk = tpnextk;
                    }
                    else
                    {
                        return 0;
                    }
                }
                else
                {
                    return 0;
                }
            }
            else
            {
                return 0;
            }
            break;
        default:
            return 0;
            break;
        }
        if (EVIC_DEBUG_MODE)
        {
            char *info1, *info2;
            info1 = EVIC_Print(numst);
            info2 = EVIC_Print(opst);
            printf("step:\nnumst:|%sopst :|%s", info1, info2);
            free(info1);
            free(info2);
        }
    }

    /* 当表达式读取完成后运算符堆栈中尚有运算符时，
    则依序取出运算符到操作数堆栈，直到运算符堆栈为空 */
    while (ExprStack_Pop(opst, &tpttk))
    {
        ExprStack_Push(numst, tpttk);
        free_num++;
        free(tpttk);
    }
    EVIC_Free(opst);
    EVIC_Free(*ec_expr);

    *ec_expr = NewExpr();
    (*ec_expr)->head = NULL;
    (*ec_expr)->compiled = 1;

    while (ExprStack_Pop(numst, &tpttk))
    {
        ExprStack_Push(*ec_expr, tpttk);
        free_num++;
        free(tpttk);
    }
    EVIC_Free(numst);

    return 1;
}

int EVIC_Calculate(EVIC_expr *ec_expr, double *rt)
{
    ExprStack numst;
    Token *pttk, *tpttk, *aptk, *bptk, *cptk;
    double a, b, c;

    numst = NewExprStack();

    for (pttk = ec_expr->head; pttk != NULL; pttk = pttk->next)
    {
        switch (pttk->TokenClass)
        {
        case NUMBER_TOKEN:
            ExprStack_Push(numst, pttk);
            break;
        case VALUE_TOKEN:
            ExprStack_Push(numst, pttk);
            break;
        case OP_TKOEN:
            switch (pttk->op[0])
            {
            case '+':
                ExprStack_Pop(numst, &aptk);
                ExprStack_Pop(numst, &bptk);
                Token_Value(aptk, &a);
                Token_Value(bptk, &b);

                tpttk = NewToken();
                tpttk->TokenClass = NUMBER_TOKEN;
                tpttk->number = b + a;
                ExprStack_Push(numst, tpttk);
                free_num += 3;
                free(tpttk);
                free(aptk);
                free(bptk);
                break;
            case '-':
                ExprStack_Pop(numst, &aptk);
                ExprStack_Pop(numst, &bptk);
                Token_Value(aptk, &a);
                Token_Value(bptk, &b);

                tpttk = NewToken();
                tpttk->TokenClass = NUMBER_TOKEN;
                tpttk->number = b - a;
                ExprStack_Push(numst, tpttk);
                free_num += 3;
                free(tpttk);
                free(aptk);
                free(bptk);
                break;
            case '*':
                ExprStack_Pop(numst, &aptk);
                ExprStack_Pop(numst, &bptk);
                Token_Value(aptk, &a);
                Token_Value(bptk, &b);

                tpttk = NewToken();
                tpttk->TokenClass = NUMBER_TOKEN;
                tpttk->number = b * a;
                ExprStack_Push(numst, tpttk);
                free_num += 3;
                free(tpttk);
                free(aptk);
                free(bptk);
                break;
            case '/':
                ExprStack_Pop(numst, &aptk);
                ExprStack_Pop(numst, &bptk);
                Token_Value(aptk, &a);
                Token_Value(bptk, &b);

                tpttk = NewToken();
                tpttk->TokenClass = NUMBER_TOKEN;
                tpttk->number = b / a;
                ExprStack_Push(numst, tpttk);
                free_num += 3;
                free(tpttk);
                free(aptk);
                free(bptk);
                break;
            case '^':
                ExprStack_Pop(numst, &aptk);
                ExprStack_Pop(numst, &bptk);
                Token_Value(aptk, &a);
                Token_Value(bptk, &b);

                tpttk = NewToken();
                tpttk->TokenClass = NUMBER_TOKEN;
                tpttk->number = pow(b, a);
                ExprStack_Push(numst, tpttk);
                free_num += 3;
                free(tpttk);
                free(aptk);
                free(bptk);
                break;
            default:
                return 0;
                break;
            }
            break;
        case FUNC_TOKEN:
            ExprStack_Pop(numst, &aptk);
            Token_Value(aptk, &a);
            tpttk = NewToken();
            tpttk->TokenClass = NUMBER_TOKEN;
            tpttk->number = (*(pttk->pfunc))(a);
            ExprStack_Push(numst, tpttk);
            free_num += 2;
            free(tpttk);
            free(aptk);
            break;
        default:
            return 0;
            break;
        }
        if (EVIC_DEBUG_MODE)
        {
            char *info1;
            info1 = EVIC_Print(numst);
            printf("step:\nnumst:|%s", info1);
            free(info1);
        }
    }
    ExprStack_Pop(numst, &aptk);
    Token_Value(aptk, rt);
    EVIC_Free(numst);
    free_num++;
    free(aptk);
    return 1;
}

int EVIC_Eval(char *expr, double *rtBuf)
{
    malloc_num = free_num = 0;

    EVIC_expr *ec_expr;
    char *info;
    int rt;
    double rrr;

    rt = EVIC_Cut(expr, &ec_expr);
    if (rt == 0)
    {
        return 0;
        if (EVIC_DEBUG_MODE)
            printf("cut failed\n");
    }
    else
    {
        if (EVIC_DEBUG_MODE)
        {
            info = EVIC_Print(ec_expr);
            printf(info);
            free(info);
        }
    }

    rt = EVIC_Compile(&ec_expr);
    if (rt == 0)
    {
        return 0;
        if (EVIC_DEBUG_MODE)
            printf("compile failed\n");
    }
    else
    {
        if (EVIC_DEBUG_MODE)
        {
            printf("------------------------\nresult:\n");
            info = EVIC_Print(ec_expr);
            printf(info);
            free(info);
        }
    }

    rt = EVIC_Calculate(ec_expr, &rrr);
    if (rt == 0)
    {
        return 0;
        if (EVIC_DEBUG_MODE)
            printf("calculat failed\n");
    }
    else
    {
        if (EVIC_DEBUG_MODE)
            printf("------------------------\nresult: %lf\n", rrr);
    }

    EVIC_Free(ec_expr);
    *rtBuf = rrr;

    // printf("malloc:%d\nfree:%d\n", malloc_num, free_num);
    return 1;
}