#include "first_pass.h"

/* we add all the label name from the file to the symbol table *
 * we check errors in the ".as" file and return message if we find one */
void first_pass(char *nameOfAsFile, essentials *assem_param, headSymbol *sym, headData *head_data_line, int *error, int *entry_flag, int *external_flag) {

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

        skipSpacesAndTabs(ptrLine); /* we want to point on the first char in the line text */

        /* if it is an empty line we skip */
        if (*ptrLine == '\n' || *ptrLine == '\0')
            continue;

        /* if it is a comment line we skip */
        if (*ptrLine == ';') \

            continue;

        label_flag = is_label(sym, ptrLine, label_name, error, lineCounter, 0); /* if the name is valid we point on it with ptr_label and return 1 */

        if (label_flag) {

            if (validLabelName(sym, label_name, 1, error, lineCounter)) {

                ptrLine += strlen(label_name) + 1; /* point after the label name and the ':' */
                skipSpacesAndTabs(ptrLine);
            }

            else
                continue;
        }

        if (*ptrLine == '.') { /* the start of the data, string, extern, entry directive command */

            if (check_word(ptrLine, ".data") || check_word(ptrLine, ".string")) {

                if (label_flag) {

                    strcpy(label_type, "data"); /* save the label type */
                    add_to_symbolTable(sym, label_name, label_type, assem_param->IC);
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
                skipSpacesAndTabs(ptrLine);

                if (label_flag) { /* if we pass a label name before .extern */

                    irrelevantLabel(error, lineCounter);
                    continue;
                }

                save_label_name(ptrLine, label_name);

                if (validLabelName(sym, label_name, 0, error, lineCounter)) {

                    skipChars(ptrLine);
                    skipSpacesAndTabs(ptrLine);

                    if (*ptrLine != '\n' && *ptrLine != '\0') {

                        extraTextContinue(error, lineCounter);
                        continue;
                    }

                    add_to_symbolTable(sym, label_name, label_type, 0);
                    continue;
                }
            }

            else { /* the word from the text file start with '.' but it is not a directive command */

                undefinedDirCommand(error, lineCounter);
                continue;
            }
        }

        if (label_flag) {

            strcpy(label_type, "code");
            add_to_symbolTable(sym, label_name, label_type, assem_param->IC);
            readingInstruction(assem_param, head_data_line, ptrLine, error, lineCounter);
            continue;
        }

        readingInstruction(assem_param, head_data_line, ptrLine, error, lineCounter);
    }

    fclose(asFile);
}

void printSymbol(headSymbol *head){

    symbolTable *tmp = head->head;
    while(tmp != NULL){

        printf("%s, %s, %d\n", tmp->symName, tmp->sign, tmp->value);
        tmp = tmp->next;
    }

}

/* check if we have reached a declaration of a valid symbol according to the symbol type */
int is_label (headSymbol *sym, char *line, char *labelName, int *error, int lineCounter, int symbolType){

    int i = 0;

    while(line[i] != '\n' && line[i] != '\0' && line[i] != ' ' && line[i] != '\t'){ /* check if there is ':' in label name */

        if(line[i] == ':'){

            save_label_name(line, labelName);
            return 1;
        }

        i++;
    }

    return 0;
}


/* save the label name from the file */
void save_label_name(char *line, char *label_name){

    int i = 0;

    for(i = 0 ; line[i] != '\n' && line[i] != '\0' && i < SYMBOL_LENGTH ; i++){

        label_name[i] = line[i];

        if(line[i] == ':' || line[i] == ')' || line[i] == ',' ){

            label_name[i+1] = '\0';
            return;
        }
    }

    if(label_name[SYMBOL_LENGTH-1] != '\0')
        label_name[0] = ' ';

}


/* check if the label name is not exist in the symbol table, is not equal to familiar names in the program */
int validLabelName(headSymbol *sym, char *labelName, int type, int *error , int lineCounter){

    symbolTable  *tmp = sym->head;
    int len = strlen(labelName);

    if (labelName[0] == ' '){

        longLabelName(error, lineCounter);
        return 0;
    }

    if(len == 0){

        noLabelStatement(error, lineCounter);
        return 0;
    }

    if(labelName[len-1] != ':' && type)
        return 0;

    if(type)
        labelName[len-1] = '\0';
    else
        labelName[len] = '\0';

    if(invalidName(labelName, error, lineCounter)) /* check if the name of the label is register name or instruction or directive command or if not exists label name */
        return 0;

    if (!sym->head)
        return 1;

    tmp = sym->head;

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


/* return true if the command name is the same as the word from the text file */
int check_word(char *line, char *command){

    int i = 0;

    while(line[i] && line[i] != ',' && line[i] != ' ' && line[i] != '\t' && line[i] != '\n')
        i++;

    if(strlen(command) < i)
        return 0;

    if(!strncmp(command, line, i))
        return 1;

    return 0;
}


/* if all the parameter write right in the text file we save them in the data table, else we return error */
void reading_data_param(essentials *assem_param, headData *head_data, char *line, int *error, int lineCounter){

    int num = 0;
    int first_IC = assem_param->IC;
    char *comma = NULL; /* will point on the char after the num */
    errno = 0;

    if (*line != ' ' && *line != '\t') {

        if(*line == '\n' || *line == '\0') { /* check if after the .data command we do not have param */

            noParam(error, lineCounter); /* error message */
            return;
        }
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

            if (*comma == '.') { /* if this is a real number (example: 0.1) */

                invalidDataParam(error, lineCounter); /* error message */
                return;
            }

            else if (*comma != ',') {

                missingComma(error, lineCounter); /* error message */
                return;
            }

            else { /* *comma = ',' */

                comma++;
                skipSpacesAndTabs(comma); /* we want to check if the user enter 2 commas in row */

                if(*comma == '\n' || *comma == '\0'){

                    extraTextContinue(error, lineCounter); /* error message */
                    return;
                }

                if (*comma == ',') {

                    multipleCommas(error, lineCounter); /* error message */
                    return;
                }
            }

            line = comma;
            add_data_line(head_data, num, first_IC);
            assem_param->DC += 1;
            assem_param->IC += 1;
        }
    }
}


void reading_string_param(essentials *assem_param, headData *head_data, char *line, int *error, int lineCounter){

    char letter= '\0' ;
    int i = 0;
    int first_IC = assem_param->IC;
    int lastStrInd = -1; /* save the index of the closing " */
    int noString = 0; /* equal to 1 if there are more characters after " */

    if (*line != ' ' && *line != '\t') {

        if(*line == '\n' || *line == '\0') { /* check if after the .string command we do not have param */

            noParam(error, lineCounter); /* error message */
            return;
        }
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

    while(line[i] != '\n' && line[i] != '\0') { /* the first pass over the string is to check closing of the " */

        if(line[i] == '\"') { /* save the index of the closing " */

            lastStrInd = i;
            noString = 0;
        }

        if(i > lastStrInd && (line[i] != ' ' && line[i] != '\t')) /* if we reached to " but there ara more characters  */
            noString = 1;

        i++;
    }

    if(lastStrInd == -1 || noString){

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
    char *instructions [LEN_INSTRUCTIONS] = {"mov", "cmp", "add", "sub", "lea", "not", "clr", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"};

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

    if(*line != ' ' && *line != '\t' && *line != '\n'){

        noSpaceOrTab(error, lineCounter); /* error message */
        return;
    }

    skipSpacesAndTabs(line);

    if(*line == ','){ /* if there is a comma before the parameters */

        invalidComma(error, lineCounter); /* error message */
        return;
    }

    if(13 < instructInd){ /* there should be not operands */

        if(*line != '\n' && *line != '\0'){

            extraTextContinue(error, lineCounter); /* error message */
            return;
        }
        assem_param->IC++; /* we add one line in the final table */
        return;
    }

    checkInstParam(assem_param, line , error, lineCounter, instructInd);
}

/* check the instruction parameters : number, register (label name we check in the second pass */
void checkInstParam(essentials *assem_param, char *line , int *error, int lineCounter, int instructInd) {

    int jumpAddress = 0; /* equal to 1 when we reach to jump address in the line */
    int regi = 0; /* equal to 1 when we pass register */
    int passParam = 0; /* equal to 1 if we pass a parameter */
    int i = 0;

    if (*line == '\n' || *line == '\0') {

        noParam(error, lineCounter); /* error message */
        return;
    }

    if(instructInd == 5 || 6 < instructInd){

        if (*line == 'r') { /* first parameter in the jump address is register or label name */

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

            if (!check_number(line)) {

                invalidNumberParam(error, lineCounter); /* error message */
                return;
            }

            assem_param->IC += 2;
            skipChars(line);
            skipSpacesAndTabs(line);

            if (*line != '\n' && *line != '\0')
                extraTextContinue(error, lineCounter);

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

            if ((instructInd == 9 || instructInd == 10 || instructInd == 13) && jumpAddress) { /* jmp or bne or jsr */

                line += i; /* point on the first parameter in ( ) */

                while (1) {

                    if (*line == 'r') { /* first parameter in the jump address is register or label name */

                        if (check_register(line[1]) == -2) {

                            invalidRegisterParam(error, lineCounter); /* error message */
                            return;
                        }

                        if (check_register(line[1]) != -1) {/* the parameter is register */

                            if (regi || passParam) {

                                if (regi)
                                    assem_param->IC += 3;
                                if (passParam)
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

                        if (!check_number(line)) {

                            invalidNumberParam(error, lineCounter); /* error message */
                            return;
                        }

                        if (regi || passParam) {

                            assem_param->IC += 4;
                            skipChars(line);
                            skipSpacesAndTabs(line);
                            break;
                        }

                        passParam = 1;
                        skipChars(line);
                        skipSpacesAndTabs(line);
                    }

                    else { /* label name */

                        if (regi || passParam) {

                            assem_param->IC += 4;
                            skipChars(line);
                            skipSpacesAndTabs(line);
                            break;
                        }

                        skipChars(line); /* skip label name */
                        skipSpacesAndTabs(line);
                        passParam = 1;
                    }

                    if (*line != ',' && (passParam || regi)) {

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

                    invalidInstParam(error, lineCounter); /* error message */
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

    else { /* the instruction include 2 param */

        while (1) {

            if (*line == 'r') {

                if (check_register(line[i]) == -2) {

                    invalidRegisterParam(error, lineCounter); /* error message */
                    return;
                }

                if (check_register(line[1]) != -1) {

                    if (regi || passParam) {

                        if (regi)
                            assem_param->IC += 2;

                        if (passParam)
                            assem_param->IC += 3;

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

                if (!check_number(line)) {

                    invalidNumberParam(error, lineCounter); /* error message */
                    return;
                }

                if (regi || passParam) {

                    assem_param->IC += 3;
                    skipChars(line);
                    skipSpacesAndTabs(line);
                    break;
                }

                passParam = 1;
                skipChars(line);
                skipSpacesAndTabs(line);
            }

            else {

                skipChars(line); /* label name */
                skipSpacesAndTabs(line);

                if (regi || passParam) {

                    assem_param->IC += 3;
                    break;
                }

                passParam = 1;
            }

            if (*line != ',' && (passParam || regi)) {

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

/* skip all characters that are not : '\t', ' ', '\0', '\n', ',' */
void skipChars(char *line){

    while(*line && *line != '\n' && *line != '\0' && *line != '\t' && *line != ' '&& *line != ',' && *line != '(' && *line != ')'){

        memmove(line, line+1, strlen(line));
    }
}

/* check if the parameter is valid register *
 * return -1 if the next character is not a number *
 * return -2 if the next character is not a valid number *
 * else return the number of the register*/
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

/* check if the parameter is valid number */
int check_number(char *line){

    int i = 1;

    if(line[i] == '-' || line[i] == '+')
        i++;

    while(line[i] && line[i] != ' ' && line[i] != '\t' && line[i] != '\n' && line[i] != '\0' && line[i] != ','){

        if(isdigit(line[i]) == 0) /* the char not digit */
            return  0;

        i++;
    }

    return 1;
}
