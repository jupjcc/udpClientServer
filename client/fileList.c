#include "fileList.h"
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <ctype.h>      // isspace
#include <libgen.h>

#define MAX_NUM_FILES 100
#define MAX_FILE_NAME_LEN 132

int nFiles = 0;
int iFileSel = 0;
char fileNames[MAX_NUM_FILES][MAX_FILE_NAME_LEN];

static char *strstrip(char *s);

//  read from text file the list of files to be sent via udp 
int ReadFileList(char nameOfListFile[])
{
    char *baseName = basename(nameOfListFile);
    char *dirName = dirname(nameOfListFile);
    char *listPath = 0;
    if (baseName[0] != '@')
    {
        nFiles = 1;
        strcpy(&fileNames[0][0], nameOfListFile);
    }
    else
    {
        listPath = malloc(strlen(dirName) + strlen(baseName) - 1);
        sprintf(listPath, "%s/%s", dirName, &baseName[1]);
        printf("Reading list of files from %s\n", listPath);
        char lineBuf[1024];
        FILE *fptr;
        if ((fptr = fopen(listPath, "r")) == NULL)
        {
            printf("Unable to open list file %s\n", listPath);
            return 0;
        }
        nFiles = 0;
        while (fgets(lineBuf, sizeof(lineBuf), fptr) != NULL)
        {
            char *trimmed;
            trimmed = strstrip(lineBuf);
            if (strlen(trimmed) > 0 && trimmed[0] != '#')
            {
                if (nFiles >= MAX_NUM_FILES)
                {
                    printf("File list limited to max size of %d, ", MAX_NUM_FILES);
                    break;
                }
                strcpy(fileNames[nFiles++], trimmed);
            }
        }
    }
    return nFiles;
}

void GetNextFileName(char fName[])
{
    if (nFiles == 0 || iFileSel >= nFiles)
    {
        fName[0] = 0;
    }
    else
    {
        strcpy(fName, fileNames[iFileSel++]);
    }
    
}

static char *strstrip(char *s)
{
    size_t size;
    char *end;

    size = strlen(s);

    if (!size)
        return s;

    end = s + size - 1;
    while (end >= s && isspace(*end))
        end--;
    *(end + 1) = '\0';

    while (*s && isspace(*s))
        s++;

    return s;
}    
