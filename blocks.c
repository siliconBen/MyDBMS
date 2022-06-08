#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BSIZE 8000
#define BSIZE2 BSIZE - sizeof(FILE*) + sizeof(char*) + sizeof(struct block*)

typedef struct block {
    FILE* data;
    char bid[15];
    struct block* next;
} BLOCK;

typedef struct header {
    BLOCK* start;
    int offset;
    int width;
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

HEADER* newHeader() {
    HEADER* newHead = (HEADER *) malloc(sizeof(HEADER));
    newHead->start = newBlock();
    newHead->offset = 0;
    newHead->width = 1;
    return newHead;
}

//get more disk space
void appendBlock(int nBlocks, HEADER* table) {
    BLOCK* blockProbe = table->start;
    for (int i = 0; i < table->width; i++) {
        if (blockProbe->next == NULL) {
            printf("fatal err\n");
        }
        else {
            blockProbe = blockProbe->next;
        }
    }

    for (int i = 0; i < nBlocks; i++) {
        BLOCK* addBlock = newBlock();
        blockProbe->next = addBlock;
        blockProbe = blockProbe->next;
        table->width += 1;
    }
}

//add data, keep ordered!
void writeBlock(char* toWrite, BLOCK* block) {
    fopen(block->bid, "w");
    fwrite(toWrite, 1, sizeof(toWrite), block->data);
    fclose(block->data);
}

void freeBlock(BLOCK* block) {
    free(block);
}

void freeContent(char* content) {
    free(content);
}

unsigned char* open(BLOCK* block) {
    unsigned char* content = (unsigned char*) malloc(sizeof(unsigned char[BSIZE]));
    block->data = fopen(block->bid, "r");

    if (block->data == NULL) {
        printf("file open error");
    }

    fread(content, 1, BSIZE, block->data);
    fclose(block->data);
    return content;
}

BLOCK* buildBlockList() {
    char blockListStart[15];
    FILE* boot;
    BLOCK* start;

    boot = fopen("bootstrap.bin", "r");
    fgets(blockListStart, 15, boot);
    fclose(boot);

    if (strcmp(blockListStart, "") == 0) {
        printf("no boot ptr\n");

        boot = fopen("bootstrap.bin", "w");
        start = newBlock();
        snprintf(blockListStart, 16, "%p\n", start);
        fwrite(blockListStart, 1, sizeof(blockListStart), boot);
        fclose(boot);

        return start;
    }
    fclose(boot);
}

int main() {
    buildBlockList();
}