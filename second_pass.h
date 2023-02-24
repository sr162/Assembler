#include "first_pass.h"
#include "error_messages.h"
#include "structure.h"
#include "constant.h"
#include "manage_bit_line.h"

void second_pass(essentials *assem_param, headSymbol *head_symbol, headData *head_data, char *nameOfAsFile, char *nameOfExtFile, char *nameOfEntFile, char *nameOfObjFile, int *error, int *ext_flag, int *ent_flag);
int exists_label(headSymbol *symbol_head, char *label_name, int type, int *error, int lineCounter);
void put_in_ent_file(FILE *entfile, headSymbol *symbol_head, char *label_name, int lineCounter, int *error);
void put_in_ext_file(FILE *extfile, char *label_name, int IC);
void print_data_line(FILE *obj_file, essentials *assem_param, headData *head_data, int current_IC);
void read_op1(FILE *obj_file, FILE *ext_file, essentials *assem_param, headSymbol *head_symbol, char *ptrLine, int ind_instruction, int *error, int lineCounter);
void read_op2(FILE *obj_file, FILE *ext_file, essentials *assem_param, headSymbol *head_symbol, char *ptrLine, int ind_instruction, int *error, int lineCounter);
void jump_label(char *label_name, int *flag);



