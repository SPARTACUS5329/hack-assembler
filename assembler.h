#pragma once
#define SIZE 200

typedef struct hashTableItem {
    int data;
    int key;
} hash_table_item_t;

hash_table_item_t* symbolTable[SIZE];
hash_table_item_t* compTable[50];
hash_table_item_t* destTable[20];
hash_table_item_t* jumpTable[20];
hash_table_item_t* dummyItem;
hash_table_item_t* item;

void error(const char*);
unsigned long hash(char*);
hash_table_item_t* search(char*, hash_table_item_t* hashTable[]);
void insert(char*, int, hash_table_item_t* hashTable[]);
char** initialize(const char*);
void firstPass(char**);
void secondPass(char**);
char* translateAInstruction(char*, int);
char* translateCInstruction(char*);
long int decimal2Binary(int);
bool isNumeric(const char*);
char* leftPad(char*, char, int);