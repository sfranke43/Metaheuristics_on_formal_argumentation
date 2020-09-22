#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "transAPX.h"
#include "formArg_collection.h"
#include <string.h>
#include <time.h>
#include <signal.h>


int contains(char*, char***, int);
int findArg(char*, char*****, int);
int createAttacked(int argcnt, char***** arr, char*** ext, int** arrLen, int extLen, char*** att, int* attLen);
double isSubset(char*** sub, char*** over, int subLen, int overLen);
int defend(char*** def, char***** arr, char*** att, int* defLen, int attLen, int argcnt, int** arrLen);
int createUndefendedAttacks(int argcnt, char***** arr, char*** ext, int arrLen[argcnt][3], int extLen, char*** def, int defLen,
                                char*** att, int* attLen);
int myRandom(int lower, int upper, int tabu[], int tabuLen);
double quality(char*** ext, char***** arr, int extLen, int argcnt, int arrLen[argcnt][3], char* prob, char* sem, double punish, double* realQual);
double conflictFree(char*** ext, char***** arr, int extLen, int argcnt, int arrLen[argcnt][3], int punish, double* realQual);
double complete(char*** ext, char***** arr, int extLen, int argcnt, int arrLen[argcnt][3], int punish, double*  realQual);
double preferred(char*** ext, char***** arr, int extLen, int argcnt, int arrLen[argcnt][3], int punish, double*  realQual);
double stable(char*** ext, char***** arr, int extLen, int argcnt, int arrLen[argcnt][3], int punish, double* realQual);
double grounded(char*** ext, char***** arr, int extLen, int argcnt, int arrLen[argcnt][3], int punish, double* realQual);
int createNewSol(char*** currSol, char**** neighbors, char***** arr, int currSolLen, int* neighborsLen, int** neighborAtLen, int argcount,
                    int arrLen[argcount][3], double* realQualOfBest, int min_random, int max_random, double threshold_random, int maxDef, int maxDel, int maxPossAtt,
                        char* problem, char* semantic, double punish, int* tabu, int tabuLen, int** actArr);
int execTS(char***** arr, int argcnt, int arrLen[argcnt][3], char*** initialSol, int initialSolLen, int* tabu, int tabuLen, int tabuSize,
                int maxIteration, char* problemStr, char* semanticStr, int min_random, int max_random, double threshold_random, int maxDef, int maxDel, int maxPossAtt);


int main(int argc, char* argv[])
{


    /**Read in the command of the user*/
    if(strcmp(argv[1], "--formats") == 0)
    {
        printf(".apx");
        return 0;
    }
    if(strcmp(argv[1], "--problems") == 0)
    {
        printf("[ein paar Probleme, aber noch lange nicht alle, lol]");
        return 0;
    }

    char* probAndSem;
    char* fileLoc;
    char* checkArgs;
    int min_random;
    int max_random;
    int maxDef;
    int maxDel;
    int maxPossAtt;
    int itCnt;
    int tabuSize;

    for(int i = 0; i < argc; i++)
    {
        if(strcmp(argv[i], "-p") == 0)
            probAndSem = argv[i+1];
        if(strcmp(argv[i], "-f") == 0)
            fileLoc = argv[i+1];
        if(strcmp(argv[i], "-a") == 0)
            checkArgs = argv[i+1];
        if(strcmp(argv[i], "-n") == 0)
        {
            min_random  = atoi(argv[i+1]);
            max_random  = atoi(argv[i+2]);
            maxDef      = atoi(argv[i+3]);
            maxDel      = atoi(argv[i+4]);
            maxPossAtt  = atoi(argv[i+5]);
            itCnt       = atoi(argv[i+6]);
            tabuSize    = atoi(argv[i+7]);
        }


    }
    char* prob = malloc(2);
    memcpy( prob, &probAndSem[0], 2 );
    char* sem = malloc(4);
    memcpy( sem, &probAndSem[3], strlen(probAndSem) - 2 );
    if(strcmp(sem, "ADM") == 0)
        sem = "admissable";
    if(strcmp(sem, "CO") == 0)
        sem = "complete";
    if(strcmp(sem, "ST") == 0)
        sem = "stable";
    if(strcmp(sem, "PRF") == 0)
        sem = "preferred";


    printf("%s", checkArgs);
    FILE *fp;
    fp = fopen(fileLoc, "r");

    char* **argarrayPoint;//argments is an string array
    char* ***attarrayPoint;//attackers is an array of string arrays
    int argcnt = 0;
    int attcnt = 0;
    int* strcnt = &argcnt;
    int* strcnt1 = &attcnt;

    char** argarray = NULL;
    char*** attarray = NULL;
    argarrayPoint = &argarray;
    attarrayPoint = &attarray;

    translateAPX(fp, argarrayPoint, attarrayPoint, strcnt, strcnt1);//translate the input file to the  array structeres

    fp = fclose(fp);


/* ******create new array to store each argument alphabetically,
*        each argument that is attacked by arg x is stored in arr[posOfX][1]
*        each argument that attacks arg x is stored in arr[posOfX][2][..]
*        both subarrays are sorted
*/
    char**** arr = NULL;
    arr = (char****)realloc(arr, (argcnt) * sizeof(char ***));//get the size of all arguments for column 0

    for(int i = 0; i < argcnt; i++)//initialize 2 String arrays for attackers (2) and attacked (1)
    {
        arr[i] = (char***)malloc((3) * sizeof(char **));
        arr[i][0] = argarray[i];//initialize the arguments in column 0
        arr[i][1] = NULL;//initialize column 1
        arr[i][2] = NULL;//initialize column 2
    }

    int len[argcnt][3];//store the length of the subarrays
    //initialize length of argument as 1, and the other two as 0
    for(int i = 0; i <argcnt; i++)
    {
        len[i][0] = 1;
        len[i][1] = 0;
        len[i][2] = 0;
    }

for(int i = 0; i < attcnt; i++)
    printf("%s,  %s     ", attarray[i][0], attarray[i][1]);

    //enter the data in the subarrays
    for(int i = 0; i < attcnt; i++)
    {
        //find the attacker and the attacked in column 0
        int attacking = findArg(attarray[i][0], &arr, argcnt);
        int attacked = findArg(attarray[i][1], &arr, argcnt);
        printf("%d  %d   ", attacking, attacked);
        //allocate space for one attack
        (arr[attacking][1]) = (char**)realloc(arr[attacking][1], (++len[attacking][1]) * sizeof(char*));
        (arr[attacked][2]) = (char**)realloc(arr[attacked][2], (++len[attacked][2]) * sizeof(char*));
        //enter the data
        arr[attacking][1][len[attacking][1]-1] = attarray[i][1];
        arr[attacked][2][len[attacked][2]-1] = attarray[i][0];

    }

    //sort the subarrays
    for(int i = 0; i < argcnt; i++){
        quickSort(arr[i][1], 0, len[i][1]-1);
        quickSort(arr[i][2], 0, len[i][2]-1);
    }
    free(argarray);
    free(attarray);

    //initialize random seed for neighborhood
    srand(time(NULL));


    char** initialSol;
    int* tabu;
    int initialSolLen;
    int tabuLen;


    if(strcmp(prob, "SE") == 0)
    {
        initialSol = NULL;
        initialSolLen = 0;
        tabu = malloc(sizeof(int));
        tabuLen = 0;
    }

    if(strcmp(prob, "DC") == 0 || strcmp(prob, "DS") == 0)
    {
        initialSol = malloc(sizeof(char*));
        initialSol[0] = checkArgs;
        initialSolLen = 1;
        tabu = malloc(sizeof(int));
        tabuLen = 0;
    }




    int result = execTS(&arr, argcnt, len, &initialSol, initialSolLen, tabu, tabuLen, tabuSize, itCnt,"blub", sem, min_random, max_random, 0.5, maxDef, maxDel, maxPossAtt);

    if(result == 1 && strcmp(prob, "DS") == 0)
    {
        initialSol = NULL;
        initialSolLen = 0;
        tabu = malloc(sizeof(int));
        tabuLen = 1;
        tabu[0] = findArg(checkArgs, &arr, argcnt) + 1;
        result = execTS(&arr, argcnt, len, &initialSol, initialSolLen, tabu, tabuLen, tabuSize, itCnt, "blub", sem, min_random, max_random, 0.5, maxDef, maxDel, maxPossAtt);
    }
    if(result == 1)
        printf("\nYES");
    else
        printf("\nNO");


    return 0;
}
///home/sebastian/Desktop/A-1-admbuster_1000.apx"
