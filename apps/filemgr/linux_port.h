/* Linux replacements for the Solaris mount table and volume services. */
#ifndef FILEMGR_LINUX_PORT_H
#define FILEMGR_LINUX_PORT_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdint.h>
#include <ctype.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>
#include <libintl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/statvfs.h>
#include <mntent.h>
#include <xview/xview.h>
#include <xview/font.h>
#include <dirent.h>
#include <stdarg.h>
#include <fcntl.h>
#include <xview/defaults.h>
#include <xview/icon_load.h>
#include <xview/attr_.h>
#include <xview/xv_init_.h>
#include <xview/xv_usage_.h>
#include <xview/sel_req_.h>
#include "support/ds_item.h"
#undef MNTTAB
#define MNTTAB "/proc/self/mounts"
struct mnttab {
    char *mnt_special, *mnt_mountp, *mnt_fstype, *mnt_mntopts, *mnt_time;
};
/* Fill a Solaris-style entry from the Linux mount table; -1 means EOF. */
int fm_getmntent(FILE *stream, struct mnttab *entry);
/* Solaris volume management is unavailable; ordinary directories still work. */
int volmgt_running(void);
int volmgt_check(char *path);
/* Resolve a removable-media name; unsupported on Linux. */
char *media_findname(char *name);
void ds_expand_pathname(char *path, char *buffer);
char *ds_relname(void);
char *ds_expand_template(char *str, char *arg1, int mode);
/* Format into a fixed-size buffer and fail before using a truncated path. */
int fm_snprintf(char *buffer, size_t size, const char *format, ...);
/* Query options on an adapted mount-table entry. */
char *fm_hasmntopt(struct mnttab *entry, const char *option);
#endif
