#include "helpers.h"

#include <stdio.h>
#include <stdlib.h>

u8* read_file_binary(const char* path, size_t* out_size) {
    FILE* f;
    fopen_s(&f, path, "rb");
    if (!f) return NULL;

    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    rewind(f);

    u8* buffer = (u8*)malloc(size);
    if (!buffer) {
        fclose(f);
        return NULL;
    }

    fread(buffer, 1, size, f);
    buffer[size] = '\0';
    fclose(f);

    if (out_size)
        *out_size = size;

    return buffer;
}