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
         if (check_word (ptrLine, "mcr")) {

            ptrLine += 3;
            removeAllSpaces(ptrLine); /* we want only the name of the macro */

            if(invalidName(ptrLine)){

                illegalMacroName(error, lineCounter);
                continue;
            }

             inMcr = 1;
             skipMcr = 1;

             if (posFirstLineOfMcr == -1)
                 posFirstLineOfMcr = (int) ftell(amFile); /* save the position of the file pointer from the beginning of the file to the first line of the macro  */

             strcpy(mcr_name, ptrLine);
             continue;
        }

        /* we get to the end of the macro */
        if (check_word (ptrLine, "endmcr") && inMcr) {

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


/* Checks whether we have reached the word we are looking for */
int check_word(char *line, char *word){

    int i = 0;

    while(line[i] && line[i] != ',' && line[i] != ' ' && line[i] != '\t' && line[i] != '\n')
        i++;

    if(strlen(word) != i)
        return 0;

    if(!strncmp(word, line, i))
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


/* check if we reach to macro name after we past the block of this macro */
int it_is_mcrName(headMacro *mcr, char *line, FILE *asFile, FILE *source){

    int i = 0;
    char copyLine[LINE_LENGTH] = {'\0'};
    macroTable *tmp = mcr->head;

    while(tmp != NULL){

        if(check_word(line, tmp->mcrName)){

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

/* check if the name is instruction or guidance or register name, else return false */
int invalidName(char *name){

    int i = 0;
    /* name of instructors, directives and registers */
    char *directives [LEN_DIRECTIVES] = {".data", ".string", ".entry", ".extern"};
    char *instructions [LEN_INSTRUCTIONS] = {"mov", "cmp", "add", "sub", "lea", "not", "clr", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"};
    char *registers[LEN_REGISTERS] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};

    for (i = 0 ; i < LEN_INSTRUCTIONS ; i++){

        if(!strcmp(name, instructions[i]))
            return 1;
    }

    for (i = 0 ; i < LEN_DIRECTIVES ; i++){

        if(!strcmp(name, directives[i]))
            return 1;
    }

    for (i = 0 ; i < LEN_REGISTERS; i++){

        if(!strcmp(name, registers[i]))
            return 1;
    }

    return 0;
}

