#if !defined INCLUDE_HASH_TABLE_H
#define INCLUDE_HASH_TABLE_H
#include <stdlib.h>

#define MAX_WORD_SIZE 32

typedef unsigned long long hash_t;

#if defined HASH_TABLE_OPTIMIZED 

#include <immintrin.h>

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


struct alignas(32) hash_table
{
        int     size;
        stack*  data;
};
        

extern int init_hash_table(hash_table* const __restrict self,
                           const int                    size,
                           const int                    max_collision_count)
        __attribute__((nonnull(1)));

extern int hash_table_search(hash_table *const __restrict self,
                             const char *const __restrict element,
                             const size_t elem_size,
                             int *const __restrict in_table_pos = nullptr,
                             int *const __restrict in_stack_pos = nullptr)
        __attribute__((nonnull(1,2), hot));


extern int hash_table_search_in_stack(hash_table* const __restrict self,
                                      const hash_t                 index,
                                      const char* const __restrict element,
                                      const size_t                 elem_size,
                                      int* const  __restrict       in_table_pos = nullptr,
                                      int* const  __restrict       in_stack_pos = nullptr
                             )
__attribute__((nonnull(1, 3), hot));



extern int add_element(hash_table* const __restrict self,
                       const char* const __restrict element,
                       const int                    size)
        __attribute__((nonnull(1,2), hot));


extern int hash_table_stat(hash_table* const __restrict self)
        __attribute__((nonnull(1)));

extern int hash_table_excel_stat(hash_table* const __restrict self,
                                 const char* const __restrict file_name)
        __attribute__((nonnull(1)));

extern int hash_table_destr(hash_table* const __restrict self)
        __attribute__((nonnull(1)));

#else

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

struct hash_table
{
        int     size;
        stack*  data;
        hash_t (*hash_function)(const void* data, const size_t size);
};


extern int init_hash_table(hash_table* const            self,
                           const int                    size,
                           const int                    max_collision_count,
                           hash_t (*function_ptr)(const void* data, const size_t size));

extern int hash_table_search(hash_table *const  self,
                             const char *const  element,
                             const size_t       elem_size,
                             int *const         in_table_pos = nullptr,
                             int *const         in_stack_pos = nullptr);
        
 extern int add_element(hash_table* const  self,
                        const char* const  element,
                        const int          size);
 
extern int hash_table_stat(hash_table* const self);
        
extern int hash_table_excel_stat(hash_table* const self,
                                 const char* const file_name);

extern int hash_table_destr(hash_table* const self);


#endif

#endif 

