#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include "formArg_collection.h"

double quality(char*** ext, char***** arr, int extLen, int argcnt, int arrLen[argcnt][3], char* prob, char* sem, double punish, double* realQual);
int createNewSol(char*** currSol, char**** neighbors, char***** arr, int currSolLen, int* neighborsLen, int** neighborAtLen, int argcount,
                    int arrLen[argcount][3], double* realQualOfBest, int min_random, int max_random, double threshold_random, int maxDef, int maxDel, int maxPossAtt,
                        char* problem, char* semantic, double punish, int* tabu, int tabuLen, int** actArr);

double linRed(double startTemp, int itCnt, double rate)
{
    return startTemp - (itCnt * rate);
}

double expRed(double startTemp, int itCnt, double alpha)
{
    return startTemp* pow(alpha, itCnt);
}



int execSimAn(char***** arr, int argcnt, int arrLen[argcnt][3], char*** initialSol, int initialSolLen, int* tabu, int tabuLen,
                int startTemp, int coolingChoice, double decrease, char* problemStr, char* semanticStr, int min_random, int max_random, double threshold_random, int maxDef, int maxDel, int maxPossAtt)
{
   clock_t start, end;
    double cpu_time_used;
    start = clock();


    FILE *filePtr;




    double (*cool[])(double, int, double) = {linRed, expRed};
    int breakCond = 0; // if this number exceeds 10000 there was no improvement in the last 10000 iterattions. We can end the algorithm then
    int itCnt = 0; //number of iterations
    double breaky = (*cool[coolingChoice])(startTemp, itCnt, 0.1);
    double temp = (*cool[coolingChoice])(startTemp, itCnt, 0.1);



    tabu = realloc(tabu, (initialSolLen + tabuLen) * sizeof(int));

    for(int i = tabuLen; i < tabuLen + initialSolLen; i++)
    {
        tabu[i] = -1 * (findArg((*initialSol)[i - tabuLen], arr, argcnt) + 1);
    }
    tabuLen += initialSolLen;

    int* tabuSave = malloc(tabuLen * sizeof(int));
    memcpy(tabuSave, tabu, tabuLen * sizeof(int));
    int tabuSaveLen = tabuLen;

    char** currSol = malloc(initialSolLen * sizeof(char*));
    char*** currPoint = &currSol;
    int currSolLen = initialSolLen;
    char** bestSol = malloc(initialSolLen * sizeof(char*));
    int bestSolLen = initialSolLen;
    char** newSol = NULL;
    int newSolLen = 0;

        char*** neighbors = NULL;
    int neighborLen = 0;
    int* neighborAtLen = NULL;


    memcpy(currSol, *initialSol, initialSolLen * sizeof(char*));
    bestSol = currSol;




    double* newQual = malloc(sizeof(double));

    double currQual = quality(initialSol, arr, initialSolLen, argcnt, arrLen, problemStr, semanticStr, 1, newQual);
    double bestQual = currQual;


    int mycntr = 0;


    while(temp > 0.000000001)
    {

        printf("\n\n0");

        int* actArr = NULL;
        int act = createNewSol(&currSol, &neighbors, arr, currSolLen, &neighborLen, &neighborAtLen, argcnt, arrLen, newQual,
                                min_random, max_random,threshold_random, maxDef, maxDel, maxPossAtt,
                                 problemStr, semanticStr, 10, tabu, tabuLen, &actArr);


        printf("\nNEWQUAL: %lf", *newQual);

        for(int i = 0; i< neighborLen; i++){
            printf(" | ");
            for(int j = 0; j< neighborAtLen[i]; j++)
                printf("%s  ", (neighbors)[i][j]);}


        tabuLen = tabuSaveLen;
        tabu = malloc(tabuLen * sizeof(int));
        memcpy(tabu, tabuSave, tabuLen * sizeof(int));

        int rand3 = rand() % neighborLen;
        newSol = malloc(neighborAtLen[rand3] * sizeof(char*));

        newSolLen = neighborAtLen[rand3];
        printf("LAENGE: %d", neighborAtLen[rand3]);
        memcpy(newSol, (neighbors)[rand3], newSolLen * sizeof(char*));

        free(neighbors);
        free(neighborAtLen);
        *newQual = quality(&newSol, arr, newSolLen, argcnt, arrLen, problemStr, semanticStr, 1, newQual);

        double rand2 = (double)rand() / RAND_MAX;

        double multiplier = 10000;
        printf("\ncurr: %lf   new: %lf  ", currQual, *newQual);
        double checker = exp(((multiplier* ((*newQual))) - (multiplier * ((currQual)))) / temp);
        printf("rand2 = %lf  exp = %lf\n", rand2, checker);

        for(int i = 0; i < newSolLen; i++)
            printf("new[%d]= %s ", i, newSol[i]);
        if(*newQual - currQual >= 0.0 || checker -rand2 > 0.0)
        {

            printf("\nBetter");

            currSol = malloc(newSolLen * sizeof(char*));
            memcpy(currSol, newSol, newSolLen * sizeof(char*));

            currQual = *newQual;

            currSolLen = newSolLen;

            mycntr++;


        }

        else
        {
            printf("\n       Worse");
            free(newSol);
        }
        if(currQual > bestQual)
        {

            bestQual = currQual;
            bestSol = realloc(bestSol, currSolLen * sizeof(char*));
            bestSolLen = currSolLen;

            memcpy(bestSol, currSol, currSolLen * sizeof(char*));
            if(bestQual >= 1.0)
                break;



        }

        breakCond++;



          double bQ = quality(&bestSol, arr, bestSolLen, argcnt, arrLen, problemStr, semanticStr, 1, newQual);
          double cQ = quality(&currSol, arr, currSolLen, argcnt, arrLen, problemStr, semanticStr, 1, newQual);
        printf("\nrealQualityOfBest = %lf", bQ);
        printf("\nrealQualityOfCurr = %lf", cQ);
        printf("\ncnt = %d   ",itCnt);
        printf("\curQual = %lf ", currQual);
        printf("\nnewQual = %lf  ",*newQual);
        printf("\nbestQual = %lf  ",bestQual);
        printf("\ncurrLen = %d  ",currSolLen);
        printf("\nbestLen = %d  ",bestSolLen);

        printf("\ntemp = %lf",temp);


        itCnt++;
        temp = (*cool[coolingChoice])(startTemp, itCnt, decrease);
    }
    end = clock();
    cpu_time_used = ((double) (end - start));
    printf("\n");
    for(int i = 0; i<bestSolLen; i++)
        printf("\nbest[%d] = %s", i, bestSol[i]);

    printf("\nqualityOfBest = %lf   %lf", quality(&bestSol, arr, bestSolLen, argcnt, arrLen, problemStr, semanticStr, 1, newQual), *newQual);

    char** att = NULL;
    char** def = NULL;
    int attLen = 0;
    int defLen = 0;

    createAttacked(argcnt, arr, &bestSol, arrLen, bestSolLen, &att, &attLen);
    defend(&def, arr, &att, &defLen, attLen, argcnt, arrLen);

    for(int i = 0; i < defLen; i++)
        printf("\n def[%d] = %s", i, def[i]);

    printf("\n\ntime needed = %lf;  time per iteration = %lf", cpu_time_used/ CLOCKS_PER_SEC, (cpu_time_used / itCnt)/ CLOCKS_PER_SEC);

    if(bestQual >= 1)
        return 1;
    else
        return 0;
}
