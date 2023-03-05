#include "error_messages.h"

/* check if we get enough arguments (>=2) and return ERROR if not */
void checkNumOfArgu (int args){
    if (args < 2){
        printf("ERROR : you need to write at list one file name .\n ");
        exit(0);
    }
}

/* check if we can open the file and return ERROR if not */
FILE *checkFileOpening(FILE *fd, char *fileToOpen, char *mode, int *error){

    if (!(fd = fopen(fileToOpen, mode))) {

        printf("ERROR : the file %s is not open. \n", fileToOpen);
        *error = 1;
        return NULL;
    }
    return fd;
}

void memoAllocationFail(void){

    printf("You have memory allocation error. \n");
    exit(0);
}

void lineLengthLong(int *error, int line){

    printf("Error in line : %d\n", line);
    printf("The line length is longer than 81 characters.\n\n");
    *error = 1;
}

void illegalLabelName(int *error, int line){

    printf("Error in line : %d\n", line);
    printf("The label name is the name of a register or directive or instruction command\n");
    printf("or the label has characters other than A-Z or a-z or 0-9 \n\n");
    *error = 1;
}

void illegalMacroName(int *error, int line){

    printf("Error in line : %d\n", line);
    printf("The macro name is the name of a register or directive or instruction command\n");
    printf("or the macro has characters other than A-Z or a-z or 0-9 \n\n");
    *error = 1;
}

void longLabelName(int *error, int line){

    printf("Error in line : %d\n", line);
    printf("The label name is longer than 30 characters.\n\n");
    *error = 1;
}

void spaceInLabelStatement(int *error, int line){

    printf("Error in line : %d\n", line);
    printf("There are spaces in the label statement.\n\n");
    *error = 1;
}

void doubleLabelName(int *error, int line){

    printf("Error in line : %d\n", line);
    printf("Two different statements about a label with the same label name.\n\n");
    *error = 1;
}

void extraTextContinue(int *error, int line){

    printf("Error in line : %d\n", line);
    printf("Extraneous text.\n\n");
    *error = 1;
}

void noSpaceOrTab(int *error, int line){

    printf("Error in line : %d\n", line);
    printf("There is no space or tab before the parameters.\n\n");
    *error = 1;
}

void invalidComma(int *error, int line){

    printf("Error in line : %d\n", line);
    printf("Invalid comma.\n\n");
    *error = 1;
}

void noParam(int * error, int line){

    printf("Error in line : %d\n", line);
    printf("You didn't insert parameters after command word.\n\n");
    *error = 1;
}

void invalidDataParam(int *error, int line){

    printf("Error in line : %d\n", line);
    printf("Invalid data parameter, Only integers preceded by '-' or '+' or nothing are allowed.\n\n");
    *error = 1;
}

void invalidString(int *error, int line){

    printf("Error in line : %d\n", line);
    printf("Invalid string, string must to start with \" and finish with \".\n\n");
    *error = 1;
}

void multipleCommas(int *error, int line){

    printf("Error in line : %d\n", line);
    printf("Multiple consecutive commas.\n\n");
    *error = 1;
}

void missingComma(int *error, int line){

    printf("Error in line : %d\n", line);
    printf("Missing a comma.\n\n");
    *error = 1;
}

void irrelevantLabel(int *error, int line){

    printf("Error in line : %d\n", line);
    printf("Setting a label before .extern or .entry makes the label irrelevant.\n\n");
    *error = 1;
}

void undefinedDirCommand(int *error, int line){

    printf("Error in line : %d\n", line);
    printf("Undefined directive command.\n\n");
    *error = 1;
}

void undefinedInstructCommand(int *error, int line){

    printf("Error in line : %d\n", line);
    printf("Undefined instruction command.\n\n");
    *error = 1;
}

void IllegalInstParam(int *error, int line){

    printf("Error in line : %d\n", line);
    printf("Illegal parameter.\n\n");
    *error = 1;
}

void invalidRegisterParam(int *error, int line){

    printf("Error in line : %d\n", line);
    printf("Invalid parameter of register type, the registers begin with 'r' and after a number from 0 to 7.\n\n");
    *error = 1;
}

void invalidNumberParam(int *error, int line){

    printf("Error in line : %d\n", line);
    printf("Invalid parameter of type number, numbers are defined as follows : start with '#' and \n");
    printf("can include the symbols '-', '+' and any integer number \n\n");
    *error = 1;
}

void noLabelStatement(int *error, int line){

    printf("Error in line : %d\n", line);
    printf("There is no label statement or the name of the label does \n");
    printf("not exist in the symbol table.\n\n");
    *error = 1;
}

void labelCannotBeEnt(int *error, int line){

    printf("Error in line : %d\n", line);
    printf("The label name is defined as external.\n\n");
    *error = 1;
}


void exceededDataAmount(int *error){

    printf("ERROR : we have exceeded the maximum amount of data that can be used to run the program.\n\n");
    *error = 1;
}