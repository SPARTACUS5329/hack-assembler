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

hash_table_item_t *searchSymbol(char *key, hash_table_item_t* hashTable[]) {
   int hashIndex = hash(key);

   while(hashTable[hashIndex] != NULL) {
      if(hashTable[hashIndex]->key == hashIndex)
         return hashTable[hashIndex]; 
			
      ++hashIndex;
      hashIndex %= SIZE;
   }        
	
   return NULL;        
}

void insertSymbol(char *key, int data, hash_table_item_t* hashTable[]) {
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

lookup_table_item_t *searchKeyword(char *key, lookup_table_item_t* hashTable[], int size) {
   int hashIndex = hash(key);

   while(hashTable[hashIndex] != NULL) {
      if(hashTable[hashIndex]->key == hashIndex)
         return hashTable[hashIndex]; 
			
      ++hashIndex;
      hashIndex %= size;
   }        
	
   return NULL;        
}

void insertKeyword(char *key, int data, lookup_table_item_t* hashTable[], int size) {
   int hashIndex = hash(key);
   lookup_table_item_t *item = (lookup_table_item_t*) malloc(sizeof(lookup_table_item_t));
   item->data = data;  
   item->key = hashIndex;

   while(hashTable[hashIndex] != NULL) {
      ++hashIndex;
      hashIndex %= size;
   }
	
   hashTable[hashIndex] = item;
}

void preInitialize() {
    // List of possible computations
    insertKeyword("0", 101010, compTable, COMP_TABLE_SIZE);
    insertKeyword("1", 111111, compTable, COMP_TABLE_SIZE);
    insertKeyword("-1", 111010, compTable, COMP_TABLE_SIZE);
    insertKeyword("D", 1100, compTable, COMP_TABLE_SIZE);
    insertKeyword("A", 110000, compTable, COMP_TABLE_SIZE);
    insertKeyword("!D", 1101, compTable, COMP_TABLE_SIZE);
    insertKeyword("!A", 110001, compTable, COMP_TABLE_SIZE);
    insertKeyword("-D", 1111, compTable, COMP_TABLE_SIZE);
    insertKeyword("-A", 110011, compTable, COMP_TABLE_SIZE);
    insertKeyword("D+1", 11111, compTable, COMP_TABLE_SIZE);
    insertKeyword("A+1", 110111, compTable, COMP_TABLE_SIZE);
    insertKeyword("D-1", 1110, compTable, COMP_TABLE_SIZE);
    insertKeyword("A-1", 110010, compTable, COMP_TABLE_SIZE);
    insertKeyword("D+A", 10, compTable, COMP_TABLE_SIZE);
    insertKeyword("D-A", 10011, compTable, COMP_TABLE_SIZE);
    insertKeyword("A-D", 111, compTable, COMP_TABLE_SIZE);
    insertKeyword("D&A", 0, compTable, COMP_TABLE_SIZE);
    insertKeyword("D|A", 10101, compTable, COMP_TABLE_SIZE);
    insertKeyword("M", 1110000, compTable, COMP_TABLE_SIZE);
    insertKeyword("!M", 1110001, compTable, COMP_TABLE_SIZE);
    insertKeyword("-M", 1110011, compTable, COMP_TABLE_SIZE);
    insertKeyword("M+1", 1110111, compTable, COMP_TABLE_SIZE);
    insertKeyword("M-1", 1110010, compTable, COMP_TABLE_SIZE);
    insertKeyword("D+M", 1000010, compTable, COMP_TABLE_SIZE);
    insertKeyword("D-M", 1010011, compTable, COMP_TABLE_SIZE);
    insertKeyword("M-D", 1000111, compTable, COMP_TABLE_SIZE);
    insertKeyword("D&M", 1000000, compTable, COMP_TABLE_SIZE);
    insertKeyword("D|M", 1010101, compTable, COMP_TABLE_SIZE);

    // List of possible destinations
    insertKeyword("null", 0, destTable, DEST_TABLE_SIZE);
    insertKeyword("M", 1, destTable, DEST_TABLE_SIZE);
    insertKeyword("D", 10, destTable, DEST_TABLE_SIZE);
    insertKeyword("MD", 11, destTable, DEST_TABLE_SIZE);
    insertKeyword("A", 100, destTable, DEST_TABLE_SIZE);
    insertKeyword("AM", 101, destTable, DEST_TABLE_SIZE);
    insertKeyword("AD", 110, destTable, DEST_TABLE_SIZE);
    insertKeyword("AMD", 111, destTable, DEST_TABLE_SIZE);

    // List of possible jumps
    insertKeyword("null", 0, jumpTable, JUMP_TABLE_SIZE);
    insertKeyword("JGT", 1, jumpTable, JUMP_TABLE_SIZE);
    insertKeyword("JEQ", 10, jumpTable, JUMP_TABLE_SIZE);
    insertKeyword("JGE", 11, jumpTable, JUMP_TABLE_SIZE);
    insertKeyword("JLT", 100, jumpTable, JUMP_TABLE_SIZE);
    insertKeyword("JNE", 101, jumpTable, JUMP_TABLE_SIZE);
    insertKeyword("JLE", 110, jumpTable, JUMP_TABLE_SIZE);
    insertKeyword("JMP", 111, jumpTable, JUMP_TABLE_SIZE);
}

char** initialize(const char *fileName) {
    insertSymbol("R0", 0, symbolTable);
    insertSymbol("R1", 1, symbolTable);
    insertSymbol("R2", 2, symbolTable);
    insertSymbol("R3", 3, symbolTable);
    insertSymbol("R4", 4, symbolTable);
    insertSymbol("R5", 5, symbolTable);
    insertSymbol("R6", 6, symbolTable);
    insertSymbol("R7", 7, symbolTable);
    insertSymbol("R8", 8, symbolTable);
    insertSymbol("R9", 9, symbolTable);
    insertSymbol("R10", 10, symbolTable);
    insertSymbol("R11", 11, symbolTable);
    insertSymbol("R12", 12, symbolTable);
    insertSymbol("R13", 13, symbolTable);
    insertSymbol("R14", 14, symbolTable);
    insertSymbol("R15", 15, symbolTable);
    insertSymbol("SCREEN", 16384, symbolTable);
    insertSymbol("KBD", 24576, symbolTable);
    insertSymbol("SP", 0, symbolTable);
    insertSymbol("LCL", 1, symbolTable);
    insertSymbol("ARG", 2, symbolTable);
    insertSymbol("THIS", 3, symbolTable);
    insertSymbol("THAT", 4, symbolTable);

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
            insertSymbol(hash, i - hashCount, symbolTable);
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
            printf("Line %d: A Instruction: %s\n", i + 1, convertedInstruction);
        } else if (line[0] != '(') {
            convertedInstruction = translateCInstruction(line);
            printf("Line %d: C Instruction: %s\n", i + 1, convertedInstruction);
        }
    }
}

char* translateAInstruction(char *instruction, int n){
    long int binNum;
    hash_table_item_t *symbol;
    if (isNumeric(instruction)) {
        binNum = atoi(instruction);
        binNum = decimal2Binary(binNum);
    } else if ((symbol = searchSymbol(instruction, symbolTable)) != NULL) {
        binNum = decimal2Binary(symbol->data);
    } else {
        insertSymbol(instruction, n, symbolTable);
        binNum = decimal2Binary(n);
    }

    char binStr[MAX_LINE_LENGTH];
    sprintf(binStr, "%ld", binNum);
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
    char dest[5], comp[5], jump[5];

    char *equals_sign = strchr(instruction, '=');
    char *semicolon = strchr(instruction, ';');
    
    // Check if '=' and ';' exist
    if (equals_sign == NULL || semicolon == NULL)
        error("Invalid C-instruction format!");
    
    // dest
    strncpy(dest, instruction, equals_sign - instruction);
    dest[equals_sign - instruction] = '\0';
    
    // comp
    strncpy(comp, equals_sign + 1, semicolon - equals_sign - 1);
    comp[semicolon - equals_sign - 1] = '\0';
    
    // jump
    strcpy(jump, semicolon + 1);
    jump[strcspn(jump, "\n")] = '\0';

    lookup_table_item_t *destItem = searchKeyword(dest, destTable, DEST_TABLE_SIZE);
    lookup_table_item_t *compItem = searchKeyword(comp, compTable, COMP_TABLE_SIZE);
    lookup_table_item_t *jumpItem = searchKeyword(jump, jumpTable, JUMP_TABLE_SIZE);

    if (destItem == NULL) error("Syntax error. Destination statement not found!");
    if (compItem == NULL) error("Syntax error. Computation statement not found!");
    if (jumpItem == NULL) error("Syntax error. Jump statement not found!");

    int binDest = destItem->data;
    int binComp = compItem->data;
    int binJump = jumpItem->data;

    char binDestStr[MAX_LINE_LENGTH], binCompStr[MAX_LINE_LENGTH], binJumpStr[MAX_LINE_LENGTH];

    sprintf(binDestStr, "%d", binDest);
    sprintf(binCompStr, "%d", binComp);
    sprintf(binJumpStr, "%d", binJump);

    printf("%s %s %s\n", binDestStr, binCompStr, binJumpStr);

    char *binDestPtr = leftPad(binDestStr, '0', 3);
    char *binCompPtr = leftPad(binCompStr, '0', 7);
    char *binJumpPtr = leftPad(binJumpStr, '0', 3);

    printf("%s %s %s\n", binDestPtr, binCompPtr, binJumpPtr);

    char *binInstruction = malloc(16 * sizeof(char));

    for (int i = 0; i < 13; i++) {
        if (i < 7) binInstruction[i] = binCompPtr[i];
        else if (i < 10) binInstruction[i] = binDestPtr[i - 7];
        else binInstruction[i] = binJumpPtr[i - 10];
    }

    binInstruction = leftPad(binInstruction, '1', 16);
    instruction = binInstruction;
    // free(binInstruction);
    return instruction;
}

long int decimal2Binary(int number) {
    int remainder, iters = 0;
    long int convertedNumber = 0;
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

char* leftPad(char *instruction, char pad, int finalLength) {
    char *newInstruction = malloc(finalLength * sizeof(char));
    int length = strlen(instruction);
    for (int i = 0; i < finalLength; i++) {
        if (i < finalLength - length)
            newInstruction[i] = pad;
        else
            newInstruction[i] = instruction[i - (finalLength - length)];
    }
    instruction = newInstruction;
    free(newInstruction);
    return instruction;
}

int main(int argc, char *argv[]) {
    if (argc < 2) error("File name not provided");
    const char *fileName = argv[1];
    printf("\nAssembling %s...\n\n", fileName);
    preInitialize();
    char **lines = initialize(fileName);
    firstPass(lines);
    secondPass(lines);
    return 0;
}