#include "first_pass.h"

/* we add all the label name from the file to the symbol table *
 * we check errors in the ".as" file and return message if we find one */
void first_pass(char *nameOfAsFile, essentials *assem_param, headSymbol *head_symbol, headData *head_data_line, int *error, int *entry_flag, int *external_flag) {

    FILE *asFile = NULL;
    int lineCounter = 0;
    char line[LINE_LENGTH] = {'\0'};

    if(!(asFile = checkFileOpening(asFile, nameOfAsFile, "r+", error)))
        return;

    while(fgets(line, LINE_LENGTH, asFile) != NULL) { /* check line after line in the as file */

        char label_name[SYMBOL_LENGTH] = {'\0'};
        char label_type[MAX_LENGTH_TYPE] = {'\0'}; /* save the type of the symbol ( code / data / external) */
        int label_flag = 0; /* turned on if we find label name in the line */
        char *ptrLine = line;
        lineCounter++;

        skipSpacesAndTabs(ptrLine); /* points to the first char in the line text */

        /* if it is an empty line we skip */
        if (*ptrLine == '\n' || *ptrLine == '\0')
            continue;

        /* if it is a comment line we skip */
        if (*ptrLine == ';')
            continue;

        if(strlen(ptrLine) == LINE_LENGTH-1){ /* check if the line is longer than necessary */

            lineLengthLong(error, lineCounter);

            while(strlen(ptrLine) == LINE_LENGTH-1) { /* we will go through the entire line before moving on to the next line in the file */

                if (fgets(line, LINE_LENGTH, asFile) == NULL) {

                    fclose(asFile);
                    return;
                }
            }

            continue;
        }

        label_flag = is_label(ptrLine);

        if(label_flag == -1){

            spaceInLabelStatement(error, lineCounter);
            continue;
        }

        if (label_flag == 1) {

            if(!save_and_check_label_name(head_symbol, ptrLine, label_name, 1, error , lineCounter))
                continue;

            ptrLine += strlen(label_name) + 1; /* point after the label name and the ':' */

            if(*ptrLine != ' ' && *ptrLine != '\t'){

                noSpaceOrTab(error, lineCounter);
                continue;
            }

            skipSpacesAndTabs(ptrLine);
        }

        if (*ptrLine == '.') { /* the start of the data, string, extern, entry directive command */

            if (check_word(ptrLine, ".data") || check_word(ptrLine, ".string")) {

                if (label_flag) {

                    strcpy(label_type, "data"); /* save the label type */
                    add_to_symbolTable(head_symbol, label_name, label_type, assem_param->IC);
                }

                if (check_word(ptrLine, ".data")) {

                    ptrLine += 5; /* point after the .data directive */
                    reading_data_param(assem_param, head_data_line, ptrLine, error, lineCounter);
                    continue;
                }

                if (check_word(ptrLine, ".string")) {

                    ptrLine += 7; /* point after the .string directive */
                    reading_string_param(assem_param, head_data_line, ptrLine, error, lineCounter);
                    continue;
                }
            }

            if (check_word(ptrLine, ".entry")) {

                *entry_flag = 1;
                ptrLine += 6;

                if (label_flag) { /* if we pass a label name before .entry */

                    irrelevantLabel(error, lineCounter);
                    continue;
                }

                skipSpacesAndTabs(ptrLine);

                if (*ptrLine == '\n' || *ptrLine == '\0') {

                    noLabelStatement(error, lineCounter);
                    continue;
                }

                skipChars(ptrLine); /* skip the label name */
                skipSpacesAndTabs(ptrLine);

                if (*ptrLine != '\n' && *ptrLine != '\0') {

                    extraTextContinue(error, lineCounter);
                    continue;
                }

                continue;
            }

            if (check_word(ptrLine, ".extern")) {

                *external_flag = 1;
                strcpy(label_type, "ext");
                ptrLine += 7;

                if (label_flag) { /* if we pass a label name before .extern */

                    irrelevantLabel(error, lineCounter);
                    continue;
                }

                skipSpacesAndTabs(ptrLine);

                if (*ptrLine == '\n' || *ptrLine == '\0') {

                    noLabelStatement(error, lineCounter);
                    continue;
                }

                if (save_and_check_label_name(head_symbol, ptrLine, label_name, 1, error , lineCounter)){

                    skipChars(ptrLine); /* skip the label name */
                    skipSpacesAndTabs(ptrLine);

                    if (*ptrLine != '\n' && *ptrLine != '\0') {

                        extraTextContinue(error, lineCounter);
                        continue;
                    }

                    add_to_symbolTable(head_symbol, label_name, label_type, 0);
                    continue;
                }
                continue;
            }

            else { /* the word from the text file start with '.' but it is not a directive command */

                undefinedDirCommand(error, lineCounter);
                continue;
            }
        }

        if (label_flag) {

            strcpy(label_type, "code");
            add_to_symbolTable(head_symbol, label_name, label_type, assem_param->IC);
            readingInstruction(assem_param, head_data_line, ptrLine, error, lineCounter);
            continue;
        }

        readingInstruction(assem_param, head_data_line, ptrLine, error, lineCounter);
    }

    fclose(asFile);
}


/* check if we have reached a declaration of a valid symbol according to the symbol type */
int is_label (char *line){

    int i = 0;
    int space_flag = 0;

    for(i = 0 ; line[i] != '\n' && line[i] != '\0' ; i++){ /* check if there is ':' in label name or space or tab */

        if(line[i] == ':'){

            if(space_flag)
                return -1;

            return 1;
        }

         if(line[i] == ' ' || line[i] == '\t') {

             space_flag = 1;
             continue;
         }

        if(!((line[i] >= 'A' && line[i] <= 'Z') || (line[i] >= 'a' && line[i] <= 'z') || (line[i] >= '0' && line[i] <= '9'))) /* Writing the label name is not correct */
            return 0;
    }

    return 0;
}

int save_and_check_label_name(headSymbol *head_symbol, char *line, char *label_name, int type, int *error , int lineCounter){

    int i = 0;
    int len = 0;
    symbolTable  *tmp = head_symbol->head;

    for(i = 0 ; line[i] != '\n' && line[i] != '\0' && line[i] != ' ' && line[i] != '\t' && i < SYMBOL_LENGTH ; i++){

        if(type && line[i] == ':'){

            label_name[i] = '\0';
            break;
        }

        label_name[i] = line[i];

        if(!((line[i] >= 'A' && line[i] <= 'Z') || (line[i] >= 'a' && line[i] <= 'z') || (line[i] >= '0' && line[i] <= '9'))) {

            if(!type && (line[i] == ')' || line[i] == ',')){

                label_name[i+1] = '\0';
                break;
            }

            illegalLabelName(error, lineCounter);
            return 0;
        }

    }

    if(label_name[SYMBOL_LENGTH-1] != '\0'){

        longLabelName(error, lineCounter);
        return 0;
    }

    if(invalidName(label_name)) { /* check if the name of the label is register name or instruction or directive command or if not exists label name */

        illegalLabelName(error, lineCounter);
        return 0;
    }

    len = strlen(label_name);

    if (!tmp)
        return 1;

    while(tmp != NULL) { /* check all the names in the table of label name  */

        if(len == strlen(tmp->symName)) {

            if (!strcmp(label_name, tmp->symName)) { /* check if the new label name is in the table */

                doubleLabelName(error, lineCounter); /* error message */
                return 0;
            }
        }

        tmp = tmp->next;
    }

    return 1;
}


/* check if the label name is not exist in the symbol table, is not equal to familiar names in the program */
int validLabelName(headSymbol *sym, char *labelName, int type, int *error , int lineCounter){

    symbolTable  *tmp = sym->head;
    int len = strlen(labelName);
    int i = 0;

    if (labelName[0] == ' '){

        longLabelName(error, lineCounter);
        return 0;
    }

    if(type) {/* equal to 1 if the label end with ':' */

        labelName[len - 1] = '\0';
        len--;
    }

    for (i = 0; i < len ; i++) {

        if(labelName[i] == ' ' || labelName[i] == '\t'){

            extraTextContinue(error, lineCounter);
            return 0;
        }

        if(!((labelName[i] >= 'A' && labelName[i] <= 'Z') || (labelName[i] >= 'a' && labelName[i] <= 'z') || (labelName[i] >= '0' && labelName[i] <= '9'))) {

            illegalLabelName(error, lineCounter);
            return 0;
        }
    }

    if(invalidName(labelName)) { /* check if the name of the label is register name or instruction or directive command or if not exists label name */

        illegalLabelName(error, lineCounter);
        return 0;
    }

    if (!tmp)
        return 1;

    while(tmp != NULL) { /* check all the names in the table of label name  */

        if(len == strlen(tmp->symName)) {

            if (!strcmp(labelName, tmp->symName)) { /* check if the new label name is in the table */

                doubleLabelName(error, lineCounter); /* error message */
                return 0;
            }
        }

        tmp = tmp->next;
    }

    return 1;
}


/* if all the parameter are numbers in the text line, save them in the data table, else we return error */
void reading_data_param(essentials *assem_param, headData *head_data, char *line, int *error, int lineCounter){

    int num = 0;
    int first_IC = assem_param->IC;
    char *comma = NULL; /* will point on the char after the num */
    errno = 0;

    if (*line != ' ' && *line != '\t') {

        noSpaceOrTab(error, lineCounter); /* error message */
        return;
    }

    skipSpacesAndTabs(line); /* now we point on the parameter or end of line */

    if(*line == '\n' || *line == '\0') {

        noParam(error, lineCounter); /* error message */
        return;
    }

    if (*line == ',') {

        invalidComma(error, lineCounter); /* error message */
        return;
    }

    while(*line != '\n' && *line != '\0') {

        num = strtol(line, &comma, 10); /* we want to check the number and the comma */

        if (line == comma || (errno != 0 && num == 0)) { /* the parameters are not numbers*/

            invalidDataParam(error, lineCounter); /* error message */
            return;
        }

        else {

            skipSpacesAndTabs(comma);

            if(*comma == '\n' || *comma == '\0'){ /* we reach to end of line */

                add_data_line(head_data, num, first_IC);
                assem_param->DC += 1;
                assem_param->IC += 1;
                return;
            }

            if (*comma == '.') { /* this is a real number (example: 0.1) */

                invalidDataParam(error, lineCounter); /* error message */
                return;
            }

            if (*comma != ',') {

                missingComma(error, lineCounter); /* error message */
                return;
            }

            else { /* *comma = ',' */

                comma++;
                skipSpacesAndTabs(comma);

                if(*comma == '\n' || *comma == '\0'){ /* last char was a comma */

                    extraTextContinue(error, lineCounter); /* error message */
                    return;
                }

                if (*comma == ',') { /* two commas in a row */

                    multipleCommas(error, lineCounter); /* error message */
                    return;
                }
            }

            line = comma; /* continue to the next number */
            add_data_line(head_data, num, first_IC);
            assem_param->DC += 1;
            assem_param->IC += 1;
        }
    }
}

/* if all the characters are valid in the text line, save them in the data table, else we return error */
void reading_string_param(essentials *assem_param, headData *head_data, char *line, int *error, int lineCounter){

    char letter= '\0' ;
    int i = 0;
    int first_IC = assem_param->IC;
    int lastStrInd = -1; /* save the index of the closing " */
    int noString = 0; /* equal to 1 if there are more characters after " */

    if (*line != ' ' && *line != '\t') {

        noSpaceOrTab(error, lineCounter); /* error message */
        return;
    }

    skipSpacesAndTabs(line); /* now we point on the parameter or end of line */

    if(*line == '\n' || *line == '\0') {

        noParam(error, lineCounter); /* error message */
        return;
    }

    if (*line != '\"'){ /* the string should start with " */

        invalidString(error, lineCounter); /* error message */
        return;
    }

    line++; /* point on the first char in the string */

    for( i = 0; line[i] != '\n' && line[i] != '\0'; i++) { /* the first pass over the string is to check closing of the " */

        if(line[i] == '\"') { /* save the index of the closing " */

            lastStrInd = i;
            noString = 0;
        }

        if(i > lastStrInd && (line[i] != ' ' && line[i] != '\t')) /* if we reached to " but there are more characters after */
            noString = 1;

    }

    if(lastStrInd == -1 || noString){ /* we didn't pass the closing " or there are more characters after it */

        invalidString(error, lineCounter); /* error message */
        return;
    }

    i = 0;

    while(i < lastStrInd) { /* We save the data line of the string */

        letter = *line;
        add_data_line(head_data, (int) letter, first_IC);
        assem_param->DC += 1;
        assem_param->IC += 1;
        line++;
        i++;
    }

    add_data_line(head_data, 0, first_IC); /* add '\0' to the end of the string */
    assem_param->DC += 1;
    assem_param->IC += 1;
}

/* find if the instruction in the file valid */
void readingInstruction(essentials *assem_param, headData *head_data, char *line , int *error, int lineCounter){

    int i = 0;
    int instructInd = -1;
    /* name of instructors */
    char *instructions [LEN_INSTRUCTIONS] = {"mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"};

    for (i = 0; i < LEN_INSTRUCTIONS; i++) {

        if(check_word(line, instructions[i])){ /* if we find the instruction name */

            instructInd = i; /* save the index of the instruction name */
            break;
        }
    }

    if(instructInd == -1){ /* we do not reach to valid instruction name */

        undefinedInstructCommand(error, lineCounter); /* error message */
        return;
    }

    line += strlen(instructions[i]); /* pass the instruction command */

    if(13 < instructInd){ /* there should be not operands */

        if(*line == ' ' || *line == '\t')
            skipSpacesAndTabs(line);

        if(*line != '\n' && *line != '\0'){

            extraTextContinue(error, lineCounter); /* error message */
            return;
        }

        assem_param->IC++; /* we add one line in the final table */
        return;
    }

    else {

        if (*line != ' ' && *line != '\t') { /* must be a space or tab */

            noSpaceOrTab(error, lineCounter); /* error message */
            return;
        }

        skipSpacesAndTabs(line);

        if (*line == '\n' || *line == '\0') {

            noParam(error, lineCounter); /* error message */
            return;
        }

        if (*line == ',') { /* if there is a comma before the parameters */

            invalidComma(error, lineCounter); /* error message */
            return;
        }

        checkInstOperand(assem_param, line, error, lineCounter, instructInd);
    }
}

/* check the instruction parameters : number, register (label name we check in the second pass */
void checkInstOperand(essentials *assem_param, char *line , int *error, int lineCounter, int instructInd) {

    int jumpAddress = 0; /* equal to 1 when we reach to jump address in the line */
    int regi = 0; /* equal to 1 when we pass register */
    int passOperand = 0; /* equal to 1 if we pass a operand */
    int i = 0;

    /* the instruction can be : "not", "clr", "inc", "dec", "jmp", "bne", "red", "prn", "jsr" *
     * they can be with one operand or with a jump address */
    if(3 < instructInd && instructInd != 6){

        if (*line == 'r') { /* check if the first parameter is a register or label name the start with 'r' */

            if (check_register(line[1]) == -2) {

                invalidRegisterParam(error, lineCounter); /* error message */
                return;
            }

            if (check_register(line[1]) != -1) {/* the parameter is register */

                assem_param->IC += 2;
                skipChars(line);
                skipSpacesAndTabs(line);

                if (*line != '\n' && *line != '\0')
                    extraTextContinue(error, lineCounter);

                return;
            }
        }

        else if (*line == '#') { /* the parameter is a number */

            line++;

            if (!check_number(line)) {

                invalidNumberParam(error, lineCounter); /* error message */
                return;
            }

            if(instructInd == 12) { /* number can be a destination operand only in "prn" command */

                assem_param->IC += 2;
                skipChars(line);
                skipSpacesAndTabs(line);

                if (*line != '\n' && *line != '\0')
                    extraTextContinue(error, lineCounter);

                return;
            }

            IllegalInstParam(error, lineCounter);
            return;
        }

        else { /* the parameter is label name or beginning of jump address */

            while (line[i] != ' ' && line[i] != '\t' && line[i] != '\n' && line[i] != '\0') { /* check if the line is a jump address */

                if (line[i] == '(') {

                    jumpAddress = 1;
                    i++;
                    break;
                }
                i++;
            }

            if ((instructInd == 9 || instructInd == 10 || instructInd == 13) && jumpAddress) { /* "jmp" or "bne" or "jsr" */

                line += i; /* point on the first parameter in ( ) */

                while (1) {

                    if (*line == 'r') { /* first parameter in the jump address is register or label name */

                        if (check_register(line[1]) == -2) {

                            invalidRegisterParam(error, lineCounter); /* error message */
                            return;
                        }

                        if (check_register(line[1]) != -1) {/* the parameter is register */

                            if (regi || passOperand) { /* we pass register or number or label name */

                                if (regi)
                                    assem_param->IC += 3;

                                if (passOperand)
                                    assem_param->IC += 4;

                                skipChars(line);
                                skipSpacesAndTabs(line);
                                break;
                            }

                            regi = 1;
                            skipChars(line);
                            skipSpacesAndTabs(line);
                        }
                    }

                    else if (*line == '#') { /* the parameter is a number */

                        line++;

                        if (!check_number(line)) {

                            invalidNumberParam(error, lineCounter); /* error message */
                            return;
                        }

                        if (regi || passOperand) {

                            assem_param->IC += 4;
                            skipChars(line);
                            skipSpacesAndTabs(line);
                            break;
                        }

                        passOperand = 1;
                        skipChars(line);
                        skipSpacesAndTabs(line);
                    }

                    else { /* label name */

                        if (regi || passOperand) {

                            assem_param->IC += 4;
                            skipChars(line);
                            skipSpacesAndTabs(line);
                            break;
                        }

                        skipChars(line); /* skip label name */
                        skipSpacesAndTabs(line);
                        passOperand = 1;
                    }

                    if (*line != ',' && (passOperand || regi)) { /* there is no comma after the first operand */

                        missingComma(error, lineCounter); /* error message */
                        return;
                    }

                    line++;
                    skipSpacesAndTabs(line);

                    if (*line == ',') {

                        multipleCommas(error, lineCounter); /* error message */
                        return;
                    }
                }

                if (*line != ')') {

                    extraTextContinue(error, lineCounter);
                    return;
                }

                line++;
                skipSpacesAndTabs(line);

                if (*line != '\n' && *line != '\0')
                    extraTextContinue(error, lineCounter);

                return;
            }

            else {

                if (jumpAddress) { /* if we have '(' in the label name */

                    illegalLabelName(error, lineCounter); /* error message */
                    return;
                }

                assem_param->IC += 2;
                skipChars(line);
                skipSpacesAndTabs(line);

                if (*line != '\n' && *line != '\0')
                    extraTextContinue(error, lineCounter);

                return;
            }
        }
    }

    /* the instruction can be : "mov", "cmp", "add", "sub", "lea" *
     * should have two operands */
    else {

        while (1) {

            if (*line == 'r') {

                if (check_register(line[1]) == -2) {

                    invalidRegisterParam(error, lineCounter); /* error message */
                    return;
                }

                if (check_register(line[1]) != -1) {

                    if (regi || passOperand) {

                        if (regi)
                            assem_param->IC += 2;

                        if (passOperand)
                            assem_param->IC += 3;

                        skipChars(line);
                        skipSpacesAndTabs(line);
                        break;
                    }

                    if(instructInd == 6){ /* register cant be source operand in "lea" command */

                        IllegalInstParam(error, lineCounter);
                        return;
                    }

                    regi = 1;
                    skipChars(line);
                    skipSpacesAndTabs(line);
                }
            }

            else if (*line == '#') { /* the parameter is a number */

                line++;

                if (!check_number(line)) {

                    invalidNumberParam(error, lineCounter); /* error message */
                    return;
                }

                if (regi || passOperand) {

                    if(instructInd != 1){ /* number can be a destination operand only in "cmp" command */

                        IllegalInstParam(error, lineCounter);
                        return;
                    }

                    assem_param->IC += 3;
                    skipChars(line);
                    skipSpacesAndTabs(line);
                    break;
                }

                if(instructInd == 6){ /* number cant be source operand in "lea" command */

                    IllegalInstParam(error, lineCounter);
                    return;
                }

                passOperand = 1;
                skipChars(line);
                skipSpacesAndTabs(line);
            }

            else { /* label name */

                skipChars(line);
                skipSpacesAndTabs(line);

                if (regi || passOperand) {

                    assem_param->IC += 3;
                    break;
                }

                passOperand = 1;
            }

            if (*line != ',' && (passOperand || regi)) {

                missingComma(error, lineCounter);
                return;
            }

            line++;
            skipSpacesAndTabs(line);

            if (*line == ',') {

                multipleCommas(error, lineCounter);
                return;
            }
        }

        if (*line != '\n' && *line != '\0')
            extraTextContinue(error, lineCounter);

        return;
    }
}

/* skips all characters that are not : '\t', ' ', '\0', '\n', ',', '(', ')' */
void skipChars(char *line){

    while(*line && *line != '\n' && *line != '\0' && *line != '\t' && *line != ' '&& *line != ',' && *line != '(' && *line != ')')
        memmove(line, line+1, strlen(line));

}

/* check if the operand is valid register */
int check_register(char num){

    if(isdigit(num) == 0) /* maybe it is a label name */
        return -1;

    else{

        int registerNum = num - '0';

        if(0 <= registerNum && registerNum <= 7)
            return registerNum;

        return -2;
    }
}

/* check if the operand is valid number */
int check_number(char *line){

    int i = 0;

    if(line[i] == '-' || line[i] == '+')
        i++;

    while(line[i] && line[i] != ' ' && line[i] != '\t' && line[i] != '\n' && line[i] != '\0' && line[i] != ','){

        if(isdigit(line[i]) == 0) /* the char not digit */
            return  0;

        i++;
    }

    return 1;
}
