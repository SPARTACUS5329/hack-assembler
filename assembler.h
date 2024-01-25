#pragma once
#define SIZE 200

typedef struct symbolTableItem {
    int data;
    int key;
} symbol_table_item_t;

symbol_table_item_t* symbolTable[SIZE];
symbol_table_item_t* dummyItem;
symbol_table_item_t* item;

void error(const char*);
unsigned long hash(char*);
symbol_table_item_t* search(char*);
void insert(char*, int);
char** initialize(const char*);
void firstPass(char**);
void secondPass(char**);
char* translateAInstruction(char*);