#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "blpToAssembly.h"

int main(){

    char line[LINESZ];
    int endifs = 0;

    Map * map = NULL;
    int mapSize = 0;
    int stack_size = 0;

    String registers[10];

    initialize_registers(registers);

    initialize_print();

    while (fgets(line, LINESZ, stdin) != NULL) {
        remove_newline(line);

        if(strncmp(line, "function", 8) == 0) {
            mapSize = blpToAssemblyFunction(line, mapSize, &map, registers);
        }

        else if(strncmp(line, "var", 3) == 0 || strncmp(line, "vet", 3) == 0) {
            mapSize = blpToAssemblyLocalVariable(line, mapSize, &map, &stack_size);
        }

        else if(strncmp(line, "enddef", 6) == 0){
            blpToAssemblySubqRsp(&stack_size);
        }

        else if(line[0] == 'v' && line[1] == 'i'){
            blpToAssemblyAttribution(line, mapSize, map, stack_size, registers);
        }

        else if(strncmp(line, "get", 3) == 0 || strncmp(line, "set", 3) == 0) {
            blpToAssemblyGettersAndSetters(line, mapSize, map);
        }

        else if(strncmp(line, "if", 2) == 0) {
            blpToAssemblyIf(line, &endifs, mapSize, map);
        }

        else if(strncmp(line, "endif", 5) == 0){
            blpToAssemblyEndIf(endifs);
        }

        else if(strncmp(line, "return", 3) == 0){
            blpToAssemblyReturn(line, mapSize, map);
        }

        else if (strncmp(line, "end", 3) == 0 && strncmp(line, "enddef", 6) != 0 && strncmp(line, "endif", 5) != 0) {
            blpToAssemblyLeaveRet(&map, &mapSize, &stack_size);
        }
    }


    return 0;
}

