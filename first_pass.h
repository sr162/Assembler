#include "pre_assembler.h"
#include "error_messages.h"
#include "structure.h"
#include "constant.h"
#include <errno.h>

void first_pass(char *nameOfAsFile, essentials *assem_param, headSymbol *head_symbol, headData *head_data_line, int *error, int *entry_flag, int *external_flag);
int is_label (char *line);
int save_and_check_label_name(headSymbol *head_symbol, char *line, char *label_name, int type, int *error , int lineCounter);
void reading_data_param(essentials *assem_param, headData *head_data, char *line, int *error, int lineCounter);
void reading_string_param(essentials *assem_param, headData *head_data, char *line, int *error, int lineCounter);
void readingInstruction(essentials *assem_param, headData *head_data, char *line , int *error, int lineCounter);
void checkInstOperand(essentials *assem_param, char *line , int *error, int lineCounter, int instructInd);
int check_register(char num);
int check_number(char *line);
void skipChars(char *line);