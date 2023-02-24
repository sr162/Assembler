#include "pre_assembler.h"

void places_macros(char *nameOfAmFile, char *nameOfAsFile, int *error){

    FILE *amFile = NULL;
    FILE *asFile = NULL;
    char line [LINE_LENGTH]= {'\0'};
    char mcr_name[LINE_LENGTH] = {'\0'};
    char copyLine [LINE_LENGTH]= {'\0'};
    int posFirstLineOfMcr = -1 ; /* the position of amFile in the file with ftell() */
    int countMacroLines = 0; /* count the amount lines in the macro */
    int inMcr = 0, skipMcr = 0; /* flag if we are point in macro block, flag if we past at list one macro */
    int lineCounter = 0;
    headMacro *mcr = createMacroTable(); /* head of the macro list */

    if(!(amFile = checkFileOpening(amFile, nameOfAmFile, "r+", error)))
        return;

    if(!(asFile = checkFileOpening(asFile, nameOfAsFile, "w+", error)))
        return;

    while (fgets(line, LINE_LENGTH, amFile) != NULL) { /* we run until we get to the end of file */

        char *ptrLine = line;
        fpos_t pos;
        fgetpos(amFile, &pos);
        strcpy(copyLine, line);
        lineCounter++;

        skipSpacesAndTabs(ptrLine);
        /* we check if we get to macro block */
         if (checkWord (ptrLine, "mcr")) {

            ptrLine += 3;
            removeAllSpaces(ptrLine); /* we want only the name of the macro */

            if (!invalidName(ptrLine ,error, lineCounter)){

                inMcr = 1;
                skipMcr = 1;

                if (posFirstLineOfMcr == -1)
                    posFirstLineOfMcr = (int) ftell(amFile); /* save the position of the file pointer from the beginning of the file to the first line of the macro  */

                strcpy(mcr_name, ptrLine);
                continue;
            }
        }

        /* we get to the end of the macro */
        if (checkWord (ptrLine, "endmcr") && inMcr) {

            inMcr = 0;
            add_to_mcrTable(mcr, mcr_name, posFirstLineOfMcr, countMacroLines);
            countMacroLines = 0;
            posFirstLineOfMcr = -1 ;
            continue;
        }

         /* we in the macro block */
         if (inMcr) {

             countMacroLines++;
             continue;
         }

         if (skipMcr) {

             if (it_is_mcrName(mcr, ptrLine, asFile, amFile)) {

                 fsetpos(amFile, &pos);
                 continue;
             }
         }

        fputs(copyLine, asFile);
    }

    free_macroTable(mcr);
    fclose(amFile);
    fclose(asFile);
}

void printMacro(headMacro *head){

    macroTable *tmp = head->head;
    if(tmp == NULL)
        printf("NULL\n");
    while(tmp != NULL){

        printf("%s, %d, %d\n", tmp->mcrName, tmp->numOfLines, tmp->pos_of_firstMcrLine);
        tmp = tmp->next;
    }

}

/* check if we get to macro block */
int checkWord (char *line, char *word){

    int i = 0;
    int letterCounter = 0;
    int lenWord = strlen(word);

    while((line[i] >= 'A' && line[i] <= 'Z') || (line[i] >= 'a' && line[i] <= 'z') || (line[i] >= '0' && line[i] <= '9') || line[i] == '#' || line[i] == '.' ){

        letterCounter++;
        i++;
    }

    if(lenWord != letterCounter)
        return 0;


    if(strncmp(line, word, letterCounter) == 0)
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


/* check if we get to macro name after we past the block of this macro */
int it_is_mcrName(headMacro *mcr, char *line, FILE *asFile, FILE *source){

    int i = 0;
    char copyLine[LINE_LENGTH] = {'\0'};
    macroTable *tmp = mcr->head;

    while(tmp != NULL){

        if(checkWord(line, tmp->mcrName)){

            fseek(source, tmp->pos_of_firstMcrLine, SEEK_SET); /* we point with the file pointer to the start of the macro first line */

            while(i < tmp->numOfLines){

                fgets(copyLine, LINE_LENGTH, source);
                fputs(copyLine, asFile);
                i++ ;
            }
            return 1;
        }
        tmp = tmp->next;
    }

    return 0;
}

