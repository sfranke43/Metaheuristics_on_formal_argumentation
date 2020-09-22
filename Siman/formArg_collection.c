#include <stdlib.h>

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

double isSubset(char*** sub, char*** over, int subLen, int overLen);
double conflictFree(char*** ext, char***** arr, int extLen, int argcnt, int arrLen[argcnt][3], int punish, double*  realQual);
double admissable(char*** ext, char***** arr, int extLen, int argcnt, int arrLen[argcnt][3], int punish, double*  realQual);
double complete(char*** ext, char***** arr, int extLen, int argcnt, int arrLen[argcnt][3], int punish, double*  realQual);
double preferred(char*** ext, char***** arr, int extLen, int argcnt, int arrLen[argcnt][3], int punish, double* realQual);
double grounded(char*** ext, char***** arr, int extLen, int argcnt, int arrLen[argcnt][3], int punish, double* realQual);
double stable(char*** ext, char***** arr, int extLen, int argcnt, int arrLen[argcnt][3], int punish, double*  realQual);
int createAttacked(int argcnt, char***** arr, char*** ext, int arrLen[argcnt][3], int extLen, char*** att, int* attLen);



double quality(char*** ext, char***** arr, int extLen, int argcnt, int arrLen[argcnt][3], char* prob, char* sem, double punish, double* realQual)
{


    double (*semQual[])(char***, char*****, int, int, int[argcnt][6], int, double*) =
                        {conflictFree, admissable, complete, preferred, grounded, stable};
    int choice;
    if(strcmp(sem, "conflict free") == 0)
        choice = 0;
    else if(strcmp(sem, "admissable") == 0)
        choice = 1;
    else if(strcmp(sem, "complete") == 0)
        choice = 2;
    else if(strcmp(sem, "preferred") == 0)
        choice = 3;
    else if(strcmp(sem, "grounded") == 0)
        choice = 4;
    else if(strcmp(sem, "stable") == 0)
        choice = 5;
    else
    {
        printf("\nSemantic needs to be one of the supported semantics");
        return -1;
    }

    //double qual = admissable(ext, arr, extLen, argcnt, arrLen, punish, realQual);
    double qual = (*semQual[choice])(ext, arr, extLen, argcnt, arrLen, punish, realQual);
    return qual;


}

/**
*   returns how much of the of the extension is attacked by itself => if the return > 0 then the extension is not conflicfree
*/
double conflictFree(char*** ext, char***** arr, int extLen, int argcnt, int arrLen[argcnt][3], int punish, double* realQual)
{
    if(extLen == 0)
        return 0;
    char** attSol = NULL;
    int attSolLen = 0;

    createAttacked(argcnt, arr, ext, arrLen, extLen, &attSol, &attSolLen);
    double sub = isSubset(&attSol, ext, attSolLen, extLen);
    *realQual = sub;
    return 1 - sub;
}

/**
*   returns how much of the of the extension is defended by itself (extension has to be conflictfree)
*/
double admissable(char*** ext, char***** arr, int extLen, int argcnt, int arrLen[argcnt][3], int punish, double* realQual)
{

    if(extLen == 0)
        return 0;
    //if the extension is not admissable the quality is zero
    if(conflictFree(ext, arr, extLen, argcnt, arrLen, punish, realQual) < 1.0)
        return 0.0;
    char** attSol = NULL;
    int attSolLen = 0;

    createAttacked(argcnt, arr, ext, arrLen, extLen, &attSol, &attSolLen);
    double sub = isSubset(&attSol, ext, attSolLen, extLen);

    char** def = NULL;
    int defLen = 0;

    defend(&def, arr, &attSol, &defLen, attSolLen, argcnt, arrLen);
    //quality is detrmined by checking if defend and extesnio are the  same. Therefore check if both are subsets of one another
    double res = conflictFree(ext, arr, extLen, argcnt, arrLen, punish, realQual) * isSubset(ext, &def, extLen, defLen) ;
    free(def);
    free(attSol);
    *realQual = res;
    return res;
}

/**
*   returns how much of the of the extension is defended by itself and how much outside of the extension is defended (extension has to be conflictfree)
*/
double complete(char*** ext, char***** arr, int extLen, int argcnt, int arrLen[argcnt][3], int punish, double* realQual)
{

    if(extLen == 0)
        return 0;

    char** attSol = NULL;
    int attSolLen = 0;

    createAttacked(argcnt, arr, ext, arrLen, extLen, &attSol, &attSolLen);
    double sub = isSubset(&attSol, ext, attSolLen, extLen);

    char** def = NULL;
    int defLen = 0;

    defend(&def, arr, &attSol, &defLen, attSolLen, argcnt, arrLen);

    //quality is detrmined by checking if defend and extesnio are the  same. Therefore check if both are subsets of one another
    double res = conflictFree(ext, arr, extLen, argcnt, arrLen, punish, realQual) * ((isSubset(ext, &def, extLen, defLen) + isSubset(&def, ext, defLen, extLen)) / 2.0);
    free(def);
    free(attSol);
    *realQual = res;
    return res;
}

/**
*   returns completed but tries to values bigger extensions more depending on punish [more punish => size matters less]
*   (extension has to be conflictfree)
*/
double preferred(char*** ext, char***** arr, int extLen, int argcnt, int arrLen[argcnt][3], int punish, double* realQual)
{

    *realQual = complete(ext, arr, extLen, argcnt, arrLen, punish, realQual);
    double result = conflictFree(ext, arr, extLen, argcnt, arrLen, punish, realQual)*(((extLen / argcnt) + *realQual * punish) / (punish + 2));
    *realQual = conflictFree(ext, arr, extLen, argcnt, arrLen, punish, realQual)*(((extLen / argcnt) + (*realQual)) / 2);
    return result;

}


/**
*   returns completed but tries to values smaller extensions more depending on punish [more punish => size matters less]
*   (extension has to be conflictfree)
*/
double grounded(char*** ext, char***** arr, int extLen, int argcnt, int arrLen[argcnt][3], int punish, double* realQual)
{

    *realQual = complete(ext, arr, extLen, argcnt, arrLen, punish, realQual);
    double result = (1-(extLen / argcnt) + *realQual * punish) / (punish + 2);
    *realQual = (1-(extLen / argcnt) + (*realQual)) / 2;
    return result;

}

double stable(char*** ext, char***** arr, int extLen, int argcnt, int arrLen[argcnt][3], int punish, double* realQual)
{



    *realQual = complete(ext, arr, extLen, argcnt, arrLen, punish, realQual);
    char** attSol = NULL;
    int* attSolLen = malloc(sizeof(int));
    createAttacked(argcnt, arr, ext, arrLen, extLen, &attSol, attSolLen);

    char** toBeAttacked = NULL;
    int toBeAttackedLen = 0;
    int extCurr = 0;
    int arrCurr = 0;

    double isAttacked = (double)(*attSolLen) / (argcnt - extLen);

    double result =(isAttacked + *realQual * punish) / (punish + 2);
    return conflictFree(ext, arr, extLen, argcnt, arrLen, punish, realQual) * result;

return 0;
}

/**
* returns the index of the string str in the 0th column of array arr with the length len
*/
int findArg(char* str, char***** arr, int len)
{
    int first = 0;
    int last = len-1;
    int mid = (first+last)/2;

    while (first <= last)
    {
        if (strcmp((*arr)[mid][0], str) < 0)
            first = mid + 1;
        else if (strcmp((*arr)[mid][0], str) == 0)
            return mid;
        else
            last = mid - 1;

        mid = (first + last)/2;
    }


    return -1;
}
/**
* creates a binary search on a one dimensional sorted string array and returns the position if there is one
*/
int contains(char* str, char*** arr, int len)
{


    for(int i = 0; i< len; i++)
    {
        if(strcmp((*arr)[i], str) == 0)
            return i;
    }
    return -1;
}



/**
*   checks, whether the sorted and disjunct string array sub is a subset of the  sorted and disjunct string array over
*   returns the percentage of arguments in sub that are also in over
*   => if it returns 1.0 sub is a subset of over (important for complete semantics)
*   => if it returns 0.0 sub and over do not share a single element (important for conflict free)
*/
double isSubset(char*** sub, char*** over, int subLen, int overLen)
{
    if(subLen == 0 && overLen == 0)//if there is no arguments in sub and in over, thenwe compare two empty lists, so we return basically true
        return 1;
    else if(subLen == 0)//if there is no element in sub, then we do not have a subset relation
        return 0;
    int subCurr = 0;//the current position in sub
    int overCurr = 0;//the current position in over
    double found = 0;//counts how many elements of subset are in over
    while(subCurr < subLen && overCurr < overLen)
    {
        if(strcmp((*sub)[subCurr], (*over)[overCurr]) == 0)//if a match is found move on element ahead in both arrays and increase the matches
        {

            found++;
            subCurr++;
            overCurr++;
        }
        else if(strcmp((*sub)[subCurr], (*over)[overCurr]) < 0)//if sub[x] < over[y] increase x by 1 to try
        {
            subCurr++;
        }
        else//likewise increase y by 1
        {
            overCurr++;
        }
    }
    return (found / subLen);//return the percantage of matches
}

/**
*   creates a sorted and disjunct list att with the length attLen of all elements that are attacked by the extension ext with arr as a basis
*/
int createAttacked(int argcnt, char***** arr, char*** ext, int arrLen[argcnt][3], int extLen, char*** att, int* attLen)
{

    *attLen = 0;
    //iterate over all elements of the extension
    for(int i = 0; i < extLen; i++)
    {

        int curr = findArg((*ext)[i], arr, argcnt);
        (*att) = (char**)realloc(*att, (*attLen + ((arrLen)[curr])[1])* sizeof(char*));//allocate space for all attacks of one extension member
        for(int j = 0; j < arrLen[curr][1]; j++)//enter the attacks
        {
            (*att)[(*attLen)+j] = (*arr)[curr][1][j];

        }
        *attLen += ((arrLen)[curr])[1];
    }

    quickSort(*att, 0, *attLen-1);//sort the array
    //***********************************create new list where redundant elements are deleted***********************************
    char** attNew = NULL;//initialize new empty string array
    int attLenNew = 0;
    if(*attLen > 0)//give it a length of 1 if needed
    {
        attNew = malloc(sizeof(char*));
        attNew[0] = (*att)[0];
        attLenNew = 1;
    }

    for(int i = 1; i < *attLen; i++)//delete redundant elements
    {
        if(strcmp((*att)[i], (*att)[i-1]) != 0)//only copy not redundant elements in new list
        {
            attNew = realloc(attNew, (attLenNew+1)*sizeof(char*));
            attNew[attLenNew++] = (*att)[i];
        }
    }

    *att = attNew;//assign new values
    *attLen = attLenNew;

    return 0;
}

/**
* creates a list of all undefended attacks onthe list ext;
* the list of defended elements has to be provided
*/
int createUndefendedAttacks(int argcnt, char***** arr, char*** ext, int arrLen[argcnt][3], int extLen, char*** def, int defLen,
                                    char*** att, int* attLen)
{
    *att = malloc(sizeof(char*));

    *attLen = 0;
    //iterate over all elements of the extension
    for(int i = 0; i < extLen; i++)
    {

        int curr = findArg((*ext)[i], arr, argcnt);
        (*att) = (char**)realloc(*att, (*attLen + ((arrLen)[curr])[2])* sizeof(char*));//allocate space for all attacks of one extension member
        for(int j = 0; j < arrLen[curr][2]; j++)//enter the attacks
        {

            (*att)[(*attLen)+j] = (*arr)[curr][2][j];

        }


        (*attLen) += ((arrLen)[curr])[2];
    }

    if((*attLen) == 0)
        return 0;
    quickSort(*att, 0, (*attLen)-1);//sort the array
    //***********************************create new list where redundant elements are deleted***********************************
    deleteRedundant(att, attLen);



    char** undefAtt = malloc(sizeof(char*));
    int undefAttLen = 0;

    for(int i = 0; i < *attLen; i++)
    {
        if(contains((*att)[i], def, defLen) == -1)
        {
            (undefAttLen)++;
            undefAtt = realloc(undefAtt, undefAttLen * sizeof(char*));
            (undefAtt)[(undefAttLen) - 1] = (*att)[i];
        }
    }
    free(*att);
    *att = undefAtt;
    *attLen = undefAttLen;

    return 0;
}

/**
*   creates a list of all elements in arr that are defended by a set of argmuments that attack the list of arguments in arr
*   NOTE: att is ! NOT ! the list in the extension, it has to be evaluated from the extension via createAttacked(..)
*/
int defend(char*** def, char***** arr, char*** att, int* defLen, int attLen, int argcnt, int arrLen[argcnt][3])
{

    for(int i = 0; i < argcnt; i++)//iterate over all argguments in arr
    {
        char** attCurr = (*arr)[i][2];//get all elements that attack the current argument

        double check = isSubset(&attCurr, att, arrLen[i][2], attLen);
        if(arrLen[i][2] == 0 || check- 1.0f < 0.00000001f && check - 1.0f >= 0.0f)//if check == 0 or if there are no attackers on an argument
        {

            *def = (char**)realloc(*def, (*defLen + 1)* sizeof(char*));//allocate space for one more char
            (*def)[*defLen] = (*arr)[i][0];
            (*defLen)++;

        }

    }


    return 0;
}

double evalQualOfSol(char*** sol, char* problemStr, char* semanticStr)
{
    return 0;
}
/**
* implements a linear search over the array arr
*/
int containsInt(int* arr, int search, int len)
{
    for(int i = 0; i < len; i++)
        if(arr[i] == search)
            return i;

    return -1;
}

int createAttackersOnUndefendedAttacks(char*** undefendedAttackers, int undefendedAttackersLen, char***** arr, int argcnt, int arrLen[argcnt][3],
                                            char*** result, int* resultLen)
{
    *result = malloc(sizeof(char**));

    for(int i = 0; i < argcnt; i++)
    {
        for(int j = 0; j < undefendedAttackersLen; j++)
        {
            if(contains((*undefendedAttackers)[j], &((*arr)[i][1]), arrLen[i][1]) != -1)
            {

                *result = realloc((*result), ((*resultLen) + arrLen[i][1]) * sizeof(char*));
                for(int k = 0; k < arrLen[i][1]; k++)
                {
                    (*result)[*resultLen + k] = (*arr)[i][1][k];
                }
                *resultLen += arrLen[i][1];
            }
        }

                //add the element
    }
    return 0;
}

/**
* deletes all redundant elements in a string list
*/
int deleteRedundant(char*** ext, int* extLen)
{
    if((*extLen) == 1 || (*extLen) == 0)
        return 0;
    else
    {

        for(int i = 0; i < *extLen; i++)
        {
            for(int j = i+1; j < *extLen; j++)
            {
                if(strcmp((*ext)[i], (*ext)[j]) == 0)
                {
                    for(int k=j; k < *extLen; k++)
                    {
                        (*ext)[k] = (*ext)[k + 1];
                        *ext = realloc(*ext, (*extLen - 1) * sizeof(char*));
                        (*extLen)--;
                        if(j != 0)
                            j--;
                        else
                            j = 0;
                    }

                }
            }
        }
        return 0;
    }
}
/**
* returns a random int bewteen and including lower and upper and excluding all members of tabu
*/
int myRandom(int lower, int upper, int* tabu, int tabuLen)
{

    //no tabus therefore just return a rando number in range
    if(tabuLen == 0)
    {
        return  (rand() % (upper - lower + 1)) + lower;
    }
    //if i is not in tabu add it to the list of possible candidates
    else
    {
        int* sol = malloc(sizeof(int));
        int solLen = 0;

        int cntOfHits = 0;
        for(int i = lower; i <= upper; i++)//go through range
        {
            if(containsInt(tabu, i, tabuLen) == -1)//add number to candidate list if it is not in tabu
            {
                sol = realloc(sol, (solLen+1)*sizeof(int));
                sol[solLen] = i;
                solLen++;
                cntOfHits++;


            }

        }
        //if all numbers bewteen low and high are tabu, the solLen is 0. therefore return -1
        if(solLen == 0)
            return -1;
        int rand2 = (rand() % (solLen));//choose a random possible candidate
        return  sol[rand2];
    }
    return -1;//something went wrong if this is triggered
}
/**
* creates one new solution with a basis of currSol and performing it on the arhument with posiition rand1
*/
int oneNewSol(int rand1, int*curAct, char*** currSol, char*** newSol, char***** arr, int currSolLen, int* newSolLen, int argcount,
                    int arrLen[argcount][3], double* realQualOfBest, char* semantic, double punish, int* tabu, int tabuLen)
{

        char* randArg;
        randArg = (char*)(((*arr)[rand1])[0]);
        /**check if the argument is already in the current solution*/
        int pos = contains(randArg, currSol, currSolLen);

        //if it is not in the current solution, add it to the current solution
        if(pos == -1 && containsInt(tabu, (rand1) + 1, tabuLen) == -1)
        {

            (*newSol) = (char**)realloc((*newSol), (currSolLen +1) * sizeof(char*));
            memcpy(*newSol, *currSol, currSolLen  * sizeof(char*));
            (*newSol)[currSolLen] = randArg;
            (*newSolLen) = currSolLen + 1;
            *curAct = rand1 + 1;

        }
        //if it is in the current solution, delete it from the current solution
        else if(pos != -1 && containsInt(tabu, (-1 * rand1) - 1, tabuLen) == -1)
        {
            //if the last element from the list is deleted
            if(currSolLen == 1)
            {
                //printf("\nEntferneMitLen1");
                *newSolLen = 0;
                *curAct = -1 * (rand1 + 1);
                //allocate with size of 1, otherwise segFault
                *newSol = realloc(*newSol,  sizeof(char*));
            }
            else
            {

                (*newSol) = (char**)realloc((*newSol), (currSolLen - 1) * sizeof(char*));
                //copy every element of curr except for pos
                int j = 0;
                for(int i = 0; i < currSolLen; i++)
                {
                    if(i != pos)
                    {
                    (*newSol)[j] = (*currSol)[i];
                    j++;
                }
            }

            (*newSolLen) = currSolLen - 1;
            *curAct = -1 * (rand1 + 1);
            }
        }
        //pos is not in range of the length
        else{

            return -1;}

        return 0;
}

/**
* choose the best of min 10 and max 20 possible solutions as a next step
* returns the best action found
* action: x -> add argument at position x-1; negative -> delete argument with position x-1 in arr from extension
* (the +1 is there to avoid condusion between adding and eleting argument at position 0)
* returns 0 if all arguments were tried and all solutions were not conflictfree
*/
int createNewSol(char*** currSol, char**** neighbors, char***** arr, int currSolLen, int* neighborsLen, int** neighborAtLen, int argcount,
                    int arrLen[argcount][3], double* realQualOfBest, int min, int max, double threshold, int maxDef, int maxDel, int maxPossAtt,
                    char* problem, char* semantic, double punish, int* tabu, int tabuLen, int** actArr)
{
    printf(" \n entered creatNewSol  ");
    //zu Testzwecken
    int rand_cnt = 0;
    int del_cnt = 0;
    int def_cnt = 0;
    int att_cnt = 0;
    //Ende test

    char*** result  = malloc(sizeof(char**));
    int resultLen = 0;
    int* resultAtLen = malloc(sizeof(int));
    //an action is adding or deleting an argument (represented by ist position in arr): positive nr => adding; negative nr => deleting
    int curAct = 0;
    int bestAct = argcount+1; //initialize as argcount to be able to see if no solution was found

    double curQuality = 0.0;
    double bestQuality = 0.0;

    char*** bestSol = NULL;
    int bestSolLen = 0;
    char*** newSol = malloc(sizeof(char**));
    *newSol = NULL;
    int* newSolLen = malloc(sizeof(int));

    int cnt = 0;
    int thresholdCnt = 0;
    int thresholdSwitch = 0;
    int numberOfConlifctedSols = 0;

    //forbid to add members of the current solution
    tabu = realloc(tabu, (tabuLen + currSolLen)* sizeof(int));
    for(int i = tabuLen; i < tabuLen + currSolLen; i++)
    {
        tabu[i] = (findArg((*currSol)[i - tabuLen], arr, argcount) + 1);

    }
    tabuLen = tabuLen + currSolLen;
    printf("\n1");
    /**step 1: add or drop random arguments*/
    cnt = 0;
    while(cnt + numberOfConlifctedSols < argcount && ((cnt < min || thresholdCnt < 10)&& cnt < max))
    {

        //choose a random argument that is not tabu and add tha chosen argument ro the tabu list so it is not tried again in this iteration
        int rand1 = myRandom(0, argcount-1, (tabu), tabuLen);

        if(containsInt(tabu, -1 * (rand1 + 1), tabuLen) != -1 || containsInt(tabu, (rand1 + 1), tabuLen) != -1)
        {
            numberOfConlifctedSols++;
            continue;
        }


        (tabu) = realloc((tabu), (tabuLen + 1) * sizeof(int));
        (tabu)[tabuLen] =  -1*(rand1 + 1);
        tabuLen++;

        //create ew solution with pos
        int oneSol = oneNewSol(rand1, &curAct, currSol, newSol, arr, currSolLen, newSolLen, argcount,
                    arrLen, realQualOfBest, semantic, punish, tabu, tabuLen);

        if(oneSol == 0)
        {

            //evaluate the qualiy of the currnt solution and check if it is the best one found in this iteration
            double* realQual;
            realQual = malloc(sizeof(double));


            double confFree = conflictFree(newSol, arr, (*newSolLen), argcount, arrLen, punish, realQual);

            double newQual;
            //try again if the solution was not conflict free
            if(confFree < 0.0)
            {
                printf("selstam");
                numberOfConlifctedSols++;
            }

            else
            {

                //add solution to neighborhood
                deleteRedundant(newSol, newSolLen);
                result = realloc(result, (resultLen + 1) * sizeof(char**));
                resultAtLen = realloc(resultAtLen, (resultLen + 1) * sizeof(char**));
                resultAtLen[resultLen] = *newSolLen;
                result[resultLen] = malloc((*newSolLen) * sizeof(char*));
                memcpy(result[resultLen], *newSol, (*newSolLen) * sizeof(char*));
                *actArr = realloc(*actArr, (resultLen + 1) * sizeof(int));
                (*actArr)[resultLen] = curAct;
                resultLen++;

                cnt++;
                rand_cnt++;


            }

        }

    }

    printf("\n1");

    char** att = NULL;
    char** def = NULL;
    int attLen = 0;
    int defLen = 0;

    printf("\n2");

    createAttacked(argcount, arr, currSol, arrLen, currSolLen, &att, &attLen);
    defend(&def, arr, &att, &defLen, attLen, argcount, arrLen);
    cnt = 0;
    /**step 2: add already defended elemets*/
    while(cnt < defLen && cnt < maxDef)
    {
        // choose and find the element
        int rand3 = myRandom(0, defLen - 1, NULL, 0);
        int rand3pos = findArg((def)[rand3], arr, argcount);

        //only proceed if the action is not forbidden
        if(containsInt(tabu, -1 * (rand3pos + 1), tabuLen) == -1 )
        {
            (tabu) = realloc((tabu), (tabuLen + 1) * sizeof(int));
            (tabu)[tabuLen] =  (rand3pos + 1);
            tabuLen++;

            int oneSol = oneNewSol(rand3pos, &curAct, currSol, newSol, arr, currSolLen, newSolLen, argcount,
                    arrLen, realQualOfBest, semantic, punish, tabu, tabuLen);


            //only proceed if the solution was successfully created
            if(oneSol == 0)
            {
                double* realQual;
                realQual = malloc(sizeof(double));

                curQuality = quality(newSol, arr, (*newSolLen), argcount, arrLen,
                                                problem, semantic, punish, realQual);
                double confFree = conflictFree(newSol, arr, (*newSolLen), argcount, arrLen, punish, realQual);

                if(confFree < 0.0)
                {
                    numberOfConlifctedSols++;
                }
                //add solution to neighborhood
                else
                {

                    deleteRedundant(newSol, newSolLen);
                    result = realloc(result, (resultLen + 1) * sizeof(char**));
                    resultAtLen = realloc(resultAtLen, (resultLen + 1) * sizeof(char**));
                    resultAtLen[resultLen] = *newSolLen;
                    result[resultLen] = malloc((*newSolLen) * sizeof(char*));
                    memcpy(result[resultLen], *newSol, (*newSolLen) * sizeof(char*));
                    *actArr = realloc(*actArr, (resultLen + 1) * sizeof(int));
                    (*actArr)[resultLen] = curAct;
                    resultLen++;
                    def_cnt++;

                }

            }
        }
        //do nothing
        else
        {

        }
        cnt++;

    }

    printf("\n3");

    char** unDefAtt = NULL;
    int unDefAttLen = 0;
    char** possibleAtt = NULL;
    int possibleAttLen = 0;

    createUndefendedAttacks(argcount, arr, currSol, arrLen, currSolLen, &def, defLen, &unDefAtt, &unDefAttLen);

    createAttackersOnUndefendedAttacks(&unDefAtt, unDefAttLen, arr, argcount, arrLen, &possibleAtt, &possibleAttLen);
    free(unDefAtt);

    printf("\n4");
    printf("attLen: %d ", possibleAttLen);
    cnt = 0;

    while(cnt < possibleAttLen && cnt < maxPossAtt)
    {
        // choose and find the element
        int rand4 = myRandom(0, possibleAttLen - 1, NULL, 0);
        int rand4pos = findArg((possibleAtt)[rand4], arr, argcount);

        //only proceed if the action is not forbidden
        if(containsInt(tabu, -1 * (rand4pos + 1), tabuLen) == -1  && containsInt(tabu, (rand4pos + 1), tabuLen) == -1 )
        {

            (tabu) = realloc((tabu), (tabuLen + 1) * sizeof(int));
            (tabu)[tabuLen] =  -1*(rand4pos + 1);
            tabuLen++;

            int oneSol = oneNewSol(rand4pos, &curAct, currSol, newSol, arr, currSolLen, newSolLen, argcount,
                    arrLen, realQualOfBest, semantic, punish, tabu, tabuLen);

            //only proceed if the solution was successfully created
            if(oneSol == 0)
            {

                double* realQual;
                realQual = malloc(sizeof(double));

                curQuality = quality(newSol, arr, (*newSolLen), argcount, arrLen,
                                                problem, semantic, punish, realQual);
                double confFree = conflictFree(newSol, arr, (*newSolLen), argcount, arrLen, punish, realQual);

                if(confFree < 0.0)
                {
                    numberOfConlifctedSols++;
                }
                //add solution to neighborhood
                else
                {

                    deleteRedundant(newSol, newSolLen);
                    result = realloc(result, (resultLen + 1) * sizeof(char**));
                    resultAtLen = realloc(resultAtLen, (resultLen + 1) * sizeof(char**));
                    resultAtLen[resultLen] = *newSolLen;
                    result[resultLen] = malloc((*newSolLen) * sizeof(char*));
                    memcpy(result[resultLen], *newSol, (*newSolLen) * sizeof(char*));
                    *actArr = realloc(*actArr, (resultLen + 1) * sizeof(int));
                    (*actArr)[resultLen] = curAct;
                    resultLen++;
                    att_cnt++;


                }

            }
        }
        //do nothing
        else
        {
        }
        cnt++;

    }
    free(possibleAtt);

        printf("\n5");
        cnt = 0;
    /** step 3: try to delete elements*/
    while(cnt < currSolLen && cnt < maxDel)
    {


        int rand2 = myRandom(0, currSolLen - 1, NULL, 0);
        int rand2pos = findArg((*currSol)[rand2], arr, argcount);

        if(containsInt(tabu, -1 * (rand2pos + 1), tabuLen) == -1 )
        {


            (tabu) = realloc((tabu), (tabuLen + 1) * sizeof(int));
            (tabu)[tabuLen] =  (rand2pos + 1);
            tabuLen++;
            int oneSol = oneNewSol(rand2pos, &curAct, currSol, newSol, arr, currSolLen, newSolLen, argcount,
                    arrLen, realQualOfBest, semantic, punish, tabu, tabuLen);




            double* realQual;
            realQual = malloc(sizeof(double));


            double confFree = conflictFree(newSol, arr, (*newSolLen), argcount, arrLen, punish, realQual);

            if(confFree < 0.0)
            {

                numberOfConlifctedSols++;
            }
            //add to neighborhood
            else if(oneSol == 0)
            {


                    deleteRedundant(newSol, newSolLen);
                    result = realloc(result, (resultLen + 1) * sizeof(char**));
                    resultAtLen = realloc(resultAtLen, (resultLen + 1) * sizeof(char**));
                    resultAtLen[resultLen] = *newSolLen;
                    result[resultLen] = malloc((*newSolLen) * sizeof(char*));
                    memcpy(result[resultLen], *newSol, (*newSolLen) * sizeof(char*));
                    *actArr = realloc(*actArr, (resultLen + 1) * sizeof(int));
                    (*actArr)[resultLen] = curAct;
                    resultLen++;
                    del_cnt++;

            }


        }
        else
        {

        }
        cnt++;

    }

    (*neighbors) = malloc(resultLen * sizeof(char**));
    *neighborAtLen = malloc(resultLen * sizeof(int));
    *neighbors = result;
    *neighborsLen = resultLen;
    *neighborAtLen = resultAtLen;
    free(tabu);

    for(int i = 0; i < resultLen; i++)
        printf("%d ", (*actArr)[i]);
    printf("\nrnd: %d, def: %d, del: %d, att: %d", rand_cnt, def_cnt, del_cnt, att_cnt);
    printf("\n done");
    return 0;
}


