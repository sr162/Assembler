#include "first_pass.h"

char * first_pass(char* amFile){

    FILE * fd ;
    int DC = 0;
    int IC = 0;
    int skipLabel = 0;
    int lineCounter = 0;
    int * error = 0;
    char line[LINE_LENGTH] ={'\0'};
    char * ptrLine;
    headSymbol * sym ;

    sym = createSymbolTable();
    fd = checkFileOpening(fd, amFile, "r+");

    while(fgets(line, LINE_LENGTH, fd) != NULL){

        int numParm = 0;
        char label_name[SYMBOL_LENGTH] = {'\0'};
        char symbol_type [MAX_LENGTH_TYPE] = {'\0'}; /* save the type of the symbol ( code / data / external / entry ) */
        int label_flag = 0;
        int indInstruct = -1;
        int indGuidance = -1;
        ptrLine = line;
        lineCounter++;

        skipSpacesAndTabs(ptrLine);

        /* if it is an empty line we skip */
        if(*ptrLine == '\n' || *ptrLine == '\0')
            continue;

        /* if it is a comment line */
        if(*ptrLine == ';') \
            continue;

        label_flag = is_label (sym, ptrLine, label_name, error, lineCounter); /* if the name is valid we save it in the label_name */

        if(label_flag) {

            ptrLine += strlen(label_name); /* point on the operands */
            skipSpacesAndTabs(ptrLine);

            if (*ptrLine == '.') {

                if(!strncmp(".data", ptrLine, 5) || !strncmp(".string", ptrLine, 7)) {

                    strcpy(symbol_type, "data");
                    put_in_symbolTable(sym, label_name, symbol_type, DC, error);

                    if(!strncmp(".data", ptrLine, 5)){

                        ptrLine += 5;
                        skipSpacesAndTabs(ptrLine);
                        reading_data_param()
                    }

                }
            }
        }


        if((indInstruct = which_instruction(ptrLine)) != -1){

            param = legalParam(ptrLine + strlen(instructions[indInstruct]), numParm);
            doTheInstruct(indInstruct, param);
        }

        else if((indGuidance = which_guidance(ptrLine)) != -1){

        }
    }
}

/* return true if we have reached a declaration of a valid symbol */
int is_label (headSymbol * sym, char * line, char name[SYMBOL_LENGTH], int * error, int lineCounter) {

    name = strtok(line, " \t");
    int len = strlen(name);

    if (name[len - 1] == ':') { /* it is a label */

        name[len - 1] = '\0';
        return uniqueLabelName(sym, name, error, lineCounter);
    }

    return 0;
}


int uniqueLabelName(headSymbol * sym, char * labelName, int * error , int lineCounter){

    symbolTable  * tmp ;

    if(unValidName(labelName)){ /* check if the name of the label is a instruction or guidance or register name */

        illegalName(lineCounter);
        *error = 1;
        return 0;
    }

    if (!sym->head)
        return 1;

    tmp = sym->head;

    while(!tmp) { /* check if the new label name is unique */

        if (!strcmp(labelName, tmp->symName)) {

            doubleLabelName(lineCounter);
            *error = 1;
            return 0;
        }

        tmp = tmp->next;
    }

    return 1;
}


void put_in_symbolTable(char * labelName, headSymbol * sym, char * type, int IC, int * error){

    symbolTable * p = malloc(sizeof(symbolTable *));
    symbolTable  * tmp ;

    if (!p)
        memoAllocationFail();

    /* save all the parameters of the Label we want to save */
    strcpy(p->symName, labelName) ;
    strcpy(p->sign, type);
    p->value = IC;
    p->next = NULL;

    if(!(sym->head)) /* if the first label is not install */
        sym->head = p;

    else{
        tmp = sym->head;
        while (tmp->next != NULL) /* we want to insert p to the last macro in the list */
            tmp = tmp->next;

        tmp->next = p;
    }

}


int which_instruction(char * line){

    char * p = line;
    int i;
    char * word ;

    word = strtok(p, " \t\n");

    for(i = 0 ; i < LEN_INSTRUCTIONS ; i++){
        if (!strcmp(word, instructions[i]))
            return i;
    }
    return -1;
}


char * legalParam(char * line, int * numParam){

    char * param;
    if (*line != ' ' && *line != '\t')


    skipSpacesAndTabs(line);
    param = strtok(param, " \t,");
    if (strcmp(regesters))

}