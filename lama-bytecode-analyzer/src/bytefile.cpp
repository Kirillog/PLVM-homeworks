#include "bytefile.h"
#include <cstdarg>
#include <cstdlib>
#include <memory>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <errno.h>

static void vfailure(const char *s, va_list args) {
    fprintf(stderr, "*** FAILURE: ");
    vfprintf(stderr, s, args);
    exit(255);
}

void failure(const char *s, ...) {
    va_list args;

    va_start(args, s);
    vfailure(s, args);
}

Bytefile::Bytefile(const char *fname) : f_(std::unique_ptr<bytefile>(read_file(fname))) {
}

Bytefile::bytefile *Bytefile::read_file(const char *fname) {
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

Bytefile::~Bytefile() {
    free(f_->global_ptr);
}

const char *Bytefile::get_string(size_t pos) const {
    if (pos >= f_->stringtab_size) {
        failure("unexpected pos %d in string table", pos);
    }
    return &f_->string_ptr[pos];
}

size_t Bytefile::get_public_symbols_size() const {
    return f_->public_symbols_number;
}

const char *Bytefile::get_public_name(size_t i) const {
    if (i >= f_->public_symbols_number) {
        failure("unexpected ind %d in public symbols table", i);
    }
    return get_string(f_->public_ptr[i * 2]);
}

size_t Bytefile::get_public_offset(size_t i) const {
    if (i >= f_->public_symbols_number) {
        failure("unexpected ind %d in public symbols table", i);
    }
    return f_->public_ptr[i * 2 + 1];
}

const char *Bytefile::get_ip() const {
    return f_->code_ptr;
}

const char *Bytefile::get_end() const {
    return f_->end;
}
