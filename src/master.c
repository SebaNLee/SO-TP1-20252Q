
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "parameters.h"

#define ERROR -1




int main(int argc, char const *argv[]) {


    if (setParams(argc, ( char * const *) argv) == ERROR) {
        exit(EXIT_FAILURE);
    }
    printParams();
}