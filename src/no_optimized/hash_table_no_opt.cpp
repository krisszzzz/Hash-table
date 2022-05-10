#include <hash_table_no_opt.h>


int init_hash_table(hash_table* const self,
                    const int         size,
                    const int         max_collision_count,
                    hash_t (*function_ptr)(const void* data, const size_t size))
{
        if (self == nullptr) {
                PrettyPrint("Unexpected nullptr hash_table while initializing\n");
                return NULLPTR_ERROR;
        }

        if (size <= 0) {
                PrettyPrint("Out of range size. The size should be positive at least\n");
                return OUT_OF_RANGE;
        }

        if (max_collision_count <= 0) {
                PrettyPrint("Out of range max_collision_count. The max_collision_size "
                            "should be positive\n");
        }

        if (function_ptr == nullptr) {
                PrettyPrint("The pointer to hash function is null\n");
                return NULLPTR_ERROR;
        }

        self->size = size;
        self->data = (stack*)calloc(size, sizeof(stack));

        if (self->data == nullptr) {
                PrettyPrint("Memory allocation error\n");
                return MALLOC_ERROR;
        }

        for (int iter_count = 0; iter_count < size; ++iter_count) {
                stack_init(&self->data[iter_count], max_collision_count);
        }

        self->hash_function  = function_ptr;
        return 0;
}


int add_element(hash_table* const self, const char* const element,
                const int         size)
{
        if (self == nullptr) {
                PrettyPrint("Unexpected nullptr hash_table\n");
                return NULLPTR_ERROR;
        }

        if (element == nullptr) {
                PrettyPrint("Unexpected nullptr string\n");
                return NULLPTR_ERROR;
        }

        hash_t element_index = self->hash_function(element, size) % self->size;

        if (hash_table_search(self, element, size, nullptr, nullptr) == 0) { 
                stack_push(&self->data[element_index], element, size);
        }                     

        return 0;
}

int hash_table_excel_stat(hash_table* const self,
                          const char* const file_name)        
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

        for (int iter_count = 0; iter_count < self->size; ++iter_count) {
                fprintf(excel_data, "%d, %d\n", iter_count, self->data[iter_count].size); 
        }

        fclose(excel_data);
        
        return 0;
}

int hash_table_stat(hash_table* const self)
{
        if (self == nullptr) {
                PrettyPrint("Unexpected nullptr hash_table\n");
                return NULLPTR_ERROR;
        }

        for (int iter_count = 0; iter_count < self->size; ++iter_count) {
                printf("hash_table element number %-8d - size: %-8d capacity : %-8d\n",
                       iter_count + 1, self->data[iter_count].size,
                       self->data[iter_count].capacity);
        }

        return 0;
}



int hash_table_search(hash_table* const self,        const char* const element,
                      const size_t      elem_size,   int*              in_table_pos,
                      int*              in_stack_pos)
{
        if (self == nullptr) {
                PrettyPrint("Unexpected nullptr self (hash_table* const)\n");
                return NULLPTR_ERROR;
        }

        if (element == nullptr) {
                PrettyPrint("Unexpected nullptr element (const char* const)\n");
                return NULLPTR_ERROR;
        }

        hash_t elem_index                 = self->hash_function(element, elem_size) % self->size;
        stack_node* current_stack_elem = self->data[elem_index].elements;
        
        for (int iter_count = 0; iter_count < self->data[elem_index].size; ++iter_count) {
                if (strncmp(current_stack_elem[iter_count].data, element, elem_size) == 0 && !current_stack_elem[iter_count].data[elem_size]) {
                        if (in_table_pos && in_stack_pos) {
                                *in_table_pos = elem_index;
                                *in_stack_pos = iter_count;
                        }
                        
                        return 1;
                } 
        }
              
        return 0; 
}

int hash_table_destr(hash_table* const self)
{
        if (self == nullptr) {
                PrettyPrint("Unexpected nullptr self (hash_table*)\n");
                return NULLPTR_ERROR;
        }

        for (int iter_count = 0; iter_count < self->size; ++iter_count) {
                stack_destr(&self->data[iter_count]);
        }

        free(self->data);

        self->data = nullptr;
        self->hash_function = nullptr;
        self->size = 0;
        
        return 0;
}

