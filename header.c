#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "blocks.c"

typedef struct bptr {
    BLOCK* block;
    int offset;
    struct bptr* next;
} B_PTR

typedef struct header {
    B_PTR* start;
    FILE* schema;
} HEADER;

HEADER* newHeader() {
    HEADER* newHeaderFile = (HEADER*) malloc(sizeof(HEADER*));
    newHeaderFile->start->block = newBlock(-1);
    newHeaderFile->start->offset = 0;
    return newHeaderFile;
}



