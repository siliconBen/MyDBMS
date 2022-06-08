#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BSIZE 8000

typedef struct block {
    FILE* block;
    char bid[20];
    struct block* next;
} BLOCK;

BLOCK* newBlock() {
    BLOCK* newBlock = (BLOCK *) malloc(sizeof(BLOCK));
    strcpy(newBlock->bid, "Hello.bin\0");
    newBlock->next = NULL;
    return newBlock;
}

char* open(BLOCK* block) {
    char* content = (char*) malloc(sizeof(char[BSIZE]));
    block->block = fopen("hi.bin", "w");
    
    if (block->block == NULL) {
        printf("file creation error");
    }
    fread(content, 1, BSIZE, block->block);
    fclose(block->block);
    return content;
}

int main() {
    BLOCK* one = newBlock();
    char* print = open(one);
    
}