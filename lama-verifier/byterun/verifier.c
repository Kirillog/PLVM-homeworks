#include "bytefile.h"
#include "stdio.h"
#include "verify.h"

int main (int argc, char* argv[]) {
    bytefile *f = read_file (argv[1]);
    if (!verify(f)) {
        fprintf(stderr, "Verify error\n");
        return 1;
    }
    close_file(f);
    return 0;
}