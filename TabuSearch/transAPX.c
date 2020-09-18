#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <regex.h>
#include <stdbool.h>

/**
*  swaps two elements in the array
*/
void swap(char ** a, char ** b)
{
    char * t = *a;
    *a = *b;
    *b = t;
}


/**
*  partitions a one-dimensiional array for the quicksort alg
*  pivot element = arr[high]
*/
int partition (char ** arr, int low, int high)
{
    char * pivot = arr[high];    // pivot
    int i = (low - 1);  // Index of smaller element

    for (int j = low; j <= high- 1; j++)
    {
        // If current element is smaller than the pivot
        if (strcmp(arr[j], pivot) < 0)
        {
            i++;    // increment index of smaller element

            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}

/**
*  partitions a two-dimensiional array for the quicksort alg
*  pivot element = arr[high]
*/
int partition1 (char** *arr, int low, int high)
{
    char * pivot = arr[high][0];    // pivot
    int i = (low - 1);  // Index of smaller element

    for (int j = low; j <= high- 1; j++)
    {
        // If current element is smaller than the pivot
        if (strcmp(arr[j][0], pivot) < 0)
        {
            i++;    // increment index of smaller element

            swap(&arr[i][0], &arr[j][0]);
            swap(&arr[i][1], &arr[j][1]);

        }
    }
    swap(&arr[i + 1][0], &arr[high][0]);
    swap(&arr[i + 1][1], &arr[high][1]);
    return (i + 1);
}

/**
*  quicksort algortihm for a one-dimensional array
*/
void quickSort(char* *arr, int low, int high)//quicksort for arguments
{
    if (low < high)
    {
        /* pi is partitioning index, arr[p] is now
           at right place */
        int pi = partition(arr, low, high);
        // Separately sort elements before
        // partition and after partition
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

/**
*  quicksort algortihm for a two-dimensional array
*/
void quickSort1(char** *arr, int low, int high)//quicksort for attacks
{
    if (low < high)
    {
        /* pi is partitioning index, arr[p] is now
           at right place */
        int pi = partition1(arr, low, high);
        // Separately sort elements before
        // partition and after partition
        quickSort1(arr, low, pi - 1);
        quickSort1(arr, pi + 1, high);
    }
}

/**
*  translate an APX file in a one dimensional argument array arg[i] & a two dimensional attack array att[i][j]
*  (the attacker is in att[i][0], the attacked is in att[i][1]
*/
int translateAPX(FILE *fp, char*** argarray, char**** attarray, int* argcnt, int* attcnt)
{

    char * line = NULL;
    size_t len = 0;
    size_t read;
    regex_t regexArg;
    regex_t regexAtt;
    int argFind;
    int attFind;

    //define arrays for arguments and attacks
    //char* *argarray = NULL;//argments is an string array
    int i = 0, strcount = 0;
    //char* **attarray = NULL;//attackers is an array of string arrays
    int j = 0, strcount1 = 0;


//***********************define regular expressions to find "arg" and "att" in file***********************
    argFind = regcomp(&regexArg, "arg", 0);
        if( argFind ){ fprintf(stderr, "Could not compile regex for arguments\n"); exit(1);}
    attFind = regcomp(&regexAtt, "att", 0);
        if( attFind ){ fprintf(stderr, "Could not compile regex for attacks\n"); exit(1);}

    if (fp == NULL)//if File does not exist
        exit(EXIT_FAILURE);
    bool readArg;
    readArg  = false;

    while ((read = getline(&line, &len, fp)) != -1) {//iterate ove rall lines of input file

        argFind = regexec(&regexArg, line, 0, NULL, 0);
        attFind = regexec(&regexAtt, line, 0, NULL, 0);

//***********************remove bracktes, dots and names (att&arg)*********************

        char * start = line;
        start = start + 4; //remove start
        char* closingBracket = strchr(start, ')');
        int indexOfClosingBracket = (int)(closingBracket - start);
        for(int i = strlen(start)-1; i >= indexOfClosingBracket; i--)
            start[i] = 0;
//***********************find "att" and "arg" and add the info to the respective arrays***********************

        if( !argFind ){
            *argarray = (char**)realloc(*argarray, (strcount + 1) * sizeof(char *));
            (*argarray)[strcount++] = strdup(start);

        }

        if( !attFind ){
            char* word;
            word = strtok(start, ","); //cut string at ","
            *attarray = (char***)realloc(*attarray, (strcount1+1)*sizeof(char**));
            (*attarray)[strcount1] = (char**)malloc( 2 * sizeof(char *));
            ((*attarray)[strcount1])[0] = strdup(word);//enter the attacker in [0]
            word = strtok(NULL, ",");
            ((*attarray)[strcount1])[1] = strdup(word);//enter the attacked in [1]
            strcount1++;

        }

    }
//***********sort the arrays to accelarate finding elements*************************
    quickSort(*argarray, 0, strcount-1);
    quickSort1(*attarray, 0, strcount1-1);//attack array is sorted by attacker





    if (line)
        free(line);
    //give the length of both arrays
    *argcnt = strcount;
    *attcnt = strcount1;

    return 0;
}
