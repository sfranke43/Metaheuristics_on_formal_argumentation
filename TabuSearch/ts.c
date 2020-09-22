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

int updateTabu(int** tabu, int* tabuLen, int tabuMaxSize, int tabuMinSize, int act)
{

    if(*tabuLen < tabuMaxSize)
    {
        (*tabu) = realloc((*tabu), ((*tabuLen) + 1) * sizeof(int));
        (*tabu)[*tabuLen] = -1 * act;
        (*tabuLen)++;
        return 0;
    }
    else if((*tabuLen) >= tabuMaxSize)
    {
        for(int i = tabuMinSize; i < tabuMaxSize - 1; i++)
        {
            (*tabu)[i] = (*tabu)[i+1];
        }
        (*tabu)[tabuMaxSize - 1] = -1 * act;
        return 0;
    }
    else
        return -1;


    return 0;
}


int execTS(char***** arr, int argcnt, int arrLen[argcnt][3], char*** initialSol, int initialSolLen, int* tabu, int tabuLen, int tabuSize, int maxIteration,
            char* problemStr, char* semanticStr, int min_random, int max_random, double threshold_random, int maxDef, int maxDel, int maxPossAtt)
{
    clock_t start, end;
    double cpu_time_used;
    start = clock();


   // FILE *filePtr;

    //filePtr = fopen("/media/sebastian/Data/Uni/Semester6/BA/qualitaetswerte","w");


    int breakCond = 0; // if this number exceeds 10000 there was no improvement in the last 10000 iterattions. We can end the algorithm then
    int itCnt = 0; //number of iterations




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


    while(maxIteration - itCnt > 0)
    {




        tabuSaveLen = tabuLen;
        tabuSave = malloc(tabuSaveLen * sizeof(int));
        memcpy(tabuSave, tabu, tabuSaveLen * sizeof(int));

        int* actArr = NULL;
        printf("before entering ");
        for(int i = 0; i < currSolLen; i++)
            printf("%s ", currSol[i]);
        int act = createNewSol(&currSol, &neighbors, arr, currSolLen, &neighborLen, &neighborAtLen, argcnt, arrLen, newQual,
                                min_random, max_random,threshold_random, maxDef, maxDel, maxPossAtt,
                                    problemStr, semanticStr, 1, tabuSave, tabuSaveLen, &actArr);



            //choose best neighbor and save him in
            double newRealQual = 0;
            *newQual = 0;
        for(int i = 0; i < neighborLen; i++)
        {
            double newNeighborQual = quality(&(neighbors[i]), arr, neighborAtLen[i], argcnt, arrLen, problemStr, semanticStr, 1, &newRealQual);
            if(containsInt(tabu, actArr[i], tabuLen) == -1 && newNeighborQual - *newQual >= 0)
            {


                newSol = malloc(neighborAtLen[i] * sizeof(char*));
                newSolLen = neighborAtLen[i];
                memcpy(newSol, neighbors[i], neighborAtLen[i] * sizeof(char*));
                *newQual = newNeighborQual;
                act = actArr[i];
            }
        }
        free(neighbors);

        printf("   aACT: %d", act);
        int op = updateTabu(&tabu, &tabuLen, tabuSize, initialSolLen, act);
        printf("\n");



        printf("\ncurr: %lf   new: %lf  ", currQual, *newQual);



            currSol = malloc(newSolLen * sizeof(char*));
            memcpy(currSol, newSol, newSolLen * sizeof(char*));
            currQual = *newQual;
            currSolLen = newSolLen;
            mycntr++;



        if(currQual > bestQual)
        {

            bestQual = currQual;
            bestSol = realloc(bestSol, currSolLen * sizeof(char*));
            bestSolLen = currSolLen;
            memcpy(bestSol, currSol, currSolLen * sizeof(char*));
        }
        if(bestQual >= 1)
            break;



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


        itCnt++;

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
