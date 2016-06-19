/***************************************************************************
*  File: alias.c                                 an addition to CircleMUD  *
*  Usage: saving player's aliases                                          *
*                                                                          *
*  a drop-in replacement for alias.c                                       *
*  written by Edward Almasy (almasy@axis.com)                              *
*                                                                          *
*  (original alias.c by Jeremy Hess and Chad Thompson)                     *
*                                                                          *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University  *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.                *
***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "conf.h"
#include "sysdep.h"
#include "structs.h"
#include "utils.h"
#include "interpreter.h"


void write_aliases(struct char_data *ch)
{
    FILE         *ptFHndl;
    char          pcFileName[127];
    struct alias *pstAliasRec;
    char         *pcRepStart;
  
    /* get name of alias file */
    get_filename(GET_NAME(ch), pcFileName, ALIAS_FILE);
  
    /* remove old alias file */
    unlink(pcFileName);
  
    /* if no aliases */
    if (!GET_ALIASES(ch))
        /* abort */
        return;
  
    /* open new alias file */
    ptFHndl = fopen(pcFileName,"wt");
  
    /* while there are alias records left */
    pstAliasRec = GET_ALIASES(ch);
    while (pstAliasRec)
    {
        /* write out command */
        fprintf(ptFHndl, "%d\n", strlen(pstAliasRec->alias)); 
        fprintf(ptFHndl, "%s\n", pstAliasRec->alias);
       
        /* remove leading spaces on replacement */
        pcRepStart = pstAliasRec->replacement; 
        while (*pcRepStart == ' ') pcRepStart++;
        
        /* write out replacement */
        fprintf(ptFHndl, "%d\n", strlen(pcRepStart)); 
        fprintf(ptFHndl, "%s\n", pcRepStart);
        fprintf(ptFHndl, "%d\n", pstAliasRec->type);
       
        /* move to next alias record */
        pstAliasRec = pstAliasRec->next;
    }
    
    /* close new alias file */
    fclose(ptFHndl);
}


void read_aliases(struct char_data *ch)
{   
    char          pcFileName[127];
    FILE         *ptFHndl;
    struct alias *pstAliasRec;
    int           iLen;
    
    /* get alias file name */
    get_filename(GET_NAME(ch), pcFileName, ALIAS_FILE);
    
    /* open alias file */
    ptFHndl = fopen(pcFileName, "r");
    
    /* if file open failed */
    if (ptFHndl == NULL)
        /* abort */
        return;
    
    /* create initial alias record */
    CREATE(pstAliasRec, struct alias, 1);
    GET_ALIASES(ch) = pstAliasRec;

    /* while not end of alias file */
    while (!feof(ptFHndl))
    {
        /* read in length of command */
        fscanf(ptFHndl, "%d\n", &iLen);

        /* allocate command string */
        CREATE(pstAliasRec->alias, char, (iLen + 2));

        /* read in command */
        fgets(pstAliasRec->alias, (iLen+1), ptFHndl);

        /* read in length of replacement */
        fscanf(ptFHndl, "%d\n", &iLen);

        /* allocate replacement string */
        CREATE(pstAliasRec->replacement, char, (iLen + 3));
        *pstAliasRec->replacement = ' ';;

        /* read in replacement */
        fgets((pstAliasRec->replacement + 1), (iLen+1), ptFHndl);

        /* read in type */
        fscanf(ptFHndl, "%d\n", &pstAliasRec->type);

        /* if there are aliases left to read */
        if (!feof(ptFHndl))
        {
            /* create new alias record and link into chain */
            CREATE(pstAliasRec->next, struct alias, 1);
            pstAliasRec = pstAliasRec->next;
            pstAliasRec->next = NULL;
        }
    }

    /* close alias file */
    fclose(ptFHndl);
} 

