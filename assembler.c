#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SIZE 200

typedef struct symbolTableItem {
    int data;
    int key;
} symbol_table_item_t;

symbol_table_item_t* symbolTable[SIZE];
symbol_table_item_t* dummyItem;
symbol_table_item_t* item;

void error(const char *message) {
    perror(message);
    exit(1);
}

unsigned long hash(char *str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash % SIZE;
}

symbol_table_item_t *search(char *key) {
   int hashIndex = hash(key);

   while(symbolTable[hashIndex] != NULL) {
      if(symbolTable[hashIndex]->key == hashIndex)
         return symbolTable[hashIndex]; 
			
      ++hashIndex;
      hashIndex %= SIZE;
   }        
	
   return NULL;        
}

void insert(char *key, int data) {
   int hashIndex = hash(key);
   symbol_table_item_t *item = (symbol_table_item_t*) malloc(sizeof(symbol_table_item_t));
   item->data = data;  
   item->key = hashIndex;

   while(symbolTable[hashIndex] != NULL) {
      ++hashIndex;
      hashIndex %= SIZE;
   }
	
   symbolTable[hashIndex] = item;
}

void initialize() {
    insert("R0", 0);
    insert("R1", 1);
    insert("R2", 2);
    insert("R3", 3);
    insert("R4", 4);
    insert("R5", 5);
    insert("R6", 6);
    insert("R7", 7);
    insert("R8", 8);
    insert("R9", 9);
    insert("R10", 10);
    insert("R11", 11);
    insert("R12", 12);
    insert("R13", 13);
    insert("R14", 13);
    insert("R15", 13);
}

void firstPass(const char *fileName) {
    FILE *fp = fopen(fileName, "r");
    char line[50];
    char c;
    int numberOfLines = 0, i = 0;
    int flag = 1;
    while (flag) {
        c = getc(fp);
        if (strcmp(&c, "\n") == 32) { // figure this out
            printf("escn encountered %s\n", line);
            // lines[numberOfLines] = &line[0];
            i = 0;
            numberOfLines++;
        } else if (c == EOF) {
            printf("EOF encountered %s\n", line);
            // lines[numberOfLines] = &line[0];
            i = 0;
            numberOfLines++;
            flag = 0;
        }
        else {
            line[i] = c;
            i++;
            printf("%s\n", line);
        }
    }
}

char* translateAInstruction(char *instruction){
    printf("%s", instruction);
    return instruction;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        error("File name not provided");
    }
    const char *fileName = argv[1];
    printf("\nAssembling %s...\n\n", fileName);
    initialize();
    firstPass(fileName);
    // printf("%s", lines[0]);
    return 0;
}