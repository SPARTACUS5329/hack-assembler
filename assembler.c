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

void preInitialize() {
    // List of possible computations
    insert("0", 101010, compTable);
    insert("1", 111111, compTable);
    insert("-1", 111010, compTable);
    insert("D", 001100, compTable);
    insert("A", 110000, compTable);
    insert("!D", 001101, compTable);
    insert("!A", 110001, compTable);
    insert("-D", 001111, compTable);
    insert("-A", 110011, compTable);
    insert("D+1", 011111, compTable);
    insert("A+1", 110111, compTable);
    insert("D-1", 001110, compTable);
    insert("A-1", 110010, compTable);
    insert("D+A", 000010, compTable);
    insert("D-A", 010011, compTable);
    insert("A-D", 000111, compTable);
    insert("D&A", 000000, compTable);
    insert("D|A", 010101, compTable);
    insert("M", 1110000, compTable);
    insert("!M", 1110001, compTable);
    insert("-M", 1110011, compTable);
    insert("M+1", 1110111, compTable);
    insert("M-1", 1110010, compTable);
    insert("D+M", 1000010, compTable);
    insert("D-M", 1010011, compTable);
    insert("M-D", 1000111, compTable);
    insert("D&M", 1000000, compTable);
    insert("D|M", 1010101, compTable);

    // List of possible destinations
    insert("null", 000, destTable);
    insert("M", 001, destTable);
    insert("D", 010, destTable);
    insert("MD", 011, destTable);
    insert("A", 100, destTable);
    insert("AM", 101, destTable);
    insert("AD", 110, destTable);
    insert("AMD", 111, destTable);

    // List of possible jumps
    insert("null", 000, jumpTable);
    insert("JGT", 001, jumpTable);
    insert("JEQ", 010, jumpTable);
    insert("JGE", 011, jumpTable);
    insert("JLT", 100, jumpTable);
    insert("JNE", 101, jumpTable);
    insert("JLE", 110, jumpTable);
    insert("JMP", 111, jumpTable);
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

    hash_table_item_t *destItem = search(dest, destTable);
    hash_table_item_t *compItem = search(comp, compTable);
    hash_table_item_t *jumpItem = search(jump, jumpTable);

    if (destItem == NULL) error("Syntax error. Destination statement not found!");
    if (compItem == NULL) error("Syntax error. Computation statement not found!");
    if (jumpItem == NULL) error("Syntax error. Jump statement not found!");

    int binDest = destItem->data;
    int compDest = compItem->data;
    int jumpDest = jumpItem->data;

    char binDestStr[MAX_LINE_LENGTH], binCompStr[MAX_LINE_LENGTH], binJumpStr[MAX_LINE_LENGTH];

    sprintf(binDestStr, "%d", binDest);
    sprintf(binCompStr, "%d", compDest);
    sprintf(binJumpStr, "%d", jumpDest);

    char *binDestPtr = leftPad(binDestStr, '0', 3);
    char *binCompPtr = leftPad(binCompStr, '0', 7);
    char *binJumpPtr = leftPad(binJumpStr, '0', 3);

    char *binInstruction = malloc(16 * sizeof(char));

    for (int i = 0; i < 13; i++) {
        if (i < 7) binInstruction[i] = binCompPtr[i];
        else if (i < 10) binInstruction[i] = binDestPtr[i - 7];
        else binInstruction[i] = binJumpPtr[i - 10];
    }

    binInstruction = leftPad(binInstruction, '1', 16);
    printf("binInstruction: %s\n", binInstruction);
    instruction = binInstruction;
    free(binInstruction);
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
    hash_table_item_t *jumpItem = search("null", jumpTable);
    if (jumpItem == NULL) printf("Item is NULL\n");
    else printf("jumpItem: %d\n", jumpItem->data);
    char **lines = initialize(fileName);
    firstPass(lines);
    secondPass(lines);
    return 0;
}