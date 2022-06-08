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

int main() {
    HEADER* head = newHeader();
    appendBlock(10, head);
    BLOCK* probe;
    for (probe=head->start; probe != NULL; probe = probe->next) {
        writeBlock("Hi there\n", probe);
        char* data = open(probe);
        printf("%s\n", data);
    }
}