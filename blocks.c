#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BSIZE 8000
#define BSIZE2 BSIZE - (sizeof(FILE*) + sizeof(char*) + sizeof(struct block*))

typedef struct block {
    FILE* data;
    char bid[9];
    struct block* next;
} BLOCK;

BLOCK* end;

typedef struct header {
    BLOCK* start;
    int offset;
    int width;
    FILE* schema;
} HEADER;

//adds new block to end of list
BLOCK* newBlock(char id[9]) {
    BLOCK* newBlock = (BLOCK *) malloc(sizeof(BLOCK));
    
    strcpy(newBlock->bid, id);

    //write bid to end of last block file
    if (end == NULL) {
        end = newBlock;
    }
    else {
        end->data = fopen(end->bid, "r");
        char buff[BSIZE];
        fread(buff, 1, BSIZE, end->data);
        fclose(end->data);

        int j=0;
        for (int i=8; i < 16; i++) {
            buff[i] = id[j];
            j++;
        }

        end->data = fopen(end->bid, "w");
        fwrite(buff, 1, BSIZE, end->data);
        fclose(end->data);

        //link blocks in memory
        end->next = newBlock;
        end = newBlock;
    }
    
    // write bid to start of file
    newBlock->data = fopen(newBlock->bid, "wb");
    char null[8] = "\0\0\0\0\0\0\0\0";
    fwrite(newBlock->bid, 1, 8, newBlock->data);
    fwrite(null, 1, 8, newBlock->data);
    fclose(newBlock->data);

    newBlock->next = NULL;
    return newBlock;
}

//read blocks on disk into memory data structure (list)
BLOCK* unpackBlocks(char id[8]) {
    BLOCK* newBlock = (BLOCK *) malloc(sizeof(BLOCK));
    printf("passed id\n%s\n", id);
    newBlock->data = fopen(id, "r");
    //read first 8bytes for id
    fread(newBlock->bid, 1, 8, newBlock->data);
    printf("id in mem\n%s\n", newBlock->bid);
    //check id and bid match

    //read next 8bytes for next id
    char isNull[8];
    fread(isNull, 1, 8, newBlock->data);
    fclose(newBlock->data);
    printf("next id on disk\n%s\n", isNull);

    //if null, return block
    if (strcmp(isNull, "\0\0\0\0\0\0\0\0")==0) {
        end = newBlock;
        return newBlock;
    }
    //if not null, unpack next block (recursive)
    else {
        newBlock->next = unpackBlocks(isNull);
        return newBlock;
    }
}

HEADER* newHeader() {
    HEADER* newHead = (HEADER *) malloc(sizeof(HEADER));
    //newHead->start = newBlock();
    newHead->offset = 0;
    newHead->width = 1;
    return newHead;
}

//write new data to block, be careful preserving first 16 bytes
void writeBlock(unsigned char* toWrite, BLOCK* block) {
    fopen(block->bid, "w");
    fwrite(toWrite, 1, BSIZE, block->data);
    fclose(block->data);
    free(toWrite);
}

void freeBlock(BLOCK* block) {
    free(block);
}

void freeContent(unsigned char* content) {
    free(content);
}

char* open(BLOCK* block) {
    char* content = (char*) malloc(sizeof(char[BSIZE]));
    block->data = fopen(block->bid, "r");

    if (block->data == NULL) {
        printf("file open error");
    }

    fread(content, 1, BSIZE, block->data);
    fclose(block->data);
    return content;
}

BLOCK* buildBlockList() {
    BLOCK* head;

    if (fopen("00000001", "r") == NULL) {
        head = newBlock("00000001");
    }
    else {
        head = unpackBlocks("00000001");
    }
    return head;
}

int main() {
    BLOCK* one = buildBlockList();
    
    newBlock("00000002");
    newBlock("00000003");
    BLOCK* four = newBlock("00000004");
    newBlock("00000005");

    char* helloworld = open(four);
    helloworld = "hello world, this will mess up the links on disk but hey ho";
    writeBlock(helloworld, four);
}