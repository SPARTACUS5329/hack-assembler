#pragma once
#include <stdint.h>
#define SIZE 200
#define COMP_TABLE_SIZE 50
#define JUMP_TABLE_SIZE 20
#define DEST_TABLE_SIZE 20

typedef struct hashTableItem {
    int data;
    int key;
} hash_table_item_t;

typedef struct lookupTable {
    uint32_t data;
    int key;
} lookup_table_item_t;

hash_table_item_t* symbolTable[SIZE];
lookup_table_item_t* compTable[50];
lookup_table_item_t* destTable[20];
lookup_table_item_t* jumpTable[20];

void error(const char*);
unsigned long hash(char*);
hash_table_item_t* searchSymbol(char*, hash_table_item_t* hashTable[]);
lookup_table_item_t* searchKeyword(char*, lookup_table_item_t* hashTable[], int);
void insertSymbol(char*, int, hash_table_item_t* hashTable[]);
void insertKeyword(char*, int, lookup_table_item_t* hashTable[], int);
void preInitialize();
char** initialize(const char*);
void firstPass(char**);
void secondPass(char**);
char* translateAInstruction(char*, int);
char* translateCInstruction(char*);
long int decimal2Binary(int);
bool isNumeric(const char*);
char* leftPad(char*, char, int);