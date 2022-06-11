#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BSIZE 8000
#define BSIZE2 BSIZE - (sizeof(FILE*) + sizeof(char*) + sizeof(struct block*))

typedef struct block {
    FILE* data;
    int bid;
    //char bid[9];
    char fileName[17];
    struct block* next;
} BLOCK;

BLOCK* end;
const int minusOne = -1;

//adds new block to end of list
BLOCK* newBlock() {
    BLOCK* newBlock = (BLOCK *) malloc(sizeof(BLOCK));
    
    //strcpy(newBlock->bid, id);
    if (end != NULL) {
        newBlock->bid = end->bid + 1;
        sprintf(newBlock->fileName, "%d", newBlock->bid);
    }

    //write bid to end of last block file
    if (end == NULL) {
        end = newBlock;
        newBlock->bid = 1;
        sprintf(newBlock->fileName, "%d", newBlock->bid);
    }
    else {
        end->data = fopen(end->fileName, "r");
        int buff[2];
        fread(buff, sizeof(int), 2, end->data);
        fclose(end->data);

        end->data = fopen(end->fileName, "wb");
        fwrite(&end->bid, sizeof(int), 1, end->data);
        fwrite(&newBlock->bid, sizeof(int), 1, end->data);
        fclose(end->data);

        //link blocks in memory
        end->next = newBlock;
        end = newBlock;
    }
    // write bid to start of file
    newBlock->data = fopen(newBlock->fileName, "wb");

    fwrite(&newBlock->bid, sizeof(int), 1, newBlock->data);

    fwrite(&minusOne, sizeof(int), 1, newBlock->data);

    fclose(newBlock->data);

    newBlock->next = NULL;

    return newBlock;
}

//read blocks on disk into memory data structure (list)
BLOCK* unpackBlocks(int id) {
    BLOCK* newBlock = (BLOCK *) malloc(sizeof(BLOCK));
    //printf("passed id\n%ld\n", id);
    sprintf(newBlock->fileName, "%d", id);
    newBlock->data = fopen(newBlock->fileName, "r");
    //read first 8bytes for id
    fread(&newBlock->bid, sizeof(int), 1, newBlock->data);
    printf("id in mem\n%d\n", newBlock->bid);
    //check id and bid match

    //read next 8bytes for next id
    int isNull;
    fread(&isNull, sizeof(int), 1, newBlock->data);
    fclose(newBlock->data);
    printf("next id on disk\n%d\n", isNull);

    //if null, return block
    if (isNull == -1) {
        end = newBlock;
        return newBlock;
    }
    //if not null, unpack next block (recursive)
    else {
        newBlock->next = unpackBlocks(isNull);
        return newBlock;
    }
}

//write new data to block, be careful preserving first 16 bytes
void writeBlock(char* toWrite, BLOCK* block) {
    block->data = fopen(block->fileName, "wb");
    fwrite(toWrite, 1, BSIZE, block->data);
    fclose(block->data);
    free(toWrite);
}

void freeBlock(BLOCK* block) {
    free(block);
}

void freeContent(char* content) {
    free(content);
}

char* openBlock(BLOCK* block) {
    char* content = (char*) malloc(sizeof(char[BSIZE]));
    block->data = fopen(block->fileName, "r");

    if (block->data == NULL) {
        printf("file open error");
    }

    fread(content, 1, BSIZE, block->data);
    fclose(block->data);
    return content;
}

BLOCK* buildBlockList() {
    BLOCK* head;
    if (fopen("1", "r") == NULL) {
        head = newBlock();
    }
    else {
        printf("here");
        head = unpackBlocks(1);
    }
    return head;
}

int main() {
    
    BLOCK* one = buildBlockList();
    newBlock();
    newBlock();
    newBlock();

    //char* helloworld = open(four);
    //helloworld = "hello world, this will mess up the links on disk but hey ho";
    //writeBlock(helloworld, four);
}