#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "sbfcmd.h"
#include "sbfutil.h"

#if 0
int sbfunbind(int argc, char **argv)
{
    printf("Unbind\n");
    return 0;
}
#endif

int main(int argc, char **argv)
{
    if (!strcmp(basename(argv[0]), "sbfbind")) {
        return sbfbind(argc, argv);
    }
    else if (!strcmp(basename(argv[0]), "sbfunbind")) {
        return sbfunbind(argc, argv);
    }
}
