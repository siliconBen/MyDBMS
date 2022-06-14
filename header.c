#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "blocks.c"

typedef struct bptr {
    BLOCK* block;
    size_t offset;
    struct bptr* next;
} B_PTR;

typedef struct header {
    B_PTR* start;
    char* name;
    size_t tupleSize;
    FILE* schema;
} HEADER;

//abitary value atm
char headerBootstrap[] = {0xAA};

HEADER* newHeader(BLOCK* head, char* schema, char* tableName, size_t recordSize) {
    HEADER* newHeaderFile = (HEADER*) malloc(sizeof(HEADER));
    newHeaderFile->start = (B_PTR*) malloc(sizeof(B_PTR));
    //need system for finding staring block!!!
    newHeaderFile->start->block = smallestBlock(head, recordSize);
    printf("smallest ID: %d\n", newHeaderFile->start->block->bid);
    newHeaderFile->start->offset = 8 + sizeof(size_t);
    newHeaderFile->schema = fopen(tableName, "wb");
    newHeaderFile->tupleSize = recordSize;
    //check file was successful

    //write the schema and name
    fwrite(schema, strlen(schema)+1, 1, newHeaderFile->schema);

    fwrite(tableName, strlen(tableName)+1, 1, newHeaderFile->schema);

    fclose(newHeaderFile->schema);

    //read the name into mem
    newHeaderFile->schema = fopen(tableName, "r");

    newHeaderFile->name = (char*) malloc(strlen(tableName)+1);

    fread(newHeaderFile->name, strlen(schema)+1, 1, newHeaderFile->schema);
    fread(newHeaderFile->name, strlen(tableName)+1, 1, newHeaderFile->schema);
    fclose(newHeaderFile->schema);

    return newHeaderFile;
}

B_PTR* extendTable(HEADER* table) {
    B_PTR* extension = (B_PTR*) malloc(sizeof(B_PTR));
    extension->block = newBlock();
    extension->offset = 8+sizeof(size_t);

    B_PTR* probe = table->start;
    for(probe; probe->next != NULL; probe = probe->next) {
        ;
    }
    
    probe->next = extension;

    return extension;
}

void writeTable(HEADER* table, char* toWrite) {
    //seek for free space
    B_PTR* probe = table->start;

    for (probe; probe->next != NULL; probe = probe->next) {
        probe->block->data = fopen(probe->block->fileName, "r+");
        fseek(probe->block->data, probe->offset, SEEK_SET);
        char lastByte;
        char currentByte;
        int match = 0;
        int counter = 0;
        //two null bytes means end of table in this block
        while (!match) {
            fread(&currentByte, 1, 1, probe->block->data);
            if (currentByte == '\0' && lastByte == '\0') {
                match = 1;
            }
            lastByte = currentByte;
            counter++;
        }
        //are we at max file size?
        if ((BSIZE - probe->offset - counter - table->tupleSize) >= 0) {
            //space found
            char write[] = "hello world!";
            writeBlock(probe->block, write);
        }
        fclose(probe->block->data);
    }
    //no space found, extend then write
    B_PTR* writeTo = extendTable(table);
    char write[] = "hello world!";
    writeBlock(probe->block, write);
}

char* readTuple(FILE* fp, size_t recordSize) {
    char* record = (char*) malloc(sizeof(char[recordSize]));
    fread(record, recordSize, 1, fp);
    return record;
}

void newDb() {
    BLOCK* head = buildBlockList();
    //HEADER* tables = newHeader(head, headerBootstrap, "tables", 2);
    //return tables;
    fopen("db", "w");
    HEADER* hw = newHeader(head, headerBootstrap, "hello world", sizeof(int)+sizeof(size_t)+sizeof(int));
    extendTable(hw);
    extendTable(hw);
    extendTable(hw);
    extendTable(hw);
    extendTable(hw);
    printf("db created\n");
}

B_PTR* unpackB_PTRs(HEADER* new, BLOCK* head) {
    int bid;
    size_t offset;

    //read in data for new B_PTR
    char* record = readTuple(head->data, new->tupleSize);
    char bidC[sizeof(int)];
    char offsetC[sizeof(size_t)];
    
    B_PTR* bptr = (B_PTR*) malloc(sizeof(B_PTR));
    //first 4 bytes of record is bid
    for (int i=0; i<sizeof(int); i++) {
        bidC[i] = *record;
        record++;
    }
    bid = (int) *bidC;
    //next bytes are for offset
    for (int i=0; i<sizeof(size_t); i++) {
        offsetC[i] = *record;
        record++;
    }
    bptr->offset = (int) *offsetC;

    //check for NULL return
    bptr->block = blockByID(bid);
    
    //if next byte on disk is null, end of list, return
    char* null;
    fread(null, sizeof(char), 1, head->data);
    if (*null == '\0') {
        fseek(head->data, sizeof(char), SEEK_CUR);
        return bptr;
    }
    //else more to be read, so recurse
    else {
        bptr->next = unpackB_PTRs(new, head);
        return bptr;
    }
}

HEADER* unpackHeaders() {
    //read block structure into mem
    BLOCK* head = buildBlockList();
    //open first block and skip it's own metadata
    head->data = fopen(head->fileName, "r");
    fseek(head->data, 8+sizeof(size_t), SEEK_SET);
    
    size_t nameSize;
    int nulls = 0;
    char* secondNull;
    while(nulls < 2) {
        //make new header
        HEADER* new = (HEADER*) malloc(sizeof(HEADER));
        //get length of the name of the next table
        fread(&nameSize, sizeof(size_t), 1, head->data);
        //not null termiated string?
        //read in name of table
        fread(new->name, nameSize, 1, head->data);
        //read in size of records
        fread(&new->tupleSize, sizeof(size_t), 1, head->data);
        unpackB_PTRs(new, head);
        fread(secondNull, sizeof(char), 1, head->data);
        if (*secondNull == '\0') {
            nulls = 2;
        }
    }
}

int main() {
    //check if there's a db
    if (fopen("db", "r") != NULL) {
        printf("db exists, unpacking...\n");
        unpackHeaders();
    }
    //else create db (table header)
    else {
        printf("creating new db\n");
        newDb();
    }
}