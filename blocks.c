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
    size_t freeSpace;
    struct block* next;
} BLOCK;

BLOCK* start;
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

    newBlock->freeSpace = BSIZE - 8 - sizeof(size_t);
    fwrite(&newBlock->freeSpace, sizeof(size_t), 1, newBlock->data);

    fclose(newBlock->data);

    newBlock->next = NULL;

    return newBlock;
}

//read blocks on disk into memory data structure (list)
BLOCK* unpackBlocks(int id) {
    BLOCK* newBlock = (BLOCK *) malloc(sizeof(BLOCK));
    if (id == 1) {
        start = newBlock;
    }
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

    //read in freeSpace
    fread(&newBlock->freeSpace, sizeof(size_t), 1, newBlock->data);

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

//write new data to block, be careful preserving first 8 bytes
void writeBlock(BLOCK* block, char* toWrite) {
    block->data = fopen(block->fileName, "wb");
    //preserve the block metadata at start of file
    fwrite(&block->bid, sizeof(int), 1, block->data);
    if (block->next != NULL) {
        fwrite(&block->next->bid, sizeof(int), 1, block->data);
    }
    else {
        fwrite(&minusOne, sizeof(int), 1, block->data);
    }

    //write new data to rest of block
    fwrite(toWrite, 1, strlen(toWrite)+1, block->data);
    //update freespace
    block->freeSpace = block->freeSpace - (strlen(toWrite)+1);

    fclose(block->data);
}

void freeBlock(BLOCK* block) {
    free(block);
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
        head = unpackBlocks(1);
    }
    return head;
}


BLOCK* smallestBlock(BLOCK* head, size_t minSpace) {
    BLOCK* probe = head;
    BLOCK* currentSmallest = head;

    for (probe; probe->next != NULL; probe = probe->next) {
        if (probe->freeSpace < currentSmallest->freeSpace) {
            currentSmallest = probe;
        }
    }

    if (currentSmallest == head && head->freeSpace<minSpace) {
        return NULL;
    }
    else {
        return currentSmallest;
    }

}

BLOCK* blockByID(int bid) {
    BLOCK* probe = start;

    int i = 1;
    for(probe; probe->next != NULL; probe = probe->next) {
        if (i == bid) {
            return probe;
        }
        i++;
    }
    return NULL;
}

size_t findEOF(BLOCK* block) {

}