#include "pre_assembler.h"

int which_instruction(char * line);
int is_label (headSymbol * sym, char * line, char name[SYMBOL_LENGTH], int * error, int lineCounter);
int uniqueLabelName(headSymbol * sym, char * labelName, int * error , int lineCounter)
void put_in_symbolTable(char * labelName, headSymbol * sym, char * type, int IC, int * error);

