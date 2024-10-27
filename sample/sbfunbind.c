#include <stdio.h>
#include <stdlib.h>

#include "sbfutil.h"

/* #define DEBUG_EN */
#define DEBUG_BLOCK_NAME "sbfunbind"
#include "libsbf.h"

#define BUFFER_SZ (1024)
#define FNAME_SZ (256)

#define ERROR_SPLITFILE     (-1)
#define FINISH_FILE         (0)
#define STOREDATA_TO_FILE   (1)
#define CREATE_NEW_FILE     (2)
#define ALL_SPLITTED        (3)
#define AVOID_DATA          (4)
#define NOMORE_DATA         (5)

static char fname[FNAME_SZ];
static char s_buffer[BUFFER_SZ];

static char *update_load_buffer(char *rem_buffer, int savelen, int *sz, FILE *fp)
{
    *sz = *sz - savelen;
    if (*sz > 0) {
        rem_buffer = &rem_buffer[savelen];
    }
    else {
        /* Load new data from sbf file */
        *sz = fread(s_buffer, 1, BUFFER_SZ, fp);
        rem_buffer = s_buffer;
    }

    return rem_buffer;
}
 
int sbfunbind(int argc, char **argv)
{
    int loop_en;
    int sz;
    int ret;
    int savelen;
    char *prefix = "";
    FILE *fp;
    FILE *ofp;
    struct sbf_unbind_t sbfu;
    char *rem_buffer;
    int file_cnt;

    if (argc != 2 && argc != 3) {
        printf("Usage $ %s <sbf file> (<prefix>)\n", argv[0]);
        return -1;
    }

    if (argc == 3) {
        prefix = argv[2];
    }

    if (get_filesize(argv[1]) <= 0) {
        printf("File %s is not reguler file\n", argv[1]);
    }

    sbf_init_unbind(&sbfu);
    fp = fopen(argv[1], "r");

    file_cnt = 0;
    sz = 0;
    savelen = 0;
    rem_buffer = NULL;
    ofp = NULL;
    loop_en = 1;
    while (loop_en) {
        rem_buffer = update_load_buffer(rem_buffer, savelen, &sz, fp);
        ret = sbfu_split_file(&sbfu, rem_buffer, sz, &savelen);
        switch (ret) {
          case SBFU_SPLIT_NEWFILE:
            if (ofp) {
                DERROR("Still open when CRATE_NEW_FILE\n");
                fclose(fp);
                fclose(ofp);
                return -1;
            }

            snprintf(fname, FNAME_SZ, "%s%s",
                     prefix, sbfu_current_comment(&sbfu));
            ofp = fopen(fname, "w");
            if (ofp == NULL) {
                printf("Splitting file : %s can not be opened\n",
                       fname);
                fclose(fp);
                return -1;
            }
            printf("[C] split file to %s sz:%d ... ", fname, sbfu_current_fsize(&sbfu));
            break;

          case SBFU_SPLIT_FILEDATA:
            if (ofp) {
                fwrite(rem_buffer, 1, savelen, ofp);
            }
            else {
                DERROR("No file when STOREDATA_TO_FILE\n");
                fclose(fp);
                return -1;
            }
            break;

          case SBFU_SPLIT_ENDFILE:
            if (ofp) {
                fwrite(rem_buffer, 1, savelen, ofp);
                fclose(ofp);
                printf("Done\n");
                file_cnt++;
                ofp = NULL;
            }
            else {
                DERROR("No file when FINISH_FILE\n");
                fclose(fp);
                return -1;
            }
            break;

          case SBFU_SPLIT_AVOID_DATA:
            break;

          case SBFU_SPLIT_ALLDONE:
            loop_en = 0;
            break;

          default:
            DWARNING("Unknown ret code:%d\n", ret);
            return -1;
        }
    }

    printf("Total %d files are split\n", file_cnt);

    fclose(fp);
    if (ofp) fclose(ofp);
    return 0;
}
