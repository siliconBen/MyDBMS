#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BSIZE 8000

typedef struct block {
    FILE* data;
    char bid[15];
    struct block* next;
} BLOCK;

typedef struct header {
    BLOCK* start;
    int offset;
    FILE* schema;
} HEADER;

BLOCK* newBlock() {
    BLOCK* newBlock = (BLOCK *) malloc(sizeof(BLOCK));
    snprintf(newBlock->bid, 16, "%p\n", newBlock);
    newBlock->data = fopen(newBlock->bid, "w");
    fclose(newBlock->data);
    newBlock->next = NULL;
    return newBlock;
}

//get more disk space
void appendBlock(int nBlocks) {
    ;
}

//add data, keep ordered
void writeBlock(char* toWrite, BLOCK* block) {
    fopen(block->bid, "w");
    fwrite(toWrite, 1, sizeof(toWrite), block->data);
    fclose(block->data);
}

void rmBlock(BLOCK* block) {
    free(block);
}

void rmContent(char* content) {
    free(content);
}

char* open(BLOCK* block) {
    char* content = (char*) malloc(sizeof(char[BSIZE]));
    block->data = fopen(block->bid, "r");
    
    if (block->data == NULL) {
        printf("file creation error");
    }
    fread(content, 1, BSIZE, block->data);
    fclose(block->data);
    return content;
}

int main() {
    BLOCK* one = newBlock();
    writeBlock("Hi there\n", one);
    char* data = open(one);
    printf("%s\n", data);
}