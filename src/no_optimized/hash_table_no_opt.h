#if !defined HASH_TABLE_INCLUDED

#define HASH_TABLE_INCLUDED

#include <stack_no_opt.h>
#include <log.h>

typedef unsigned long long hash_t;

enum HASH_TABLE_ERRORS
{
        NULLPTR_ERROR = -0xDED,
        OUT_OF_RANGE,
        MALLOC_ERROR,
        VERIFICATION_ERROR,
};

struct hash_table
{
        int     size;
        stack*  data;
        hash_t (*hash_function)(const void* data, const size_t size);
};


int init_hash_table(hash_table* const self,
                    const int         size,
                    const int         max_collision_count,
                    hash_t (*function_ptr)(const void* data, const size_t size));

int hash_table_search(hash_table* const self,
                      const char* const element,
                      const size_t      elem_size,
                      int*  in_table_pos = nullptr,
                      int*  in_stack_pos = nullptr);


int add_element(hash_table* const self, const char* const element,
                const int         size);
int hash_table_excel_stat(hash_table* const self,
                          const char* const file_name);

int hash_table_stat(hash_table* const self);

int hash_table_destr(hash_table* const self);

#endif
