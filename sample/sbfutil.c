#include <string.h>
#include <sys/stat.h>

#include "sbfutil.h"

int get_filesize(char *fname)
{
    struct stat st;

    if (stat(fname, &st) != 0) {
        return FILE_ISNOT_EXISTS;
    }

    if (S_ISREG(st.st_mode)) {
        return st.st_size;
    }
    else {
        return FILE_ISNOT_REGULER;
    }
}

char *basename(char *fname)
{
    char *end = fname + strlen(fname) - 1;

    while (end > fname) {
        if (*end == '/') break;
        end--;
    }

    return *end == '/' ? end + 1 : end;
}
