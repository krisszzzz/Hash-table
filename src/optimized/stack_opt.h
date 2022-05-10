#if !defined STACK_INCLUDED
#define STACK_INCLUDED

#include <stdlib.h>
#include <stdalign.h>
#include <log.h>
#include <string.h>
#include <immintrin.h>

#define MAX_WORD_SIZE 32
#define ALIGNMENT 32

enum ERRORS
{
        ERROR_NULL = -0xDED,
        ERROR_MALLOC_ERROR,
        ERROR_OUT_OF_RANGE 
};

struct alignas(32) stack_node
{
        __m256i     data;
};

struct alignas(32) stack
{ 
        stack_node* elements;
        int         capacity;
        int         size;
};



int stack_init(stack* const __restrict self, const int stack_size)
        __attribute__((nonnull(1)));

int stack_push(stack* const __restrict self, const char *const __restrict data, const size_t size) __attribute__((nonnull(1, 2), hot));


int stack_push_no_bound_check(stack* const __restrict      self,
                              const char* const __restrict data)
        __attribute__((nonnull(1, 2), hot));

int stack_destr(stack* const __restrict) __attribute__((nonnull(1)));
    

#endif
