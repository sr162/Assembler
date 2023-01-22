#include "main.h"

int main(int args, char * arguv[]){

    int i;
    checkNumOfArgu(args);

    for (i = 1; i < args ; i++){
        assembler_process(arguv[i]);
    }

    return 0;
}

void assembler_process(char * asFile){

    char * nameOfAmFile, lastFile;

    nameOfAmFile = places_macros(asFile); /* return the name of the am file */

   /* lastFile = first_pass(nameOfAmFile); /* updates symbol table and  */

     /*second_transition(lastFile);*/
}
