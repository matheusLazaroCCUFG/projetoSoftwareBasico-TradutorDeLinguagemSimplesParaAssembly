#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "blpToAssembly.h"

void initialize_print(){
    printf(".section .rodata\n\n");
    printf(".data\n\n.text\n\n");
}

void initialize_registers(String * registers){
    strcpy(registers[4].reg, "%rdi");
    strcpy(registers[5].reg, "%edi");
    strcpy(registers[6].reg, "%rsi");
    strcpy(registers[7].reg, "%esi");
    strcpy(registers[8].reg, "%rdx");
    strcpy(registers[9].reg, "%edx");
}

void remove_newline(char *ptr){
    while (*ptr) {
        if (*ptr == '\n')
            *ptr = 0;
        else
            ptr++;
    }
}

const char * stack_String(int siz){
    static char buffer[10], aux[10];
    strcpy(aux, "(%rbp)");
    snprintf(buffer, 10, "%d", siz);
    strcat(buffer, aux);

    return buffer;
}

const char * constantToString(char * aux){
    int i;
    static char aux2[LINESZ];
    aux2[0] = '$';
    for(i = 2; i < strlen(aux); i++){
        aux2[i - 1] = aux[i];
    }
    aux2[i - 1] = '\0';

    return aux2;
}

const char * registers_Conversion(const char * regist, String * registers){

    if(strncmp(regist, "%r", 2) == 0 || strncmp(regist, "%", 1) != 0)
        return regist;

    static char answer[10];
    int i = 4;

    while(i < 10){
        if(strncmp(regist, registers[i].reg, 4) == 0) {
            break;
        }
        i++;
    }

    if(strncmp(registers[i - 1].reg, "%rdi", 4) == 0){
        strcpy(answer, "%r8");
    }else if(strncmp(registers[i - 1].reg, "%rsi", 4) == 0){
        strcpy(answer, "%r9");
    }else{
        strcpy(answer, "%r10");
    }

    return answer;
}



const char * map_Search(char * chave, int mapSize, Map * map){
    int i2 = 0;
    for(i2 = 0; i2 < mapSize; i2++){
        if(strncmp(chave, map[i2].chave, 3) == 0)
            break;
    }
    return map[i2].valor;
}

int constantToInteger(char * aux){
    int i;
    char aux2[LINESZ];
    for(i = 2; i < strlen(aux); i++){
        aux2[i - 2] = aux[i];
    }
    aux2[i - 2] = '\0';
    return atoi(aux2);
}

const char * operationToAssembly(char operation){
    if(operation == '+'){
        return "add";
    }else if(operation == '-'){
        return "sub";
    }

    return "imul";
}

int stack_Alignment(int stackSize, int alignment){
    if(stackSize % alignment == 0)
        return stackSize - alignment;

    while(stackSize % alignment != 0)
        stackSize--;
    return stackSize;
}

void arithmetic_Operation(char * answer, char * line, Map * map, int mapSize){
    char operation = 0;
    char firstElem[1000], secondElem[1000];
    char *p = strtok(line, " ");
    int i = 0;

    while(p != NULL){
        if(i == 2) {
            if (p[0] != 'c') {
                strcpy(firstElem, map_Search(p, mapSize, map));
            } else {
                strcpy(firstElem, constantToString(p));
            }
        }else if(i == 3){
            operation = p[0];
        }if(i == 4){
            if(p[0] != 'c') {
                strcpy(secondElem, map_Search(p, mapSize, map));
            }else{
                strcpy(secondElem, constantToString(p));
            }
        }
        i++;
        p = strtok(NULL, " ");
    }

    char operationString[50];
    strcpy(operationString, operationToAssembly(operation));
    if(operation != '/'){
        printf("\tmovl %s, %%r10d\n", firstElem);

        if(operation)
            printf("\t%sl %s, %%r10d\n", operationString, secondElem);

        printf("\tmovl %%r10d, %s\n", answer);
    }else{

        printf("\tmovl %%edx, %%r10d\n");
        printf("\tmovl $0, %%edx\n");
        printf("\tmovl %s, %%eax\n", firstElem);
        printf("\tmovl %s, %%ecx\n", secondElem);
        printf( "\tidivl %%ecx\n");
        printf( "\tmovl %%eax, %s\n", answer);
        printf("\tmovl %%r10d, %%edx\n");
    }

    printf("\n");

}

void calling_Operation(char * answer, char * line, int stackSize, Map * map, int mapSize, String * registers){
    char *p = strtok(line, " ");
    int i = 0;
    char function[LINESZ];

    stackSize = stack_Alignment(stackSize, 8);

    printf( "\tmovq %%rdi, %s\n", stack_String(stackSize));
    stackSize -= 8;
    printf( "\tmovq %%rsi, %s\n", stack_String(stackSize));
    stackSize -= 8;
    printf( "\tmovq %%rdx, %s\n\n", stack_String(stackSize));

    printf( "\tmovq %%rdi, %%r8\n");
    printf( "\tmovq %%rsi, %%r9\n");
    printf( "\tmovq %%rdx, %%r10\n\n");

    while(p != NULL){
        if(i == 3){
            strcpy(function, p);
        }

        if(i == 4){
            (p[1] == 'a') ? printf( "\tleaq %s, %%rdi\n", map_Search(p, mapSize, map)) :
            printf( "\tmovq %s, %%rdi\n", (p[0] == 'c') ? constantToString(p) : registers_Conversion(map_Search(p, mapSize, map), registers));
        }
        if(i == 5){
            (p[1] == 'a') ? printf( "\tleaq %s, %%rsi\n", map_Search(p, mapSize, map)) :
            printf( "\tmovq %s, %%rsi\n", (p[0] == 'c') ? constantToString(p) : registers_Conversion(map_Search(p, mapSize, map), registers));
        }
        if(i == 6){
            (p[1] == 'a') ? printf( "\tleaq %s, %%rdx\n", map_Search(p, mapSize, map)) :
            printf( "\tmovq %s, %%rdx\n", (p[0] == 'c') ? constantToString(p) : registers_Conversion(map_Search(p, mapSize, map), registers));
        }

        i++;
        p = strtok(NULL, " ");
    }
    printf( "\n\tcall %s\n", function);
    printf( "\tmovl %%eax, %s\n\n", answer);

    stackSize += 16;

    printf( "\tmovq %s, %%rdi\n", stack_String(stackSize));
    stackSize -= 8;
    printf( "\tmovq %s, %%rsi\n", stack_String(stackSize));
    stackSize -= 8;
    printf( "\tmovq %s, %%rdx\n\n", stack_String(stackSize));

}

void blpToAssemblyIf(char line[], int *endifs, int mapSize, Map *map){
    printf( "\t# IF %s\n", line);
    char *p = strtok(line, " ");
    int i = 0;
    *endifs = *endifs + 1;
    while (p != NULL) {
        if(i == 1){
            if(p[0] == 'c'){
                int x = constantToInteger(p);
                printf( "\tmovl $%d, %%r10d\n", x);
                printf("\tcmpl $0, %%r10d\n");
            }else{

                for(int i2 = 0; i2 < mapSize; i2++){
                    if(strncmp(p, map[i2].chave, strlen(p)) == 0){
                        printf( "\tcmpl $0, %s\n", map[i2].valor);
                        break;
                    }
                }
            }
        }
        p = strtok(NULL, " ");
        i++;
    }
    printf( "\tjne endif_%d\n", *endifs);
}


void blpToAssemblyEndIf(int endifs){
    printf( "endif_%d:\n", endifs);
}

int blpToAssemblyFunction(char line[], int mapSize, Map ** map, String registers[]) {
    mapSize++;
    (*map) = (Map *) realloc((*map), (mapSize * sizeof(Map)));
    strcpy((*map)[0].chave, "fmt01");
    strcpy((*map)[0].valor, "$fmt01");
    char *p = strtok(line, " ");
    int i = 0;

    while (p != NULL) {
        if (i == 1) {
            printf(".globl %s\n%s:\n", p, p);
            printf("\tpushq %%rbp\n"
                   "\tmovq %%rsp, %%rbp\n");
        } else if (i > 1) {
            mapSize++;
            (*map) = (Map *) realloc((*map), (mapSize * sizeof(Map)));
            strcpy((*map)[mapSize - 1].chave, p);
            (p[1] == 'a') ?
            strcpy((*map)[mapSize - 1].valor, registers[(i * 2)].reg) :
            strcpy((*map)[mapSize - 1].valor, registers[(i * 2) + 1].reg);
        }

        p = strtok(NULL, " ");
        i++;
    }

    return mapSize;
}
int blpToAssemblyLocalVariable(char * line, int mapSize, Map ** map, int * stack_size) {
    mapSize++;
    (*map) = (Map *) realloc((*map), (mapSize * sizeof(Map)));
    char *p = strtok(line, " ");
    int i = 0;
    if(strncmp(line, "var", 3) == 0) {

        while (p != NULL) {
            if(i == 1) {
                strcpy((*map)[mapSize - 1].chave, p);
                (*stack_size) -= 4;
                strcpy((*map)[mapSize - 1].valor, stack_String((*stack_size)));
                (*map)[mapSize - 1].isArray = 0;
                (*map)[mapSize - 1].size = 4;
            }
            p = strtok(NULL, " ");
            i++;
        }
    }else{
        while(p!= NULL){
            if(i == 1){
                strcpy((*map)[mapSize - 1].chave, p);
                (*map)[mapSize - 1].isArray = 1;
            }
            if(i == 3){
                (*map)[mapSize - 1].size = constantToInteger(p) * 4;
                (*stack_size) -= (*map)[mapSize - 1].size;
                strcpy((*map)[mapSize - 1].valor, stack_String((*stack_size)));
            }
            p = strtok(NULL, " ");
            i++;
        }
    }
    return mapSize;
}

void blpToAssemblyReturn(char * line, int mapSize, Map * map){
    printf( "\t# %s\n", line);
    char *p = strtok(line, " ");
    int i = 0;
    while(p != NULL){
        if(i){
            if(p[0] == 'c'){
                int x = constantToInteger(p);
                printf("\tmovl $%d, %%eax\n", x);
            }else{
                int i2 = 0;
                for(; i2 < mapSize; i2++){
                    if(strncmp(p, map[i2].chave, strlen(p)) == 0){
                        printf( "\tmovl %s, %%eax\n", map[i2].valor);
                        break;
                    }
                }
            }
            printf( "\tleave\n\tret\n");
        }
        p = strtok(NULL, " ");
        i++;
    }
}

void blpToAssemblySubqRsp(int * stack_size){

    int stack_parameters = -24;

    printf( "\tsubq $%d, %%rsp\n\n", (stack_Alignment(((*stack_size) + stack_parameters), 16)) * -1);
}

void blpToAssemblyAttribution(char * line, int mapSize, Map * map, int stack_size, String * registers){
    printf( "\t# %s\n", line);
    char line2[LINESZ];
    strcpy(line2, line);
    char *p = strtok(line, " ");
    int i = 0;
    char answer[50];
    while (i <= 2) {
        if(i == 0){
            strcpy(answer, map_Search(p, mapSize, map));
        }

        if(i == 2){
            if(strncmp(p, "call", 4) == 0){
                calling_Operation(answer, line2, stack_size, map, mapSize, registers);
            }else{
                arithmetic_Operation(answer, line2, map, mapSize);
            }
        }

        i++;
        p = strtok(NULL, " ");
    }
}

void blpToAssemblyGettersAndSetters(char * line, int mapSize, Map * map){
    printf( "\t# %s\n", line);
    char *p = strtok(line, " ");
    int isGet;
    int i = 0;
    while(p != NULL){
        if(i == 0){
            if(strncmp(p, "get", 3) == 0) {
                isGet = 1;
            }else{
                isGet = 0;
            }
        }

        if(i == 1){
            (p[0] == 'p') ? printf( "\tleaq (%s), %%r10\n", map_Search(p, mapSize, map))
                          : printf( "\tleaq %s, %%r10\n", map_Search(p, mapSize, map));
        }
        if(i == 3){
            printf( "\tmovq %s, %%r9\n", constantToString(p));
            printf( "\timulq $4, %%r9\n");
            printf( "\taddq %%r10, %%r9\n");
        }

        if(i == 5){
            if(isGet) {
                printf( "\tmovl (%%r9), %%r9d\n");
                printf( "\tmovl %%r9d, %s\n", map_Search(p, mapSize, map));
            }else {
                printf( "\tmovl %s, (%%r9)\n", constantToString(p));
            }
        }

        i++;
        p = strtok(NULL, " ");
    }
    printf( "\n");
}

void blpToAssemblyLeaveRet(Map ** map, int * mapSize, int * stack_size){
    printf( "\tleave\n");
    printf( "\tret\n\n\n");
    (*map) = NULL;
    (*mapSize) = 0;
    (*stack_size) = 0;
}
