#include <immintrin.h>
#include <stack_opt.h>

#define CAPACITY  self->capacity
#define SIZE      self->size
#define ELEMENTS  self->elements


int stack_init(stack* const __restrict self, const int stack_size)
{
        if (stack_size <= 0) {
                PrettyPrint("list_size out of range\n");
                return ERROR_OUT_OF_RANGE;
        }

        CAPACITY = stack_size;
        SIZE     = 0;

        ELEMENTS = (stack_node*)aligned_alloc(ALIGNMENT, CAPACITY * sizeof(stack_node));
        
        if (ELEMENTS == nullptr) {
                PrettyPrint("Memory allocation error\n");
                return ERROR_MALLOC_ERROR;
        }

        return 0;
 };

int stack_push(stack* const __restrict      self,
               const char* const __restrict data,
               const size_t                 size)
{
        // Static aligned memory to copy data and after copy it to __m256i 
        alignas(32) char temp[MAX_WORD_SIZE] = {};

        if(SIZE == CAPACITY) {
                stack_node* temp = ELEMENTS;
                
                ELEMENTS = (stack_node*)aligned_alloc(ALIGNMENT,
                                                      sizeof(stack_node) * 2 * CAPACITY);
                 
                if (ELEMENTS == nullptr) {
                        PrettyPrint("Reallocation error\n");
                        return ERROR_MALLOC_ERROR;
                }

                memcpy(ELEMENTS, temp, sizeof(stack_node) * CAPACITY);
                free(temp);

                CAPACITY *= 2;
        }
        
        strncpy(temp, data, size);
        ELEMENTS[SIZE++].data = _mm256_load_si256((__m256i*)temp); 
        
        return 0;
}

int stack_destr(stack* const __restrict self)
{
        if (self == nullptr) {
                PrettyPrint("Unexpected nullptr self (stack* const)\n");
                return ERROR_NULL;
        }

        free(ELEMENTS);
        
        ELEMENTS = nullptr;
        SIZE     = CAPACITY = 0;

        return 0;
}

#undef SIZE
#undef CAPACITY
#undef ELEMENTS
