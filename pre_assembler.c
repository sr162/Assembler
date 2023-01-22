#include "pre_assembler.h"

char* places_macros(char * asFile){

    FILE * sourceFile = NULL;
    FILE * amFile = NULL;
    char line [LINE_LENGTH]= {'\0'};
    char mcr_name[LINE_LENGTH] = {'\0'};
    char copyLine [LINE_LENGTH]= {'\0'};
    int posFirstLineOfMcr = -1 ; /* the position of sourceFile in the file with ftell() */
    int countMacroLines = 0; /* count the amount lines in the macro */
    int inMcr = 0, skipMcr = 0; /* flag if we are point in macro block, flag if we past at list one macro */
    headMacro * mcr ; /* head of the macro list */

    mcr = createMacroTable();
    sourceFile = checkFileOpening(sourceFile, asFile, "r");
    amFile = checkFileOpening(amFile, changeToAm(asFile), "w+");

    while (fgets(line, LINE_LENGTH, sourceFile) != NULL) { /* we run until we get to the end of file */

        char *ptrLine;
        fpos_t pos;
        fgetpos(sourceFile, &pos);
        ptrLine = line;
        strcpy(copyLine, line);
        skipSpacesAndTabs(ptrLine);

        /* we check if we get to macro block */
         if (checkWord (ptrLine, "mcr")) {

            ptrLine += 3;
            removeAllSpaces(ptrLine); /* we want only the name of the macro */

            if (unValidName(ptrLine))
                continue;

            else {
                inMcr = 1;
                skipMcr = 1;

                if (posFirstLineOfMcr == -1)
                    posFirstLineOfMcr = ftell(
                            sourceFile); /* save the position of the file pointer from the beginning of the file to the first line of the macro  */

                strcpy(mcr_name, ptrLine);
            }
        }

        /* we get to the end of the macro */
        else if (checkWord (ptrLine, "endmcr")) {

            inMcr = 0;
            put_in_mcrTable(mcr, mcr_name, posFirstLineOfMcr, countMacroLines);
            countMacroLines = 0;
            posFirstLineOfMcr = -1 ;
        }

         /* we in the macro block */
         else if (inMcr) {

             countMacroLines++;
         }

        /* when we are not in mcr we copy the line to the as file , or we copy mcr lines insted of mcr name*/
        else {

             if (skipMcr) {

                 if (it_is_mcrName(mcr, ptrLine, amFile, sourceFile)) {
                     fsetpos(sourceFile, &pos);
                     continue;
                 }
             }

             fputs(copyLine, amFile);
         }
    }

    free(mcr);
    fclose(sourceFile);
    fclose(amFile);

    return changeToAm(asFile);
}

/* change the last char in the name of the file that given from s to m (.as -> .am) */
char * changeToAm (char * fileAs){

    /*int lenNameFileAs = strlen(fileAs);*/

    fileAs = "amFile.txt" ;
   /* fileAs[lenNameFileAs-1] = 'm' ;*/

    return fileAs;
}

/* check if we get to macro block */
int checkWord (char line[LINE_LENGTH], char * word){

    int i = 0;
    int letterCounter = 0;

    while((line[i] >= 'A' && line[i] <= 'Z') || (line[i] >= 'a' && line[i] <= 'z') || (line[i] >= 0 && line[i] <= 9) || line[i] == '#' || line[i] == '.' ){

        letterCounter++;
        i++;
    }

    if(strlen(word) < letterCounter)
        return 0;

    if(strncmp(line, word, strlen(word) ) == 0 )
        return 1;


    return 0;
}


/* skip the spaces and tabs in the beginning */
void skipSpacesAndTabs(char *p) {

    int i;

    for (i = 0;  *p == ' ' || *p == '\t' ; i++) {
        memmove(p, p+1, strlen(p));
    }
}


/* remove all spaces from a given string */
void removeAllSpaces(char *p) {

    int i, count = 0; /* To keep track of non-space character count */

    /* Traverse the given string. If current character
     * is not space, then place it at index 'count++' */
    for (i = 0; p[i] != '\n' ; i++) {

        if (p[i] != ' ' && p[i] != '\t')
            p[count++] = p[i]; /* here count is incremented */
    }

    p[count] = '\0'; /* finish the string */
}

/* check if the name of the macro is a instruction or guidance or register name */
int inValidName(char * name){

    int i ;

    for (i = 0 ; i < LEN_INSTRUCTIONS ; i++){

        if(!strcmp(name, instructions[i]))
            return 1;
    }

    for (i = 0 ; i < LEN_GUIDELINES ; i++){

        if(!strcmp(name, guidelines[i]))
            return 1;
    }

    for (i = 0 ; i < LEN_REGESTIERS; i++){

        if(!strcmp(name, regesters[i]))
            return 1;
    }

    return 0;
}

/* we put all the data of the macro in the macro table */
void put_in_mcrTable(headMacro *headMcr, char name[LINE_LENGTH], int posFirstLine, int linesAmount){

    macroTable * p = malloc(sizeof(macroTable *));
    macroTable * tmp ;
    if (!p)
        memoAllocationFail();

    /* save all the parameters of the macro we want to save */
    strcpy(p->mcrName, name) ;
    p->pos_of_firstMcrLine = posFirstLine;
    p->numOfLines = linesAmount;
    p->next = NULL;

    if(!(headMcr->head)) /* if the first macro is not install */
        headMcr->head = p;

    else{
        tmp = headMcr->head;
        while (tmp->next != NULL) /* we want to insert p to the last macro in the list */
            tmp = tmp->next;

        tmp->next = p;
    }
}

/* check if we get to macro name after we past the block of this macro */
int it_is_mcrName(headMacro * mcr, char * line, FILE * amFile, FILE * source){

    int i = 0;
    char copyLine[LINE_LENGTH] = {'\0'};
    macroTable *tmp;
    tmp = mcr->head;

    while(tmp != NULL){

        if(checkWord(line, tmp->mcrName)){

            fseek(source, tmp->pos_of_firstMcrLine, SEEK_SET); /* we point with the file pointer to the start of the macro first line */

            while(i < tmp->numOfLines){

                fgets(copyLine, LINE_LENGTH, source);
                fputs(copyLine, amFile);
                i++ ;
            }
            return 1;
        }
        tmp = tmp->next;
    }

    return 0;
}




