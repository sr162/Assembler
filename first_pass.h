#include "pre_assembler.h"
#include "error_messages.h"
#include "structure.h"
#include "constant.h"
#include <errno.h>

void first_pass(char *nameOfAsFile, essentials *assem_param, headSymbol *sym, headData *head_data_line, int *error, int *entry_flag, int *external_flag);
int is_label (char *line);
void save_label_name(char *line, char *label_name);
int validLabelName(headSymbol *sym, char *labelName, int type, int *error , int lineCounter);
int check_word(char *line, char *command);
void reading_data_param(essentials *assem_param, headData *head_data, char *line, int *error, int lineCounter);
void reading_string_param(essentials *assem_param, headData *head_data, char *line, int *error, int lineCounter);
void readingInstruction(essentials *assem_param, headData *head_data, char *line , int *error, int lineCounter);
void checkInstParam(essentials *assem_param, char *line , int *error, int lineCounter, int instructInd);
int check_register(char num);
int check_number(char *line);
void skipChars(char *line);