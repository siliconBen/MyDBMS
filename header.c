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

HEADER* newHeader(char* schema, char* tableName) {
    HEADER* newHeaderFile = (HEADER*) malloc(sizeof(HEADER));
    newHeaderFile->start = (B_PTR*) malloc(sizeof(B_PTR));
    newHeaderFile->start->block = newBlock();
    newHeaderFile->start->offset = 8;
    newHeaderFile->schema = fopen(tableName, "wb");
    newHeaderFile->tupleSize = 10;
    //check file was successful

    //write the schema and name
    fwrite(schema, strlen(schema)+1, 1, newHeaderFile->schema);
    fwrite(tableName, strlen(tableName)+1, 1, newHeaderFile->schema);
    fclose(newHeaderFile->schema);

    //read the name into mem
    newHeaderFile->schema = fopen(tableName, "r");
    fread(newHeaderFile->name, strlen(schema)+1, 1, newHeaderFile->schema);
    fread(newHeaderFile->name, strlen(tableName)+1, 1, newHeaderFile->schema);
    fclose(newHeaderFile->schema);

    return newHeaderFile;
}

HEADER* unpackHeaders() {
    HEADER* headers = (HEADER*) malloc(sizeof(HEADER[2]));

    for (int i = 0; i<2; i++) {
        headers->
    }
    return header
}

B_PTR* extendTable(HEADER* table) {
    B_PTR* extension = (B_PTR*) malloc(sizeof(B_PTR));
    extension->block = newBlock();
    extension->offset = 8;

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

int main() {
    BLOCK* one = buildBlockList();
    char mySchema[] = {0xAA, 0xBB, 0xCC, '\0'};
    
    char myTableName[] = "First Table";

    HEADER* myTable = newHeader(mySchema, myTableName);
    writeTable(myTable, "hi");
}