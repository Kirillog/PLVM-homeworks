#pragma once

#include <memory>

struct ByteCodeIter;

struct Bytefile {
private:
    /* The unpacked representation of bytecode file */
    typedef struct {
        char *end;
        char *string_ptr;          /* A pointer to the beginning of the string table */
        int *public_ptr;           /* A pointer to the beginning of publics table    */
        char *code_ptr;            /* A pointer to the bytecode itself               */
        int *global_ptr;           /* A pointer to the global area                   */
        int stringtab_size;        /* The size (in bytes) of the string table        */
        int global_area_size;      /* The size (in words) of global area             */
        int public_symbols_number; /* The number of public symbols                   */
        char buffer[0];
    } bytefile;

    /* Reads a binary bytecode file by name and unpacks it */
    bytefile *read_file(const char *fname);

    void close_file(bytefile *f);

    std::unique_ptr<bytefile> f_;

public:
    Bytefile(const char *fname);

    const char *get_ip() const;

    const char *get_end() const;

    /* Gets a string from a string table by an index */
    const char *get_string(size_t pos) const;

    size_t get_public_symbols_size() const;

    /* Gets a name for a public symbol */
    const char *get_public_name(size_t i) const;

    /* Gets an offset for a public symbol */
    size_t get_public_offset(size_t i) const;

    void disassemble(FILE *f) const;

    ~Bytefile();
};

void failure(const char *s, ...);