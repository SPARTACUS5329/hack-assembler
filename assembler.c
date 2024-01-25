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

char** initialize(const char *fileName) {
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
    insert("R14", 14);
    insert("R15", 15);
    insert("SCREEN", 16384);
    insert("KBD", 24576);
    insert("SP", 0);
    insert("LCL", 1);
    insert("ARG", 2);
    insert("THIS", 3);
    insert("THAT", 4);

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
    char *line, *symbol = malloc(MAX_LINE_LENGTH * sizeof(char));
    int symbolCount = 0;
    for (int i = 0; i < lineCount; i++) {
        line = lines[i];
        if (line[0] == '(') {
            int j = 1;
            // Don't have to worry about j >= MAX_LINE_LENGTH
            // This is accounted for while reading the file
            while (line[j] != ')') {
                symbol[j - 1] = line[j];
                j++;
            }
            // Case "()"
            if (j == 1) error("Enter a valid label symbol");
            insert(symbol, i - symbolCount);
            symbolCount++;
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
            convertedInstruction = translateAInstruction(line, n);
            printf("convertedInstruction: %s\n", convertedInstruction);
        }
    }
}

char* translateAInstruction(char *instruction, int n){
    int binNum;
    if (isNumeric(++instruction)) {
        binNum = atoi(instruction);
        binNum = decimal2Binary(binNum);
    } else {
        insert(instruction, n);
        binNum = decimal2Binary(n);
    }

    char *binStr;
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