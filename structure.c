#include "structure.h"

headMacro * createMacroTable(){

    headMacro * list = malloc(sizeof(headMacro));

    if(list == NULL){
        memoAllocationFail();
    }
    return list;
}


headSymbol * createSymbolTable(){

    headSymbol * list = malloc(sizeof(headSymbol));

    if(list == NULL){
        memoAllocationFail();
    }
    return list;
}
