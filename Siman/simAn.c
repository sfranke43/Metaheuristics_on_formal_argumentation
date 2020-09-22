#include <stdio.h>
#include <string.h>


int linRed(int startTemp, int itCnt, int rate)
{
    return startTemp - (itCnt * rate);

}

int expRed(int startTemp, int itCnt, int alpha)
{
    return startTemp * pow(alpha, itCnt);
}
