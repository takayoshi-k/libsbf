#include <stdio.h>
#include <stdlib.h>

#include "sbfutil.h"
#include "libsbf.h"

#define BUFFER_SZ (1024)

int sbfbind(int argc, char **argv)
{
    int i;
    int fsize;
    int fnum;
    int padnum;
    char *tmp;
    char *target;
    FILE *fp;
    FILE *ifp;
    char pad[4] = {0, 0, 0, 0};
    int tlen;
    char buffer[BUFFER_SZ];

    struct sbf_header_t      top_hdr;
    struct sbf_contain_hdr_t each_hdr;

    if (argc < 3) {
        printf("Usage $ %s <output fine name> <file path to bind> (<file to bind>...)\n", argv[0]);
        return -1;
    }

    target = argv[1];

    switch (get_filesize(target)) {
      case FILE_ISNOT_REGULER:
        printf("%s has exist and not reguler file.\n", target);
        return -1;
      case FILE_ISNOT_EXISTS:
        break;
      default:
        printf("%s has alread exist, overwrite ? eneter number [0:No, 1:OK] :", target);
        scanf("%d", &i);
        if (i != 1) return -1;
        break;
    }
    
    /* Checck all file is reguler */

    for (i = 2, fnum = 0; i < argc; i++, fnum++) {
        fsize = get_filesize(argv[i]);
        switch (fsize) {
          case FILE_ISNOT_EXISTS:
            printf("File %s is not exists.\n", argv[i]);
            return -1;
          case FILE_ISNOT_REGULER:
            printf("File %s is not reguler file.\n", argv[i]);
            return -1;
        }
    }

    fp = fopen(target, "w");
    
    sbfb_create_tophdr(&top_hdr, fnum);
    fwrite(&top_hdr, 1, sizeof(top_hdr), fp);

    for (i = 2; i < argc; i++) {
        fsize = get_filesize(argv[i]);
        tmp = basename(argv[i]);

        ifp = fopen(argv[i], "r");

        if (ifp == NULL) {
            printf("Fatal error when open a file %s\n", argv[i]);
            fclose(fp);
            return -1;
        }

        padnum = sbfb_create_filehdr(&each_hdr, fsize, tmp, strlen(tmp));
        printf("[B] %s : %d (pad:%d)\n", argv[i], fsize, padnum);
        fwrite(&each_hdr, 1, sizeof(each_hdr), fp);
        while (fsize > 0) {
            tlen = fread(buffer, 1, BUFFER_SZ, ifp);
            if (tlen > 0) {
                fwrite(buffer, 1, tlen, fp);
            }
            else {
                printf("Fatal error when read data %s\n", argv[i]);
                return -1;
            }
            fsize -= tlen;
        }

        fclose(ifp);

        if (padnum > 0) {
            fwrite(pad, 1, padnum, fp);
        }
    }

    fclose(fp);

    return 0;
}
