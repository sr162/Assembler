#ifndef STRUCTURE_H
#define STRUCTURE_H
#include "error_messages.h"


typedef struct macro{
    char mcrName[LINE_LENGTH];
    int pos_of_firstMcrLine;
    int numOfLines;
    struct macro *next;
} macroTable;

typedef struct head_of_macroTable{
    macroTable *head;
}headMacro;


typedef struct symbol{
    char symName[SYMBOL_LENGTH];
    int value;
    char sign[MAX_LENGTH_TYPE];
    struct symbol *next;
}symbolTable;

typedef struct head_of_symbolTable{
    symbolTable *head;
}headSymbol;


typedef struct bit_line{
    unsigned int param:14;
}bit_line;

typedef struct data_table{
    bit_line *data;
    int first_IC;
    struct data_table *next;
}data_table;

typedef struct head_of_datatable{
    data_table *head;
}headData;


typedef struct essentials_param{
    int IC;
    int DC;
} essentials;


headMacro *createMacroTable();
headSymbol *createSymbolTable();
headData *createDataTable();
essentials *createEssentials();
void add_to_mcrTable(headMacro *headMcr, char *name, int posFirstLine, int linesAmount);
void add_to_symbolTable(headSymbol *sym, char *labelName, char *type, int IC);
void add_data_param(bit_line *bitLine, int num);
void add_data_line(headData *head_data, int num, int IC);
bit_line *create_bitLine();
void initialize_bit_line(bit_line *bitLine);
void free_macroTable(headMacro *list);
void free_symbolTable(headSymbol *list);
void free_dataLineTable(headData *list);


#endif /* STRUCTURE_H */