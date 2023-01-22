#include "constant.h"
#include "error_messages.h"
#include "structure.h"

char * places_macros(char * asFile);
char * changeToAm (char * fileAs);
int checkWord (char * line, char word[LINE_LENGTH]);
void skipSpacesAndTabs(char *p);
void removeAllSpaces(char *p);
int unValidName(char * name);
void put_in_mcrTable(headMacro *headMcr, char *name, int posFirstLine, int linesAmount);
int it_is_mcrName(headMacro * mcr, char * line, FILE * amFile, FILE * source);