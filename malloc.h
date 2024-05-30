#pragma once
#include <stdio.h>
#include <unistdio.h>
#include <sys/mman.h>
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <stdbool.h>


typedef struct Block {
    size_t size;
    int free;
    struct Block *next;
} Block;


#define BLOCK_SIZE sizeof(Block)
#define HEAP_SIZE 32768
#define MAX_TENTATIVAS 50


void *heap = NULL;
Block *free_list = NULL;

void init_heap();
void prox_block(Block *prox, size_t size);
void *_malloc( size_t size );
void _free( void *ptr );