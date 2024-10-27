#ifndef __LIB_SIMPLE_BIND_FILE_H
#define __LIB_SIMPLE_BIND_FILE_H

#include <stdint.h>
#include <string.h>

#define SBF_CURRENT_VERSION  (1)
#define SBF_COMMENT_SZ   (32)

#define SBFU_RET_NOTFTOP    (-1)
#define SBFU_RET_SMALLSZ    (-2)
#define SBFU_RET_NOTSBF     (-3)
#define SBFU_RET_NOMOREFILE (-4)
#define SBFU_RET_ENDFILE    (-5)
#define SBFU_RET_ENDPADDING (-6)
#define SBFU_RET_SBFHDR     (-7)
#define SBFU_RET_FILEDATA   (-8)

#define SBFU_SPLIT_ERROR        (-1)
#define SBFU_SPLIT_ENDFILE      (0)
#define SBFU_SPLIT_FILEDATA     (1)
#define SBFU_SPLIT_NEWFILE      (2)
#define SBFU_SPLIT_ALLDONE      (3)
#define SBFU_SPLIT_AVOID_DATA   (4)
#define SBFU_SPLIT_NOMORE_DATA  (5)

#ifdef DEBUG_EN
#include <stdio.h>
#ifndef DEBUG_BLOCK_NAME
#define DEBUG_BLOCK_NAME "libsbf"
#endif
#define DD(l,...) do { printf(l ":[" DEBUG_BLOCK_NAME "]((%d) ", __LINE__); printf(__VA_ARGS__); } while(0)
#define DERROR(...) DD("E", __VA_ARGS__)
#define DWARNING(...) DD("W", __VA_ARGS__)
#define DMSG(...) DD("M", __VA_ARGS__)
#else
#define DERROR(...)
#define DWARNING(...)
#define DMSG(...)
#endif

struct sbf_contain_hdr_t
{
  int32_t filesize;
  char comment[SBF_COMMENT_SZ];
  char data[0];
};

struct sbf_header_t
{
  char tag[4];
  uint8_t reserve;
  uint8_t ver;
  int16_t filenum;
};

struct sbf_unbind_t
{
  int16_t filenum;
  int16_t cur_fnum;
  int32_t cur_remain;
  int32_t cur_padsz;
  int32_t hdrsz;
  struct sbf_contain_hdr_t fhdr;
};

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Initialize unbind instance of sbf.
 *
 * @param [in/out] sbf: Instance of struct sbf_unbind_t to initialize.
 */
void sbf_init_unbind(struct sbf_unbind_t *sbf);

/**
 * Get comment field of current parsed file.
 *
 * @param [in] sbf: Instance of struct sbf_unbind_t.
 *
 * @return Text of the comment.
 */
const char * sbfu_current_comment(struct sbf_unbind_t *sbf);

/**
 * Get file size of current parsed file.
 *
 * @param [in] sbf: Instance of struct sbf_unbind_t.
 *
 * @return File size in bytes.
 */
int sbfu_current_fsize(struct sbf_unbind_t *sbf);

/**
 * Parse binary from sdb file.
 *
 * @param [in]  sbf : Instance of struct sbf_unbind_t.
 * @param [in]  data: Byte stream data from sdb file.
 * @param [in]  sz  : Length in data (in bytes).
 * @param [out] dpos: Pointer to store data for next parse.
 * @param [out] rem : Pointer to store remaining data in data.
 *
 * @return Return code in SBFU_RET_XXXXXX.
 */
int sbfu_parse_file(struct sbf_unbind_t *sbfu, char *data, int sz,
                    char **dpos, int *rem);

/**
 * Indicate data type for splitting files.
 *
 * @param [in]  sbf     : Instance of struct sbf_unbind_t.
 * @param [in]  data    : Byte stream data from sdb file.
 * @param [in]  sz      : Length in data (in bytes).
 * @param [out] savelen : Pointer to store how many bytes are the scope.
 *
 * @return Return code in SBFU_SPLIT_XXXXXX.
 */
int sbfu_split_file(struct sbf_unbind_t *sbfu, char *data, int sz,
                    int *savelen);

/**
 * Create sbf_header_t structure for top of sbf file.
 *
 * @param [out] hdr    : Pointer of the sbf_header_t for create content.
 * @param [in]  filenum: File number to conbine.
 */
void sbfb_create_tophdr(struct sbf_header_t *hdr, int filenum);

/**
 * Create sbf_contain_hdr_t structure as header structure of each files.
 *
 * @param [out] hdr     : Pointer of the sbf_contain_hdr_t for a file.
 * @param [in]  filesize: File size of the file.
 * @param [in]  cmt     : Text of comment. Usually store file path.
 * @param [in]  cmtlen  : Length of text in cmt.

 * @return Zero is returned if it is OK.
 */
int sbfb_create_filehdr(struct sbf_contain_hdr_t *hdr, int filesize,
                        const char *cmt, int cmtlen);

#ifdef __cplusplus
}
#endif

#endif /* __LIB_SIMPLE_BIND_FILE_H */
