#include "Evic.c"

int main(int argc, char const *argv[])
{
    double rt;
    if (EVIC_Eval("(ln(e)*ln(e^2))^(20*sin(pi/(3*ln(e^2))))", &rt))
    {
        printf("result: %lf\n", rt);
    }
    else
    {
        printf("eval failed\n");
    }
}
