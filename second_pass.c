#include "second_pass.h"

/* in this pass we check the label's , entry and extern, and write this on the correct file *
 * we build the final table for the obj file *
 * */
void second_pass(essentials *assem_param, headSymbol *head_symbol, headData *head_data, char *nameOfAsFile, char *nameOfExtFile, char *nameOfEntFile, char *nameOfObjFile, int *error, int *ext_flag, int *ent_flag){

    FILE *as_file = NULL;
    FILE *ext_file = NULL;
    FILE *ent_file = NULL;
    FILE *obj_file = NULL;
    char line[LINE_LENGTH] ={'\0'};
    char *ptrLine = NULL;
    char *op1[OP1_LENGTH] = {"not", "clr", "inc", "dec", "jmp", "bne", "red", "prn", "jsr"};
    char *op2[OP2_LENGTH] = {"mov", "cmp", "add", "sub", "lea"};
    int lineCounter = 0;
    assem_param->IC = 100;

    if(!(as_file = checkFileOpening(as_file, nameOfAsFile, "r+", error)))
        return;

    if(ext_flag) {

        if (!(ext_file = checkFileOpening(ext_file, nameOfExtFile, "w+", error)))
            return;
    }

    if(ent_flag) {

        if (!(ent_file = checkFileOpening(ent_file, nameOfEntFile, "w+", error)))
            return;
    }

    if(!(obj_file = checkFileOpening(obj_file, nameOfObjFile, "w+", error)))
        return;

    while(fgets(line, LINE_LENGTH, as_file) != NULL) { /*check line after line in the as file*/

        char label_name[SYMBOL_LENGTH] = {'\0'};
        int label_flag = 0;
        int op1_flag = 0;
        int op2_flag = 0;
        int i = 0;
        ptrLine = line;
        lineCounter++;

        skipSpacesAndTabs(ptrLine); /* we want to point on the first char in the line text */

        /* if it is an empty line we skip */
        if(*ptrLine == '\n' || *ptrLine == '\0')
            continue;

        /* if it is a comment line we skip */
        if(*ptrLine == ';') \
            continue;

        label_flag = is_label(ptrLine);

        if(label_flag) {

            save_label_name(ptrLine,label_name);
            ptrLine += strlen(label_name) + 1; /* pass the label name and the character ':' */
            skipSpacesAndTabs(ptrLine);
        }

        if (*ptrLine == '.') { /* the start of the data, string, extern, entry directive command */

            /*if there is an .entry directive command we will check if its valid and add the entries lines to the .ent file */
            if (check_word(ptrLine,".entry")) {

                ptrLine += 6;
                skipSpacesAndTabs(ptrLine);
                save_label_name(ptrLine, label_name);

                if(exists_label(head_symbol, label_name, 1, error, lineCounter)) /* check if the label name exists in the symbol table */
                    put_in_ent_file(ent_file, head_symbol, label_name, lineCounter, error);

                continue;
            }

            if (check_word(ptrLine,".extern"))
                continue;


            /* we already have .data and .string in the data linked list so we copy them to the obj file */
            if(check_word(ptrLine, ".data") || check_word(ptrLine, ".string")){

                copy_data_line(obj_file, assem_param, head_data, assem_param->IC);
                continue;
            }
        }

        /* instruction command with zero operand: "rts", "stop" */
        if(check_word(ptrLine, "rts")){

            bit_line *tmpLine = create_bitLine();
            add_opcode(14, tmpLine);
            print_obj_file(obj_file, assem_param, tmpLine);
            assem_param->IC += 1;
            continue;
        }

        if(check_word(ptrLine, "stop")){

            bit_line *tmpLine = create_bitLine();
            add_opcode(15, tmpLine);
            print_obj_file(obj_file, assem_param, tmpLine);
            assem_param->IC += 1;
            continue;
        }

        /* instruction command with 1 operand: "not", "clr", "inc", "dec", "red", "prn"
         * and "jmp", "bne", "jsr" that can be jump address */
        for(i = 0 ; i < OP1_LENGTH ; i++){

            if(check_word(ptrLine, op1[i])){

                ptrLine += strlen(op1[i]);
                read_op1(obj_file, ext_file, assem_param, head_symbol, ptrLine, i, error, lineCounter);
                op1_flag = 1;
                break;
            }
        }

        if(op1_flag)
            continue;


        /* instruction command with 2 operand: "mov", "cmp", "add", "sub", "lea" */
        for(i = 0 ; i < OP2_LENGTH ; i++){

            if(check_word(ptrLine, op2[i])){

                ptrLine += strlen(op2[i]);
                read_op2(obj_file, ext_file, assem_param, head_symbol, ptrLine, i, error, lineCounter);
                op2_flag = 1;
                break;
            }
        }

        if(op2_flag)
            continue;
    }

    if(assem_param->IC - 100 > DATA_LENGTH){

        exceededDataAmount(error);
    }

    fclose(as_file);
    fclose(obj_file);

    if(ext_flag)
        fclose(ext_file);

    if(ent_flag)
        fclose(ent_file);
}


/* check if the label name exists in the symbol table */
int exists_label(headSymbol *symbol_head, char *label_name, int type, int *error, int lineCounter){

    symbolTable *tmp = symbol_head->head;
    int lenLabel = strlen(label_name);

    while(tmp) {

        if(strlen(tmp->symName) == lenLabel) {

            if (!strcmp(label_name, tmp->symName)) {

                if (type && !strcmp(tmp->sign, "ext")) { /* .extern label statement in .entry statement */

                    labelCannotBeEnt(error, lineCounter);
                    return 0;
                }
                return 1;
            }
        }

        tmp = tmp->next;
    }

    noLabelStatement(error, lineCounter); /* the label name not exists in the symbol table */
    return 0;
}

/* we copy the name of the label and its value to the entry file */
void put_in_ent_file(FILE *entfile, headSymbol *symbol_head, char *label_name, int lineCounter, int *error) {

    symbolTable *tmp = symbol_head->head;
    char copyLine[LINE_LENGTH] = {'\0'};
    int lenLabel = strlen(label_name);

    while (tmp) {

        if(strlen(tmp->symName) == lenLabel) {

            if (!strcmp(label_name, tmp->symName)) {

                sprintf(copyLine, "%s\t\t%d\n", label_name, tmp->value);
                fputs(copyLine, entfile);
                break;
            }
        }

        tmp = tmp->next;
    }
}

/* we copy the name of the label and its value to the external file */
void put_in_ext_file(FILE *extfile, char *label_name, int IC){

    char line[LINE_LENGTH] = {'\0'};

    sprintf(line, "%s\t\t%d\n", label_name, IC);
    fputs(line, extfile);

}

/* adds the relevant data line as a bit line to the object file */
void copy_data_line(FILE *obj_file, essentials *assem_param, headData *head_data, int current_IC){

    data_table *tmp = head_data->head;

    while(tmp != NULL && tmp->first_IC == current_IC){

        print_obj_file(obj_file,assem_param, tmp->data);
        assem_param->IC += 1;
        head_data->head = head_data->head->next;
        free(tmp->data);
        free(tmp);
        tmp = head_data->head;
    }
}

/* save the label name from the file */
void save_label_name(char *line, char *label_name){

    int i = 0;

    for(i = 0 ; line[i] != '\n' && line[i] != '\0' && line[i] != ' ' && line[i] != '\t' && i < SYMBOL_LENGTH ; i++){

        if(line[i] == ':' || line[i] == '(' || line[i] == ')' || line[i] == ',' )
            break;

        label_name[i] = line[i];
    }

    if(i < SYMBOL_LENGTH)
        label_name[i] = '\0';
}

/* reads who is the only operand and check if the label name exist in the symbol table *
 * if it is a jump address check the first label name and the two operands in the "( )" *
 * adds the special bit line to the object file */
void read_op1(FILE *obj_file, FILE *ext_file, essentials *assem_param, headSymbol *head_symbol, char *ptrLine, int ind_instruction, int *error, int lineCounter){

    enum{not = 0, clr, inc, dec, jmp, bne, red, prn, jsr};
    enum{IMMEDIATE = 0, DIRECT, JUMP, DIR_REGI}; /* address */
    enum{SOURCE = 1, DEST}; /* which operand */
    enum{LABEL = 1, REGISTER}; /* which parameter */
    enum {E = 1, R}; /* which classify */

    int num = 0;
    int ext_flag = 0; /* equal to 1 if we reach to external file */
    int jump_address_flag = 0; /* equal to 1 if we reach '(' after label name */
    int passOperand = 0;
    int regi = 0; /* equal to 1 if the first parameter in the jump address is register */
    char label_name[SYMBOL_LENGTH] = {'\0'};
    bit_line *tmpLine = create_bitLine();

    switch (ind_instruction) {

        case not:
            add_opcode(4, tmpLine);
            break;

        case clr:
            add_opcode(5, tmpLine);
            break;

        case inc:
            add_opcode(7, tmpLine);
            break;

        case dec:
            add_opcode(8, tmpLine);
            break;

        case jmp:
            add_opcode(9, tmpLine);
            break;

        case bne:
            add_opcode(10, tmpLine);
            break;

        case red:
            add_opcode(11, tmpLine);
            break;

        case prn:
            add_opcode(12, tmpLine);
            break;

        case jsr:
            add_opcode(13, tmpLine);
            break;

        default:
            break;
    }

    skipSpacesAndTabs(ptrLine);

    if(*ptrLine == 'r'){

        if(check_register(ptrLine[1]) != -1) {

            num = check_register(ptrLine[1]);
            add_address(DEST, DIR_REGI, tmpLine);
            print_obj_file(obj_file, assem_param, tmpLine);
            assem_param->IC += 1;
            initialize_bit_line(tmpLine);
            add_register_bit_line(tmpLine, num, 1);
            print_obj_file(obj_file, assem_param, tmpLine);
            assem_param->IC += 1;
            free(tmpLine);
            return;
        }
    }

    else if(*ptrLine == '#'){

        ptrLine += 1; /* point on the number */
        num = atoi(ptrLine);
        print_obj_file(obj_file, assem_param, tmpLine);
        assem_param->IC += 1;
        initialize_bit_line(tmpLine);
        add_number_bit_line(tmpLine, num);
        print_obj_file(obj_file, assem_param, tmpLine);
        assem_param->IC += 1;
        free(tmpLine);
        return;
    }

    else {

        save_label_name(ptrLine, label_name);
        ptrLine += strlen(label_name);

        if(*ptrLine == '(')
            jump_address_flag = 1;

        if (!exists_label(head_symbol, label_name, 0, error, lineCounter)) {

            free(tmpLine);
            return;
        }

        if (jump_address_flag) {

            ptrLine++;
            bit_line *secondLine = create_bitLine();
            bit_line *thirdLine = create_bitLine();

            add_address(DEST, JUMP, tmpLine);
            add_label_bit_line(head_symbol, secondLine, label_name, &ext_flag);

            if(ext_flag) {

                put_in_ext_file(ext_file, label_name, assem_param->IC + 1); /* the label is external, copy to ext file */
                ext_flag = 0;
            }

            while (*ptrLine != '\n' && *ptrLine != '\0') {

                if (*ptrLine == 'r') {

                    if (check_register(ptrLine[1]) != -1) {

                        num = check_register(ptrLine[1]);

                        if (regi || passOperand) { /* we already pass one of the operands */

                            add_param(REGISTER, 2, tmpLine);
                            print_obj_file(obj_file, assem_param, tmpLine);
                            assem_param->IC += 1;
                            print_obj_file(obj_file, assem_param, secondLine);
                            assem_param->IC += 1;

                            if (passOperand) { /* the operand is number or label name */

                                print_obj_file(obj_file, assem_param, thirdLine);
                                assem_param->IC += 1;
                                initialize_bit_line(thirdLine); /* initialize the bit line to 0 */
                            }

                            add_register_bit_line(thirdLine, num, 2);
                            print_obj_file(obj_file, assem_param, thirdLine);
                            assem_param->IC += 1;
                            free(tmpLine);
                            free(secondLine);
                            free(thirdLine);
                            return;
                        }

                        add_param(REGISTER, 1, tmpLine);
                        add_register_bit_line(thirdLine, num, 1);
                        regi = 1;
                        skipChars(ptrLine); /* skip the register */
                        skipSpacesAndTabs(ptrLine);
                        ptrLine += 1; /* skip the comma */
                        skipSpacesAndTabs(ptrLine);
                    }
                }

                else if (*ptrLine == '#') {

                    ptrLine += 1;
                    num = atoi(ptrLine); /* point on the number */

                    if (regi || passOperand) { /* we already pass one of the operands */

                        print_obj_file(obj_file, assem_param, tmpLine);
                        assem_param->IC += 1;
                        print_obj_file(obj_file, assem_param, secondLine);
                        assem_param->IC += 1;
                        print_obj_file(obj_file, assem_param, thirdLine);
                        assem_param->IC += 1;
                        initialize_bit_line(thirdLine);
                        add_number_bit_line(thirdLine, num);
                        print_obj_file(obj_file, assem_param, thirdLine);
                        assem_param->IC += 1;
                        free(tmpLine);
                        free(secondLine);
                        free(thirdLine);
                        return;
                    }

                    add_number_bit_line(thirdLine, num);
                    passOperand = 1;
                    skipChars(ptrLine); /* skip the number */
                    skipSpacesAndTabs(ptrLine);
                    ptrLine += 1; /* skip the comma */
                    skipSpacesAndTabs(ptrLine);
                }

                else {

                    save_label_name(ptrLine, label_name);

                    if (!exists_label(head_symbol, label_name, 0, error, lineCounter)) {

                        free(tmpLine);
                        free(secondLine);
                        free(thirdLine);
                        return;
                    }

                    if (regi || passOperand) { /* we already pass one of the operands */

                        add_param(LABEL, 2, tmpLine);
                        print_obj_file(obj_file, assem_param, tmpLine);
                        assem_param->IC += 1;
                        print_obj_file(obj_file, assem_param, secondLine);
                        assem_param->IC += 1;
                        print_obj_file(obj_file, assem_param, thirdLine);
                        assem_param->IC += 1;
                        initialize_bit_line(thirdLine);
                        add_label_bit_line(head_symbol, thirdLine, label_name, &ext_flag);

                        if(ext_flag)
                            put_in_ext_file(ext_file, label_name, assem_param->IC); /* the label is external, copy to ext file */

                        print_obj_file(obj_file, assem_param, thirdLine);
                        assem_param->IC += 1;
                        free(tmpLine);
                        free(secondLine);
                        free(thirdLine);
                        return;
                    }

                    add_param(LABEL, 1, tmpLine);
                    add_label_bit_line(head_symbol, thirdLine, label_name, &ext_flag);

                    if(ext_flag) {

                        put_in_ext_file(ext_file, label_name, assem_param->IC + 2);
                        ext_flag = 0;
                    }

                    passOperand = 1;
                    ptrLine += strlen(label_name); /* skip the label name */
                    skipSpacesAndTabs(ptrLine);
                    ptrLine += 1; /* skip the comma */
                    skipSpacesAndTabs(ptrLine);
                }
            }
        }

        add_address(DEST, DIRECT, tmpLine);
        print_obj_file(obj_file, assem_param, tmpLine);
        assem_param->IC += 1;
        initialize_bit_line(tmpLine);
        add_label_bit_line(head_symbol, tmpLine, label_name, &ext_flag);

        if(ext_flag) {

            put_in_ext_file(ext_file, label_name, assem_param->IC);
            ext_flag = 0;
        }

        print_obj_file(obj_file, assem_param, tmpLine);
        assem_param->IC += 1;
        free(tmpLine);
    }
}


/* reads the two operands are and check if the label name exist in the symbol table *
 * adds the special bit line to the object file */
void read_op2(FILE *obj_file, FILE *ext_file, essentials *assem_param, headSymbol *head_symbol, char *ptrLine, int ind_instruction, int *error, int lineCounter) {

    enum {
        mov = 0, cmp, add, sub, lea
    };
    enum {
        IMMEDIATE = 0, DIRECT, JUMP, DIR_REGI
    }; /* address */
    enum {
        SOURCE = 1, DEST
    }; /* which operand */
    enum {
        LABEL = 1, REGISTER
    }; /* which parameter */

    int num = 0;
    int ext_flag = 0; /* equal to 1 if we reach to external file */
    int passOperand = 0;
    int regi = 0; /* equal to 1 if the first parameter is register */
    char label_name[SYMBOL_LENGTH] = {'\0'};
    bit_line *firstLine = create_bitLine();
    bit_line *tmpLine = create_bitLine();


    switch (ind_instruction) {

        case mov:
            break;

        case cmp:
            add_opcode(1, firstLine);
            break;

        case add:
            add_opcode(2, firstLine);
            break;

        case sub:
            add_opcode(3, firstLine);
            break;

        case lea:
            add_opcode(6, firstLine);
            break;

        default:
            break;
    }

    skipSpacesAndTabs(ptrLine); /* point on the first operand */

    while (*ptrLine != '\n' && *ptrLine != '\0') {

        if (*ptrLine == 'r') {

            if (check_register(ptrLine[1]) != -1) {

                num = check_register(ptrLine[1]);

                if (regi || passOperand) { /* already pass the first operand */

                    add_address(DEST, DIR_REGI, firstLine);
                    print_obj_file(obj_file, assem_param, firstLine);
                    assem_param->IC += 1; /* for the first parameter */

                    if (passOperand) {

                        print_obj_file(obj_file, assem_param, tmpLine);
                        assem_param->IC += 1;
                        initialize_bit_line(tmpLine);
                    }

                    add_register_bit_line(tmpLine, num, 2);
                    print_obj_file(obj_file, assem_param, tmpLine);
                    assem_param->IC += 1;
                    free(firstLine);
                    free(tmpLine);
                    return;
                }

                add_address(SOURCE, DIR_REGI, firstLine);
                add_register_bit_line(tmpLine, num, 1);
                regi = 1;
                skipChars(ptrLine); /* skip the register */
                skipSpacesAndTabs(ptrLine);
                ptrLine += 1; /* skip the comma */
                skipSpacesAndTabs(ptrLine);
            }
        } else if (*ptrLine == '#') {

            ptrLine += 1; /* point on the number */
            num = atoi(ptrLine);

            if (regi || passOperand) { /* already pass the first operand */

                print_obj_file(obj_file, assem_param, firstLine);
                assem_param->IC += 1;
                print_obj_file(obj_file, assem_param, tmpLine);
                assem_param->IC += 1;
                initialize_bit_line(tmpLine);
                add_number_bit_line(tmpLine, num);
                print_obj_file(obj_file, assem_param, tmpLine);
                assem_param->IC += 1;
                free(tmpLine);
                free(firstLine);
                return;
            }

            add_number_bit_line(tmpLine, num);
            passOperand = 1;
            skipChars(ptrLine); /* skip the number */
            skipSpacesAndTabs(ptrLine);
            ptrLine += 1; /* skip the comma */
            skipSpacesAndTabs(ptrLine);
        } else {

            save_label_name(ptrLine, label_name);

            if (!exists_label(head_symbol, label_name, 0, error, lineCounter)) {

                free(firstLine);
                free(tmpLine);
                return;
            }

            if (regi || passOperand) { /* already pass the first operand */

                add_address(DEST, DIRECT, firstLine);
                print_obj_file(obj_file, assem_param, firstLine);
                assem_param->IC += 1;
                print_obj_file(obj_file, assem_param, tmpLine);
                assem_param->IC += 1;
                initialize_bit_line(tmpLine);
                add_label_bit_line(head_symbol, tmpLine, label_name, &ext_flag);

                if (ext_flag)
                    put_in_ext_file(ext_file, label_name, assem_param->IC);

                print_obj_file(obj_file, assem_param, tmpLine);
                assem_param->IC += 1;
                free(tmpLine);
                free(firstLine);
                return;
            }

            add_address(SOURCE, DIRECT, firstLine);
            add_label_bit_line(head_symbol, tmpLine, label_name, &ext_flag);

            if (ext_flag) {

                put_in_ext_file(ext_file, label_name, assem_param->IC + 1);
                ext_flag = 0;
            }

            passOperand = 1;
            ptrLine += strlen(label_name); /* skip the number */
            skipSpacesAndTabs(ptrLine);
            ptrLine += 1; /* skip the comma */
            skipSpacesAndTabs(ptrLine);
        }
    }
}


