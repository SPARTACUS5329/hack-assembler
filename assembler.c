#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "assembler.h"
#define MAX_LINES 500
#define MAX_LINE_LENGTH 20
static int lineCount = 0;

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

hash_table_item_t *search(char *key, hash_table_item_t* hashTable[]) {
   int hashIndex = hash(key);

   while(hashTable[hashIndex] != NULL) {
      if(hashTable[hashIndex]->key == hashIndex)
         return hashTable[hashIndex]; 
			
      ++hashIndex;
      hashIndex %= SIZE;
   }        
	
   return NULL;        
}

void insert(char *key, int data, hash_table_item_t* hashTable[]) {
   int hashIndex = hash(key);
   hash_table_item_t *item = (hash_table_item_t*) malloc(sizeof(hash_table_item_t));
   item->data = data;  
   item->key = hashIndex;

   while(hashTable[hashIndex] != NULL) {
      ++hashIndex;
      hashIndex %= SIZE;
   }
	
   hashTable[hashIndex] = item;
}

char** initialize(const char *fileName) {
    insert("R0", 0, symbolTable);
    insert("R1", 1, symbolTable);
    insert("R2", 2, symbolTable);
    insert("R3", 3, symbolTable);
    insert("R4", 4, symbolTable);
    insert("R5", 5, symbolTable);
    insert("R6", 6, symbolTable);
    insert("R7", 7, symbolTable);
    insert("R8", 8, symbolTable);
    insert("R9", 9, symbolTable);
    insert("R10", 10, symbolTable);
    insert("R11", 11, symbolTable);
    insert("R12", 12, symbolTable);
    insert("R13", 13, symbolTable);
    insert("R14", 14, symbolTable);
    insert("R15", 15, symbolTable);
    insert("SCREEN", 16384, symbolTable);
    insert("KBD", 24576, symbolTable);
    insert("SP", 0, symbolTable);
    insert("LCL", 1, symbolTable);
    insert("ARG", 2, symbolTable);
    insert("THIS", 3, symbolTable);
    insert("THAT", 4, symbolTable);

    FILE *file = fopen(fileName, "r");
    char **lines;
    char buffer[100];

    if (lines == NULL) error("Memory allocation error");

    lines = (char **)malloc(MAX_LINES * sizeof(char *));
    for (int i = 0; i < MAX_LINES; i++) {
        lines[i] = (char *)malloc(100 * sizeof(char));
        if (lines[i] == NULL) error("Memory allocation error");
    }

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        if (lineCount < MAX_LINES) {
            size_t length = strlen(buffer);
            if (buffer[length - 1] == '\n')
                buffer[length - 1] = '\0';

            strcpy(lines[lineCount], buffer);
            lineCount++;
        } else error("Too many lines in the file. Increase MAX_LINES.\n");
    }

    return lines;
}

void firstPass(char **lines) {
    char *line, *hash = malloc(MAX_LINE_LENGTH * sizeof(char));
    int hashCount = 0;
    for (int i = 0; i < lineCount; i++) {
        line = lines[i];
        if (line[0] == '(') {
            int j = 1;
            // Don't have to worry about j >= MAX_LINE_LENGTH
            // This is accounted for while reading the file
            while (line[j] != ')') {
                hash[j - 1] = line[j];
                j++;
            }
            // Case "()"
            if (j == 1) error("Enter a valid label hash");
            insert(hash, i - hashCount, symbolTable);
            hashCount++;
        }
    }
}

void secondPass(char **lines) {
    static int n = 16;
    char *line;
    char *convertedInstruction;
    for (int i = 0; i < lineCount; i++) {
        line = lines[i];
        if (line[0] == '@') {
            convertedInstruction = translateAInstruction(++line, n);
            printf("A Instruction: %s\n", convertedInstruction);
        } else if (line[0] != '(') {
            convertedInstruction = translateCInstruction(line);
            printf("C Instruction: %s\n", convertedInstruction);
        }
    }
}

char* translateAInstruction(char *instruction, int n){
    int binNum;
    hash_table_item_t *symbol;
    if (isNumeric(instruction)) {
        binNum = atoi(instruction);
        binNum = decimal2Binary(binNum);
    } else if ((symbol = search(instruction, symbolTable)) != NULL) {
        binNum = symbol->data;
    } else {
        insert(instruction, n, symbolTable);
        binNum = decimal2Binary(n);
    }

    char binStr[MAX_LINE_LENGTH];
    sprintf(binStr, "%d", binNum);
    int binStrLength = strlen(binStr);
    int numberOfPaddingZeros = 16 - binStrLength;
    char *binInstruction = malloc(16 * sizeof(char));

    for (int i = 0; i < 16; i++) {
        if (i < numberOfPaddingZeros) {
            binInstruction[i] = '0';
        } else {
            binInstruction[i] = binStr[i - numberOfPaddingZeros];
        }
    }

    instruction = binInstruction;
    free(binInstruction);
    return instruction;
}

char* translateCInstruction(char *instruction) {
    return instruction;
}

int decimal2Binary(int number) {
    int remainder, convertedNumber = 0, iters = 0;
    while (number) {
        remainder = number % 2;
        convertedNumber += remainder * pow(10, iters);
        iters += 1;
        number = number / 2;
    }
    return convertedNumber;
}

bool isNumeric(const char *str) {
    if (*str == '\0') return false;
    
    while (*str != '\0') {
        if (*str < '0' || *str > '9') return false;
        str++;
    }
    
    return true;
}

int main(int argc, char *argv[]) {
    if (argc < 2) error("File name not provided");
    const char *fileName = argv[1];
    printf("\nAssembling %s...\n\n", fileName);
    char **lines = initialize(fileName);
    firstPass(lines);
    secondPass(lines);
    return 0;
}