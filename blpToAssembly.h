#ifndef LEITOR_H

#define LINESZ 256

typedef struct String{
    char reg[LINESZ];
}String;

typedef struct Map{
    char chave[LINESZ];
    char valor[LINESZ];
    int isArray;
    int size;
}Map;

void initialize_print();

void initialize_registers(String * registers);


void remove_newline(char *ptr);

const char * stack_String(int siz);


const char * constantToString(char * aux);

const char * map_Search(char * chave, int mapSize, Map * map);

const char * registers_Extension(const char * regist, String * registers);



int constantToInteger(char * aux);


const char * operationToAssembly(char operation);

int stack_Alignment(int stackSize, int alignment);


void arithmetic_Operation(char * answer, char * line, Map * map, int mapSize);


void calling_Operation(char * answer, char * line, int stackSize, Map * map, int mapSize, String * registers);


void blpToAssemblyIf(char line[], int *endifs, int mapSize, Map *map);

void blpToAssemblyEndIf(int endifs);

int blpToAssemblyFunction(char line[], int mapSize, Map ** map, String registers[]);

int blpToAssemblyLocalVariable(char * line, int mapSize, Map ** map, int * stack_size);

void blpToAssemblyReturn(char * line, int mapSize, Map * map);

void blpToAssemblySubqRsp(int * stack_size);

void blpToAssemblyAttribution(char * line, int mapSize, Map * map, int stack_size, String * Registers);

void blpToAssemblyGettersAndSetters(char * line, int mapSize, Map * map);

void blpToAssemblyLeaveRet(Map ** map, int * mapSize, int * stack_size);

#endif