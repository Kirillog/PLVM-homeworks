#include "bytefile.h"
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <errno.h>
#include "../runtime/runtime.h"

const char* get_string (const bytefile *f, int pos) {
  if (pos < 0 || pos >= f->stringtab_size) { failure("ERROR: unexpected pos %d in string table", pos); }
  return &f->string_ptr[pos];
}

const char* get_public_name (const bytefile *f, int i) {
  if (i * 2 < 0 || i * 2 >= f->public_symbols_number) { failure("ERROR: unexpected ind %d in public symbols table", i); }
  return get_string (f, f->public_ptr[i*2]);
}

int get_public_offset(const bytefile *f, int i) { return f->public_ptr[i * 2 + 1]; }

bytefile *read_file (const char *fname) {
    FILE *f = fopen(fname, "rb");
    long size;
    bytefile *file;

    if (f == 0) {
        failure("%s\n", strerror(errno));
    }

    if (fseek(f, 0, SEEK_END) == -1) {
        failure("%s\n", strerror(errno));
    }

    file = (bytefile *)malloc(offsetof(bytefile, stringtab_size) + (size = ftell(f)));
    if (file == 0) {
        failure("unable to allocate memory.\n");
    }

    rewind(f);

    if (size != fread(&file->stringtab_size, 1, size, f)) {
        failure("%s\n", strerror(errno));
    }

    fclose(f);

    file->string_ptr = &file->buffer[file->public_symbols_number * 2 * sizeof(int)];
    file->public_ptr = (int *)file->buffer;
    file->code_ptr = &file->string_ptr[file->stringtab_size];
    file->global_ptr = (int *)malloc(file->global_area_size * sizeof(int));
    file->end = (char *)file + size + offsetof(bytefile, stringtab_size);
    return file;
}

void close_file(bytefile *f) {
  free(f->global_ptr);
  free(f);
}