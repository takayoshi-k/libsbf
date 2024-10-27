#include <stdint.h>
#include <string.h>

// #define DEBUG_EN
#include "libsbf.h"

#define SBF_ACTIALSZ(s)    (((s)->filesize + 3) & 0xfffffffc)
#define SBF_PADDINGSIZE(s) ((4 - ((s) & 0x03)) & 0x03)

#define SBF_TAG "SBF "
#define IS_SBFFILE(s) ((s)->tag[0] == 'S' && (s)->tag[1] == 'B' && \
                       (s)->tag[2] == 'F' && (s)->tag[3] == ' ' && \
                       (s)->ver <= SBF_CURRENT_VERSION)

#ifndef MIN
#define MIN(a,b)  ((a) > (b) ? (b) : (a))
#endif

static void debug_show(struct sbf_unbind_t *sbf)
{
  DMSG("filenum:%d, fnum:%d comt:%s sz:%d remain:%d padsz:%d\n",
      sbf->filenum, 
      sbf->cur_fnum, 
      sbf->fhdr.comment, 
      sbf->fhdr.filesize, 
      sbf->cur_remain, 
      sbf->cur_padsz);
}

static void reset_file(struct sbf_unbind_t *sbf)
{
  sbf->cur_remain = -1;
  sbf->cur_padsz  = -1;
  sbf->hdrsz      = 0;
  memset(&sbf->fhdr, 0, sizeof(sbf->fhdr));
}

/* Unbind */

void sbf_init_unbind(struct sbf_unbind_t *sbf)
{
  sbf->filenum    = -1;
  sbf->cur_fnum   = -1;
  reset_file(sbf);
}

const char * sbfu_current_comment(struct sbf_unbind_t *sbf)
{
    return sbf->fhdr.comment;
}

int sbfu_current_fsize(struct sbf_unbind_t *sbf)
{
    return sbf->fhdr.filesize;
}

int sbfu_parse_file(struct sbf_unbind_t *sbfu, char *data, int sz,
                    char **dpos, int *rem)
{
    int ret = SBFU_RET_NOTFTOP;
    struct sbf_header_t *hdr;
    char *tmp;
    int remain;

    debug_show(sbfu);

    if (sbfu->filenum < 0) {
        DMSG("Starting SBF\n");

        if (sz < sizeof(struct sbf_header_t)) {
            ret = SBFU_RET_SMALLSZ;
        }

        hdr = (struct sbf_header_t *)data;
        if (IS_SBFFILE(hdr)) {
            sbfu->filenum = hdr->filenum;
            sbfu->cur_fnum = 0;
            sbfu->hdrsz    = 0;
            DMSG("Found HDR : filenum=%d\n", sbfu->filenum);
            *dpos = &data[sizeof(struct sbf_header_t)];
            *rem = sz - sizeof(struct sbf_header_t);
            ret = SBFU_RET_SBFHDR;
        }
        else {
            DMSG("Not SBF HDR\n");
            ret = SBFU_RET_NOTSBF;
        }
    }
    else if (sbfu->cur_fnum < sbfu->filenum) {
        if (sbfu->hdrsz < sizeof(struct sbf_contain_hdr_t)) {
            DMSG("Collecting file header\n");
            remain = sizeof(struct sbf_contain_hdr_t) - sbfu->hdrsz;
            remain = MIN(remain, sz);
            tmp = (char *)&sbfu->fhdr;
            memcpy(&tmp[sbfu->hdrsz], data, remain);
            sbfu->hdrsz += remain;
            *dpos = &data[remain];
            *rem = sz - remain;

            if (sbfu->hdrsz == sizeof(struct sbf_contain_hdr_t)) {
                sbfu->cur_remain = sbfu->fhdr.filesize;
                sbfu->cur_padsz  = SBF_PADDINGSIZE(sbfu->fhdr.filesize);
                sbfu->fhdr.comment[SBF_COMMENT_SZ - 1] = '\0';
                DMSG("Collected file header : size=%d, comment=%s, pad=%d\n",
                     sbfu->fhdr.filesize, sbfu->fhdr.comment, sbfu->cur_padsz);
                ret = sbfu->fhdr.filesize;
            }
        }
        else if (sbfu->cur_remain > 0) {
            DMSG("Data block passing : %d vs %d\n", sbfu->cur_remain, sz);
            ret = SBFU_RET_FILEDATA;
            remain = MIN(sbfu->cur_remain, sz);
            sbfu->cur_remain -= remain;
            *dpos = &data[remain];
            *rem = sz - remain;

            if (sbfu->cur_remain == 0) {
                DMSG("Data block done\n");
                ret = SBFU_RET_ENDFILE;
                if (sbfu->cur_padsz == 0) {
                    sbfu->cur_fnum++;
                    reset_file(sbfu);
                }
            }
        }
        else if (sbfu->cur_padsz >= 0) {
            DMSG("Data padding passing : %d vs %d\n", sbfu->cur_padsz, sz);
            remain = MIN(sbfu->cur_padsz, sz);
            sbfu->cur_padsz -= remain;
            *dpos = &data[remain];
            *rem = sz - remain;

            if (sbfu->cur_padsz == 0) {
                DMSG("Data Padding is done\n");
                sbfu->cur_fnum++;
                reset_file(sbfu);
                ret = SBFU_RET_ENDPADDING;
            }
        }
        else {
            DMSG("Should not happened\n");
        }
    }
    else {
        DMSG("No more file\n");
        ret = SBFU_RET_NOMOREFILE;
    }

    return ret;
}

int sbfu_split_file(struct sbf_unbind_t *sbfu, char *data, int sz,
                    int *savelen)
{
    int ret = SBFU_SPLIT_AVOID_DATA;
    int result;
    int rem;
    char *tmp = data;

    if (sz == 0) {
        /* No more data: maybe all files are split */
        return SBFU_SPLIT_ALLDONE;
    }

    rem = sz;
    while (rem) {
        result = sbfu_parse_file(sbfu, tmp, rem, &tmp, &rem);
        switch (result) {
          case SBFU_RET_SMALLSZ:
          case SBFU_RET_NOTSBF:
              return SBFU_SPLIT_ERROR;

          case SBFU_RET_SBFHDR:
              /* Continue parsing because this is 
                 that detection of SBF format file header,
                 not each files */
              break;

          case SBFU_RET_FILEDATA:
              /* File data to split */
              *savelen = sz - rem;
              return SBFU_SPLIT_FILEDATA;

          case SBFU_RET_ENDFILE:
              /* Final data of a file to split */
              *savelen = sz - rem;
              return SBFU_SPLIT_ENDFILE;

          case SBFU_RET_ENDPADDING:
          case SBFU_RET_NOTFTOP:
              /* No meaning data for splitting */
              *savelen = sz - rem;
              return SBFU_SPLIT_AVOID_DATA;

          case SBFU_RET_NOMOREFILE:
              /* Acheived file count to split */
              *savelen = 0;
              return SBFU_SPLIT_ALLDONE;

          default:
            if (ret > 0) {
              /* That ret is bigger than zero, means
                 that the file top is detected. So new file to split
                 should create */
              *savelen = sz - rem;
              return SBFU_SPLIT_NEWFILE;
            }
            return SBFU_SPLIT_ERROR;
        }
    }

    *savelen = sz - rem;
    return SBFU_SPLIT_AVOID_DATA;
}

/* Bind */

void sbfb_create_tophdr(struct sbf_header_t *hdr, int filenum)
{
    memset(hdr, 0, sizeof(struct sbf_header_t));
    memcpy(hdr->tag, SBF_TAG, sizeof(hdr->tag));
    hdr->ver = SBF_CURRENT_VERSION;
    hdr->filenum = filenum;
}

int sbfb_create_filehdr(struct sbf_contain_hdr_t *hdr, int filesize,
                        const char *cmt, int cmtlen)
{
    memset(hdr, 0, sizeof(struct sbf_contain_hdr_t));
    hdr->filesize = filesize;
    cmtlen = cmtlen > SBF_COMMENT_SZ ? SBF_COMMENT_SZ : cmtlen;
    strncpy(hdr->comment, cmt, cmtlen);
    hdr->comment[SBF_COMMENT_SZ - 1] = '\0';

    return SBF_PADDINGSIZE(filesize);
}
