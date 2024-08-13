#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "assembler.h"


int main(int argc, char** argv){
    if (argc >= 2) {
        argv++; 
        argc--;
        assembler(argc, argv);
    }
    return 0;
}