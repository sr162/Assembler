#include "structure.h"

/* in this function we allocate memory to head of the list of macros */
headMacro *createMacroTable(){

    headMacro *list = malloc(sizeof(headMacro));

    if(list == NULL){
        memoAllocationFail();
    }

    list->head = NULL;
    return list;
}

/* in this function we allocate memory to head of the list of symbols */
headSymbol *createSymbolTable(){

    headSymbol *list = malloc(sizeof(headSymbol));

    if(list == NULL){
        memoAllocationFail();
    }

    list->head = NULL;
    return list;
}


/* in this function we allocate memory to head of the list of data lines */
headData *createDataTable(){

    headData *list = malloc(sizeof(headData));

    if(list == NULL){
        memoAllocationFail();
    }

    list->head = NULL;
    return list;
}


/* in this function we allocate memory for the essential variables: IC, DC */
essentials *createEssentials(){

    essentials *assem_param = malloc(sizeof(essentials));

    if(assem_param == NULL){
        memoAllocationFail();
    }

    assem_param->DC = 0;
    assem_param->IC = 100;

    return assem_param;
}



/* if we do not have an allocation error we put all the data of the macro in the macro table, else we return error */
void add_to_mcrTable(headMacro *headMcr, char name[LINE_LENGTH], int posFirstLine, int linesAmount){

    macroTable *p = malloc(sizeof(macroTable));
    macroTable *tmp = headMcr->head; ;

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

        while (tmp->next != NULL) /* we want to insert p to the last macro in the list */
            tmp = tmp->next;

        tmp->next = p;
    }
}


/* if we do not have an allocation error we put all the symbol parameters in the symbol table, else we return error */
void add_to_symbolTable(headSymbol * sym, char * labelName, char * type, int IC){

    symbolTable *p = malloc(sizeof(symbolTable));
    symbolTable  *tmp ;

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

        while (tmp->next != NULL) /* we want to insert p to the last label in the list */
            tmp = tmp->next;

        tmp->next = p;
    }
}


void add_data_param(bit_line *bitLine, int num){

    int i = 0;

    for(i = 0; i < 14; i++){

        if(num & (1<< i))
            bitLine->param |= (1 << i);
    }
}


/* if we do not have an allocation error we put all the data line parameters in the data line table, else we return error */
void add_data_line(headData *head_data, int num, int IC){

    data_table *p = malloc(sizeof(data_table));
    data_table *tmp = head_data->head;

    if(p == NULL){
        memoAllocationFail();
    }

    /* save the parameter of the data line we want to save */
    p->data =  create_bitLine();
    add_data_param(p->data, num);
    p->first_IC = IC;
    p->next = NULL;

    if (!head_data->head) /* if the first data line is not install */
        head_data->head = p;

    else{

        while(tmp->next != NULL) /* we want to insert p to the last data line in the list */
            tmp = tmp->next;

        tmp->next = p;
    }
}


/* if we do not have an allocation error initialize all the data line parameters and return a pointer to this line */
bit_line *create_bitLine(){

    bit_line *p = malloc(sizeof(bit_line));

    if(!p)
        memoAllocationFail();

    p->param = 0;

    return p;
}

void initialize_bit_line(bit_line *bitLine){

    bitLine->param = 0;
}



/* check if the name is instruction or guidance or register name, else return false */
int invalidName(char *name){

    int i ;
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


void free_macroTable(headMacro *list){

    macroTable *tmp = list->head;

    while(list->head != NULL){

        list->head = list->head->next;
        free(tmp);
        tmp = list->head;
    }

    free(list);
}

void free_symbolTable(headSymbol *list){

    symbolTable *tmp = list->head;

    while(list->head != NULL){

        list->head = list->head->next;
        free(tmp);
        tmp = list->head;
    }

    free(list);
}

void free_dataLineTable(headData *list){

    data_table *tmp = list->head;

    while(list->head != NULL){

        list->head = list->head->next;
        free(tmp->data);
        free(tmp);
        tmp = list->head;
    }

    free(list);
}
