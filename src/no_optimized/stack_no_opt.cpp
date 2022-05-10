#include <stack_no_opt.h>

int stack_init(stack* const self, const int stack_size)         
{
        if (self == nullptr) {
                PrettyPrint("nullptr list\n");
                return ERROR_NULL;
        }

        if (stack_size <= 0) {
                PrettyPrint("list_size out of range\n");
                return ERROR_OUT_OF_RANGE;
        }

        self->capacity = stack_size;
        self->size     = 0;

        self->elements = (stack_node*)calloc(stack_size, sizeof(stack_node));
        
        if (self->elements == nullptr) {
                PrettyPrint("Memory allocation error\n");
                return ERROR_MALLOC_ERROR;
        }

        return 0;
    
}

int stack_push(stack* const self, const char* const data, const int max_len)
{
        if (self == nullptr) {
                PrettyPrint("nullptr list\n");
                return ERROR_NULL;
        }

        if (data == nullptr) {
                PrettyPrint("nullptr data\n");
                return ERROR_NULL;
        }

        if (self->size >= self->capacity) {
                self->elements = (stack_node*)realloc(self->elements,
                                                              sizeof(stack_node) * 2 * self->capacity);
                
                self->capacity *= 2;
                
                if (self->elements == nullptr) {
                        PrettyPrint("Reallocation error\n");
                        return ERROR_MALLOC_ERROR;
                }
        }
        
        strncpy(self->elements[self->size++].data, data, max_len);
        return 0;
    
}

int stack_destr(stack* const self)
{
        if (self == nullptr) {
                PrettyPrint("Unexpected nullptr self (stack* const)\n");
                return ERROR_NULL;
        }

        free(self->elements);
        
        self->elements = nullptr;
        self->size     = self->capacity = 0;

        return 0;
}

 
