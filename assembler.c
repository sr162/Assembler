#include "assembler.h"

/* main function */
int main(int args, char * arguv[]){

    int i;
    checkNumOfArgu(args);

    for (i = 1; i < args ; i++){
        assembler_process(arguv[i]);
    }

    return 0;
}

/* Manages the transitions on the file and ends the program if there is an error in one of the transitions */
void assembler_process(char * nameOfFile){

    essentials *assem_param;
    headSymbol *head_symbol;
    headData *head_data;
    char nameOfAmFile[DATA_LENGTH] = {"\0"};
    char nameOfAsFile[DATA_LENGTH] = {"\0"};
    char nameOfExtFile[DATA_LENGTH] = {"\0"};
    char nameOfEntFile[DATA_LENGTH] = {"\0"};
    char nameOfObjFile[DATA_LENGTH] = {"\0"};
    int error = 0;
    int entry_flag = 0;
    int external_flag = 0;

    changeFileName(nameOfAmFile, nameOfFile, "am");
    changeFileName(nameOfAsFile, nameOfFile, "as");

    places_macros(nameOfAmFile, nameOfAsFile, &error); /* updates the am file by arranging the macros in the as file */

   if(error){

        printf("The program ended, an error was found in the pre-assembler.\n");
        remove(nameOfAsFile);
        return;
    }

    assem_param = createEssentials();  /* initialize IC to 100 and DC to 0 */
    head_data = createDataTable();
    head_symbol = createSymbolTable();

    first_pass(nameOfAsFile, assem_param, head_symbol, head_data, &error, &entry_flag, &external_flag); /* updates symbol table and a first search for errors in the file */

    if(error){

       free_symbolTable(head_symbol);
       free_dataLineTable(head_data);
       free(assem_param);
       remove(nameOfAsFile);

       printf("The program ended, an error was found in the first pass over the file.\n");
       return;
    }

    changeFileName(nameOfExtFile, nameOfFile, "ext");
    changeFileName(nameOfEntFile, nameOfFile, "ent");
    changeFileName(nameOfObjFile, nameOfFile, "ob"); /* the final file */

    second_pass(assem_param, head_symbol, head_data, nameOfAsFile, nameOfExtFile, nameOfEntFile, nameOfObjFile, &error, &external_flag, &entry_flag);

    free_symbolTable(head_symbol);
    free(assem_param);

    if(error){

        remove(nameOfAsFile);
        remove(nameOfObjFile);

        if(external_flag)
            remove(nameOfExtFile);
        if(entry_flag)
            remove(nameOfEntFile);

        printf("The program ended, an error was found in the second pass over the file.\n");
        exit(0);
    }

    printf("finished the assembler program for the file: %s .\n", nameOfAmFile);

}

/* change the ending of the file name */
void changeFileName (char *newNameFile, char *nameFile, char *endName){

    sprintf(newNameFile, "%s%s.txt", endName,nameFile );
}

