#ifndef PRE_ASSEMBLER_H
#define PRE_ASSEMBLER_H
#include "constant.h"

void checkNumOfArgu (int args);
FILE *checkFileOpening(FILE *fd, char *fileToOpen, char *mode, int *error);
void memoAllocationFail(void);
void lineLengthLong(int *error, int line);
void illegalLabelName(int *error, int line);
void illegalMacroName(int *error, int line);
void longLabelName(int *error, int line);
void spaceInLabelStatement(int *error, int line);
void doubleLabelName(int *error, int line);
void extraTextContinue(int *error, int line);
void noSpaceOrTab(int *error, int line);
void invalidComma(int *error, int line);
void noParam(int *error, int line);
void invalidDataParam(int *error, int line);
void invalidString(int *error, int line);
void multipleCommas(int *error, int line);
void missingComma(int *error, int line);
void irrelevantLabel(int *error, int line);
void undefinedDirCommand(int *error, int line);
void undefinedInstructCommand(int *error, int line);
void IllegalInstParam(int *error, int line);
void invalidRegisterParam(int *error, int line);
void invalidNumberParam(int *error, int line);
void noLabelStatement(int *error, int line);
void labelCannotBeEnt(int *error, int line);
void exceededDataAmount(int *error);

#endif /* PRE_ASSEMBLER_H */