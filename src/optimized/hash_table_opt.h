#include <smmintrin.h>
#include <sys/types.h>
#if !defined HASH_TABLE_INCLUDED

#define HASH_TABLE_INCLUDED

#include <stack_opt.h>
#include <log.h>

typedef unsigned long long hash_t;

enum HASH_TABLE_ERRORS
{
        NULLPTR_ERROR = -0xDED,
        OUT_OF_RANGE,
        MALLOC_ERROR,
        VERIFICATION_ERROR,
};

struct alignas(32) hash_table
{
        int     size;
        stack*  data;
};

int init_hash_table(hash_table* const __restrict self,
                    const int                    size,
                    const int                    max_collision_count)
        __attribute__((nonnull(1)));

int hash_table_search(hash_table *const __restrict self,
                      const char *const __restrict element,
                      const size_t elem_size,
                      int *const __restrict in_table_pos = nullptr,
                      int *const __restrict in_stack_pos = nullptr)
        __attribute__((nonnull(1,2), hot));


int hash_table_search_in_stack(hash_table* const __restrict self,
                               const hash_t                 index,
                               const char* const __restrict element,
                               const size_t                 elem_size,
                               int* const  __restrict       in_table_pos = nullptr,
                               int* const  __restrict       in_stack_pos = nullptr
                             )
__attribute__((nonnull(1, 3), hot));


inline hash_t fast_hash(const void* data, const size_t size)
        __attribute__((nonnull(1), always_inline));


int add_element(hash_table* const __restrict self,
                const char* const __restrict element,
                const int                    size)
        __attribute__((nonnull(1,2), hot));


int hash_table_stat(hash_table* const __restrict self)
        __attribute__((nonnull(1)));

int hash_table_excel_stat(hash_table* const __restrict self,
                          const char* const __restrict file_name)
        __attribute__((nonnull(1)));

int hash_table_destr(hash_table* const __restrict self)
        __attribute__((nonnull(1)));


inline hash_t fast_hash(const void* data, const size_t size)
{
        u_int64_t crc     = 0xDED; // Random value
        u_int64_t* i_data = (u_int64_t*)data;
        
        return  (_mm_crc32_u64(crc, i_data[0])  &  _mm_crc32_u64(crc, i_data[1])) ^
                  _mm_crc32_u64(crc, i_data[2]) ^ _mm_crc32_u64(crc, i_data[3]);

}


#endif
