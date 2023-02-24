#include "manage_bit_line.h"

void add_classify(bit_line *bitLine, int which_classify){

    int i = 0;

    switch (which_classify) {

        case 1:
            bitLine->param |= (1 << i);
            break;

        case 2:
            bitLine->param |= (1 << (i+1));
            break;

        default:
            break;
    }
}


void add_opcode(int instNum, bit_line *bitLine){

    int i = 6; /* the opcode bit are 6 - 9 in extras */

    switch (instNum) {

        case 1: /* cmp */
            bitLine->param |= (1 << i);
            break;

        case 2: /* add */
            bitLine->param |= (1 << (i+1));
            break;

        case 3: /* sub */
            bitLine->param |= (1 << (i+1)) + (1 << i);
            break;

        case 4: /* not */
            bitLine->param |= (1 << (i+2));
            break;

        case 5: /* clr */
            bitLine->param |= (1 << (i+2)) + (1 << i);
            break;

        case 6: /* lea */
            bitLine->param |= (1 << (i+2)) + (1 << (i+1));
            break;

        case 7: /* inc */
            bitLine->param |= (1 << (i+2)) + (1 << (i+1)) + (1 << i);
            break;

        case 8: /* dec */
            bitLine->param |= (1 << (i+3));
            break;

        case 9: /* jmp */
            bitLine->param |= (1 << (i+3)) + (1 << i);
            break;

        case 10: /* bne */
            bitLine->param |= (1 << (i+3)) + (1 << (i+1));
            break;

        case 11: /* red */
            bitLine->param |= (1 << (i+3)) + (1 << (i+1)) + (1 << i);
            break;

        case 12: /* prn */
            bitLine->param |= (1 << (i+3)) + (1 << (i+2));
            break;

        case 13: /* jsr */
            bitLine->param |= (1 << (i+3)) + (1 << (i+2)) + (1 << i);
            break;

        case 14: /* rts */
            bitLine->param |= (1 << (i+3)) + (1 << (i+2)) + (1 << (i+1));
            break;

        case 15: /* stop */
            bitLine->param |= (1 << (i+3)) + (1 << (i+2)) + (1 << (i+1)) + (1 << i);
            break;

        default:
            break;
    }
}

/* relevant in the case of a jump address *
 * change the relevant bit (8 - 11) depending on the parameters */
void add_param(int param, int which_param, bit_line *bitLine){

    int i = 0;

    if(which_param == 1) /* param 1 */
        i = 12;
    else /* param 2 */
        i = 10;

    switch (param) {

        case 1: /* it is a label name as a parameter */
            bitLine->param |= (1 << i);
            break;

        case 2: /* it is a register as a parameter */
            bitLine->param |= (1 << (i+1)) + (1 << i);
            break;

        default:
            break;
    }
}

/* change the relevant bit (0 - 3) depending on the parameters */
void add_address(int which_operand, int which_address, bit_line *bitLine){

    int i = 0;

    if(which_operand == 1) /* source operand */
        i = 4;
    else /* destination operand */
        i = 2;

    switch (which_address) {

        case 1: /* direct address */
            bitLine->param |= (1 << i);
            break;

        case 2: /* jump address */
            bitLine->param |= (1 << (i+1));
            break;

        case 3: /* direct register address */
            bitLine->param |= (1 << i) + (1 << (i+1));
            break;

        default:
            break;

    }
}


void add_label_bit_line(headSymbol *head_symbol, bit_line *bitLine, char *label_name, int *ext_flag){

    symbolTable *tmp = head_symbol->head;
    int which_classify = 2; /* R - relocatable */

    while (tmp != NULL){

        if(!strcmp(tmp->symName, label_name)){

            if(!strcmp(tmp->sign, "ext")){

                which_classify = 1; /* E - external */
                *ext_flag = 1;
            }

            break;
        }

        tmp = tmp->next;
    }

    add_classify(bitLine, which_classify);
    add_number_bit_line(bitLine, tmp->value);
}


void add_register_bit_line(bit_line *bitLine, int regiNum, int which_param){

    int i = 0 ;

    if (which_param == 1)
        i = 8;
    else
        i = 2;

    switch (regiNum) {

        case 1:
            bitLine->param |= (1 << i);
            break;

        case 2:
            bitLine->param |= (1 << (i+1));
            break;

        case 3:
            bitLine->param |= (1 << (i+1)) + (1 << i);
            break;

        case 4:
            bitLine->param |= (1 << (i+2));
            break;

        case 5:
            bitLine->param |= (1 << (i+2)) + (1 << i);
            break;

        case 6:
            bitLine->param |= (1 << (i+2)) + (1 << (i+1));
            break;

        case 7:
            bitLine->param |= (1 << (i+2)) + (1 << (i+1)) + (1 << i);
            break;

        default:
            break;
    }
}


void add_number_bit_line(bit_line *bitLine, int num){

    int i = 2;

    for(i = 2; i < 14; i++){

        if(num & (1<< (i-2)))
            bitLine->param |= (1 << i);
    }
}


void print_obj_file(FILE *obj_file, essentials *assem_param, bit_line *tmpLine){

    char bit_line_string[14] = {'\0'};
    converts_bit_line(tmpLine, bit_line_string);
    fprintf(obj_file, "\t%d\t\t%s\n", assem_param->IC, bit_line_string);
}


/* converts the string of bits into a unique code */
void converts_bit_line(bit_line *bitLine, char bit_line_string[14]){

    int i = 0, j = 13;

    for(i = 0 ; i < 14 ; i++){

        if(bitLine->param & (1 << i))
            bit_line_string[j] = '/';
        else
            bit_line_string[j] = '*';

        j--;
    }
}

