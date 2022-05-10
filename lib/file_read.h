#include <stdio.h>
#include <log.h>

#if !defined FILE_READ_INCLUDED
#define FILE_READ_INCLUDED
#define ALIGNMENT 32

char* file_to_buffer(FILE* source, int* const buffer_size)
{
        if (source == nullptr) {
                ErrorPrint(RED "ERROR: " END " nullptr FILE pointer in file_to_buffer\n");
                return nullptr;
        }

        fseek(source, 0, SEEK_END);
	int file_size = ftell(source);
	fseek(source, 0, SEEK_SET);

	char* buffer = (char*)aligned_alloc(ALIGNMENT, (file_size + 1) * sizeof(char));

        if (buffer == nullptr) {

                ErrorPrint(RED "ERROR:" END " memory allocation error\n");
                return nullptr;
        }

        if (buffer_size) {
                *buffer_size = file_size;
        }

        fread(buffer, sizeof(char), file_size, source);
	return buffer;
}

char* load_file(const char *const src_file_path, int* const size = nullptr)
{
        if (!src_file_path) {
                ErrorPrint("src_file_path is nullptr.\n");
                return nullptr;
        }

        FILE *source_file = fopen(src_file_path, "r");

        if (!source_file) {
                ErrorPrint(RED "ERROR: " END " cannot open file. File name : %s\n", src_file_path);
                return nullptr;
        }
        
        char* source_buffer = file_to_buffer(source_file, size);
        fclose(source_file);

        if (!source_buffer) {
                ErrorPrint(RED "ERROR: " END " memory allocation error\n");
                return nullptr;
        }

        return source_buffer;
}


#endif
