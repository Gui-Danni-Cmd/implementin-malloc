
#include "./malloc.h"
#include <stdio.h>


void init_heap() {
    heap = mmap(NULL, HEAP_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if( heap == MAP_FAILED ) {
        perror("Mmap falha no block de memoria!");
        return;
    }

    free_list = (Block*) heap;

    free_list->size = HEAP_SIZE - BLOCK_SIZE;
    free_list->free = 1;
    free_list->next = NULL;
}

void prox_block( Block *prox, size_t size ) {
    Block *new_Block = (void*) ((void*) prox + size + BLOCK_SIZE );
    new_Block->size = prox->size - size - BLOCK_SIZE;
    new_Block->free = -1;
    new_Block->next = prox->next;
    prox->size = size;
    prox->free = 0;
    prox->next = new_Block; 
}

void *_malloc( size_t size ) {
    Block *current, *prev = NULL;
    void *result;

    if( !heap ){
        init_heap();
        if( !heap ){
            return NULL;
        }
    } 
    int tentativas = 0;
    bool allocated = false;
    while (!allocated && tentativas < MAX_TENTATIVAS) {
        current = free_list;
        while( current != NULL ) {
            if( current->free && current->size >= size ){

                if( current->size > size + BLOCK_SIZE ) {
                    prox_block(current, size);
                }else {
                    current->free = 0;
                }
                result =( void *)(++current);
                allocated = true;
                break;
            }
            prev = current;
            current = current->next;
        }
        if (!allocated) {
            void *new_Heap = mmap(NULL, HEAP_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
            if(new_Heap == NULL) {
                perror("Error no mmap");
                return NULL;
            }
            Block * new_Block = (Block*) new_Heap;
            new_Block->size = HEAP_SIZE - BLOCK_SIZE;
            new_Block->free = 1;
            new_Block->next = NULL;
            if(prev){
                prev->next = new_Block;
            } else {
                free_list = new_Block;
            }
            tentativas++;
            break;
        }
    
    if (!allocated) {
        printf("Falha na alocação de memória após %d tentativas\n", MAX_TENTATIVAS);
        return NULL;
    }
    }

    return result;
}

void _free(void *ptr) {
    if( !ptr ) {
        return;
    }

    Block* block = (Block*)ptr - 1;
    memset(ptr, '\0', block->size);
    block->free = 1;
    ptr = NULL;
    Block *current = free_list;
    Block* prev = NULL;

    while ( current ) {
        if( current->free ){
            Block *next = current->next;
            while (next && next->free) {
                current->size += BLOCK_SIZE + next->size;
                current->next = next->next;
                next = current->next;
            }
            if ((void*)current == heap && current->next == NULL) {
                munmap(heap, HEAP_SIZE);
                heap = NULL;
                free_list = NULL;
                return;
            }

            if (prev) {
                prev->next = current->next;
            } else {
                free_list = current->next;
            }
            munmap(current, current->size + BLOCK_SIZE);
            return;
        }
        prev = current;
        current = current->next;
    }
    if ((void*)current == heap && current->next == NULL) {
        munmap(heap, HEAP_SIZE);
        heap = NULL;
        free_list = NULL;
        return;
    }
    
}

int main(){
    int *test = (int*) _malloc(sizeof(int));
    if( test == NULL) {
        printf("Deu ruim\n");
        exit(EXIT_FAILURE);
    }
    *test = 10;
    printf("O valor: %d\nEndereço de memoria: %p\n", *test, test);
    _free(test);
    //printf("O valor: %d\nEndereço de memoria: %p\n", *test, test);
    char *str = ( char *)_malloc(sizeof(char) * 10); 
    strncpy(str, "Guilherme", 9);
    str[10] = '\0';
    printf("Print %s\n", str);


    return 0;
}