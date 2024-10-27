#ifndef __SBF_COMMAND_UTIL_H
#define __SBF_COMMAND_UTIL_H

#define FILE_ISNOT_EXISTS   (-1)
#define FILE_ISNOT_REGULER  (-2)

int get_filesize(char *fname);
char *basename(char *fname);

#endif /* __SBF_COMMAND_UTIL_H */
