#include "structure.h"
#include "error_messages.h"
#include "constant.h"

void places_macros(char *nameOfAmFile, char *nameOfAsFile, int *error);
int checkWord (char *line, char *word);
void skipSpacesAndTabs(char *p);
void removeAllSpaces(char *p);
int it_is_mcrName(headMacro *mcr, char *line, FILE *asFile, FILE *source);