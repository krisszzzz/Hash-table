
#if !defined STACK_INCLUDED 
#define STACK_INCLUDED

#include <stdlib.h>
#include <string.h>
#include <log.h>

#define MAX_WORD_SIZE 32 // Please use power of two word size to max optimization

enum ERRORS
{
        ERROR_NULL = -0xDED,
        ERROR_OUT_OF_RANGE,
        ERROR_MALLOC_ERROR
};

struct stack_node        
{
        char data[MAX_WORD_SIZE];
};

struct stack 
{
        stack_node* elements;
        int         capacity;
        int         size;
};


int stack_init(stack* const self, const int stack_size);

int stack_push(stack* const self, const char* const data, const int max_len);

int stack_destr(stack* const self);

#endif
