#include "error_messages.h"

/* check if we get enough arguments (>=2) and return ERROR if not */
void checkNumOfArgu (int args){
    if (args < 2){
        printf("ERROR : you need to write at list one file name .\n ");
        exit(0);
    }
}

/* check if we can open the file and return ERROR if not */
FILE * checkFileOpening(FILE * fd, char * fileToOpen, char * mode){

    if (!(fd = fopen(fileToOpen, mode))) {
        printf("ERROR : the file %s is not open. \n", fileToOpen);
        exit(0);
    }
    return fd;
}

void memoAllocationFail(void){

    printf("We have memory allocation error. \n");
    exit(0);
}

void illegalName(int line){

    printf("Error in line : %d\n", line);
    printf("The name is the name of a register or directive or instruction.\n\n");
}

void doubleLabelName(int line){

    printf("Error in line : %d\n", line);
    printf("Two different statements about a label with the same label name.\n\n");
}

void noSpaceOrTab(int line){

    printf("Error in line : %d\n", line);
    printf("There is no space or tab after me before the parameters.\n\n");
}

void InvalidComma(int line){

    printf("Error in line : %d\n", line);
    printf("illegal comma.\n\n");
}

void InvalidDataParam(int line){

    printf("Error in line : %d\n", line);
    printf("Invalid data parameter, you need to put only integers.\n\n");
}

void multipleCommas(int line){

    printf("Error in line : %d\n", line);
    printf("multiple consecutive commas.\n\n");
}

void multipleCommas(int line){

    printf("Error in line : %d\n", line);
    printf("multiple consecutive commas.\n\n");
}
