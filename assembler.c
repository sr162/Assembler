#include "first_pass.h"
#include "second_pass.h"
#include "structure.h"

/* change the ending of the file name */
void changeNameFile (char *newNameFile, char *nameFile, char *endName){

    sprintf(newNameFile, "%s%s.txt", endName, nameFile);
}

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

    changeNameFile(nameOfAmFile, nameOfFile, "am");
    changeNameFile(nameOfAsFile, nameOfFile, "as");

    places_macros(nameOfAmFile, nameOfAsFile, &error); /* updates the am file by arranging the macros in the as file */

   if(error){

        printf("The program ended, an error was found in the pre-assembler.\n");
        remove(nameOfAsFile);
        remove(nameOfAmFile);
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
       remove(nameOfAmFile);
       printf("The program ended, an error was found in the first pass over the file.\n");
       return;
    }

    changeNameFile(nameOfExtFile, nameOfFile, "ext");
    changeNameFile(nameOfEntFile, nameOfFile, "ent");
    changeNameFile(nameOfObjFile, nameOfFile, "ob"); /* the final file */

    second_pass(assem_param, head_symbol, head_data, nameOfAsFile, nameOfExtFile, nameOfEntFile, nameOfObjFile, &error, &external_flag, &entry_flag);

    free_symbolTable(head_symbol);
    free(assem_param);

    if(error){

        remove(nameOfAsFile);
        remove(nameOfEntFile);
        remove(nameOfEntFile);
        remove(nameOfObjFile);
        printf("The program ended, an error was found in the second pass over the file.\n");
        exit(0);
    }

    if(!external_flag)
        remove(nameOfExtFile);

    if(!entry_flag)
        remove(nameOfEntFile);

    printf("finish.\n");

}


int main(int args, char * arguv[]){

    int i;
    checkNumOfArgu(args);

    for (i = 1; i < args ; i++){
        assembler_process(arguv[i]);
    }

    return 0;
}


