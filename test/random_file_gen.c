#include <stdio.h>
#include <stdlib.h>

#define FILE_NAME_TEMPLATE "data/testdata%d.bin"
#define BUFSZ (1024)

static char fname[256];
static char databuff[BUFSZ];

static int generate_file(const char *name, int sz)
{
    int i;
    int r;
    FILE *fp;

    fp = fopen(name, "w");
    if (fp == NULL) return -1;

    while (sz) {
        r = sz > BUFSZ ? BUFSZ : sz;
        for (i = 0; i < r; i++) {
          databuff[i] = (char)(rand() & 0xff);
        }
        fwrite(databuff, 1, r, fp);
        sz -= r;
    }

    fclose(fp);
    return 0;
}

int main(int argc, char **argv)
{
    int i;
    int fnum = 10;
    int fsize;

    if (argc > 1) {
        fnum = atoi(argv[1]);
        if (fnum > 100 || fnum < 1) fnum = 10;
    }

    for (i = 0; i < fnum; i++) {
        sprintf(fname, FILE_NAME_TEMPLATE, i);
        fsize = rand() & 0xffff; /* max 64KB */
        if (fsize == 0) fsize = 1;
        printf("Generate %s : sz(%d)\n", fname, fsize);
        generate_file(fname, fsize);
    }

    return 0;
}
