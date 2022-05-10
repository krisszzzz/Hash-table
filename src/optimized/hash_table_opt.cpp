#include "murmurhash.h"
#include <hash_table_opt.h>

#define SIZE                         self->size
#define DATA_PTR                     self->data
#define DATA(elem_index)             self->data[elem_index]
#define STACK_DATA(elem_index)       current_stack_elem[elem_index].data     
    

int init_hash_table(hash_table* const __restrict self,
                    const int                    size,
                    const int                    max_collision_count)
{
        if (size <= 0) {
                PrettyPrint("Out of range size. The size should be positive at least\n");
                return OUT_OF_RANGE;
        }

        if (max_collision_count <= 0) {
                PrettyPrint("Out of range max_collision_count. The max_collision_size "
                            "should be positive\n");
        }

        SIZE = size;
        DATA_PTR = (stack*)aligned_alloc(ALIGNMENT, size * sizeof(stack));

        if (DATA_PTR == nullptr) {
                PrettyPrint("Memory allocation error\n");
                return MALLOC_ERROR;
        }

        for (int iter_count = 0; iter_count < size; ++iter_count) {
                stack_init(&DATA(iter_count), max_collision_count);
        }

 
        return 0;
}

int add_element(hash_table* const __restrict self,
                const char* const __restrict element,
                const int                    size)
{
       hash_t element_index = MurmurHash(element, size) % SIZE;

       if (hash_table_search_in_stack(self,    element_index,
                                      element, size) == 0) {
                stack_push(&DATA(element_index), element, size);
        }
        
        return 0;

}

int hash_table_search(hash_table *const __restrict self,
                      const char *const __restrict element,
                      const size_t                 elem_size,
                      int *const __restrict        in_table_pos,
                      int *const __restrict        in_stack_pos)
{
        alignas(32) char temp[MAX_WORD_SIZE] = {};

        hash_t elem_index              = MurmurHash(element, elem_size) % SIZE;
        stack_node* current_stack_elem = DATA(elem_index).elements;
        
        int size = DATA(elem_index).size;
        strncpy(temp, element, elem_size);

        __m256i to_cmp = _mm256_load_si256((__m256i*)temp);
        
        for (int iter_count = 0; iter_count < size; ++iter_count) {
                
                __m256i mask        = _mm256_cmpeq_epi8(to_cmp, STACK_DATA(iter_count));
                unsigned int i_mask = (unsigned int)_mm256_movemask_epi8(mask);
                if (i_mask == 0xFFFFFFFF) {
                        if (in_table_pos && in_stack_pos) {
                                *in_table_pos = elem_index;
                                *in_stack_pos = iter_count;
                        }

                        return 1;
                }

        }
        

        return 0;
}

int hash_table_search_in_stack(hash_table* const __restrict self,
                               const hash_t                 index,
                               const char* const __restrict element,
                               const size_t                 elem_size,
                               int* const __restrict        in_table_pos,
                               int* const __restrict        in_stack_pos
                               )
{
        alignas(32) char temp[MAX_WORD_SIZE] = {};
                
        stack_node* current_stack_elem = DATA(index).elements;
        int size = DATA(index).size;

        strncpy(temp, element, elem_size);
        __m256i to_cmp = _mm256_load_si256((__m256i*)temp);
        
        for (int iter_count = 0; iter_count < size; ++iter_count) {
                __m256i mask = _mm256_cmpeq_epi8(to_cmp, STACK_DATA(iter_count));

                unsigned int i_mask = (unsigned int)_mm256_movemask_epi8(mask);

                if (i_mask == 0xFFFFFFFF) {
                        if (in_table_pos && in_stack_pos) {
                                *in_table_pos = index;
                                *in_stack_pos = iter_count;
                        }
                        
                        return 1;
                }

        }

        return 0;
        
}

int hash_table_excel_stat(hash_table* const __restrict self,
              	          const char* const __restrict file_name)        
{
        if (file_name == nullptr) {
                PrettyPrint("Unexpected nullptr file_name (const char* const)\n");
                return NULLPTR_ERROR; 
        }
        
        FILE* excel_data = fopen(file_name, "a");
        
        if (excel_data == nullptr) {
                PrettyPrint("Cannot open file to write excel data\n");
                return NULLPTR_ERROR;
        }

        for (int iter_count = 0; iter_count < SIZE; ++iter_count) {
                fprintf(excel_data, "%d, %d\n", iter_count, DATA(iter_count).size); 
        }

        fclose(excel_data);
        
        return 0;
}

int hash_table_stat(hash_table* const __restrict self)
{
        for (int iter_count = 0; iter_count < SIZE; ++iter_count) {
                printf("hash_table element number %-8d - size: %-8d capacity : %-8d\n",
                       iter_count + 1, DATA(iter_count).size,
                       DATA(iter_count).capacity);
        }

        return 0;
}




int hash_table_destr(hash_table* const __restrict self)
{
        for (int iter_count = 0; iter_count < SIZE; ++iter_count) {
                stack_destr(&DATA(iter_count));
        }

        free(DATA_PTR);

        DATA_PTR = nullptr;
        SIZE = 0;
        
        return 0;
       
}
