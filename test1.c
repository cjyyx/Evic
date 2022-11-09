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
