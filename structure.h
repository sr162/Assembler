#include "error_messages.h"

char *guidelines [LEN_GUIDELINES] = {".data", ".string", ".entry", ".extern"};
char *instructions [LEN_INSTRUCTIONS] = {"mov", "cmp", "add", "sub", "lea", "not", "clr", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"};
char * regesters[LEN_REGESTIERS] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};

typedef struct macro{
    char mcrName[LINE_LENGTH];
    int pos_of_firstMcrLine;
    int numOfLines;
    struct macro * next;
} macroTable;

typedef struct head_of_macroTable{
    macroTable *head;
}headMacro;

typedef struct symbol{
    char symName[SYMBOL_LENGTH];
    int value;
    char sign[MAX_LENGTH_TYPE];
    struct symbol * next;
}symbolTable;

typedef struct head_of_symbolTable{
    symbolTable *head;
}headSymbol;



headMacro * createMacroTable();
headSymbol * createSymbolTable();
