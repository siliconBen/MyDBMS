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
    FILE* schema;
} HEADER;

HEADER* newHeader(char* schema, char* tableName) {
    HEADER* newHeaderFile = (HEADER*) malloc(sizeof(HEADER));
    newHeaderFile->start->block = newBlock();
    newHeaderFile->start->offset = 8;
    newHeaderFile->schema = fopen(tableName, "wb");
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

int main() {
    BLOCK* one = buildBlockList();
    char mySchema[] = {0xAA, 0xBB, 0xCC, '\0'};
    
    char myTableName[] = "First Table";

    printf("%d %d\n", sizeof(mySchema), sizeof(myTableName));
    HEADER* myTable = newHeader(mySchema, myTableName);
}