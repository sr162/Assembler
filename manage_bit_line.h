#ifndef MANAGE_BIT_LINE_H
#define MANAGE_BIT_LINE_H
#include "constant.h"
#include "structure.h"

void add_classify(bit_line *bitLine, int which_classify);
void add_opcode(int instNum, bit_line *bitLine);
void add_param(int param, int which_param, bit_line *bitLine);
void add_address(int which_operand, int which_address, bit_line *bitLine);
void add_label_bit_line(headSymbol *head_symbol, bit_line *bitLine, char *label_name, int *ext_flag);
void add_register_bit_line(bit_line *bitLine, int regiNum, int which_param);
void add_number_bit_line(bit_line *bitLine, int num);
void print_obj_file(FILE *obj_file, essentials *assem_param, bit_line *tmpLine);
void converts_bit_line(bit_line *bitLine, char bit_line_string[14]);

#endif /* STRUCTURE_H */