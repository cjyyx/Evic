#include "Evic.c"

int main(int argc, char const *argv[])
{
    for (long i=0;i<5000000;i++)
    {
        double rt;        
        EVIC_Eval("(ln(e)*ln(e^2))^(20*sin(pi/(3*ln(e^2))))", &rt);
    }
}
