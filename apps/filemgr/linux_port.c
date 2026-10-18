/* Linux adapters for optional Solaris desktop services. */
#include "linux_port.h"
#include "support/ce.h"
#include <errno.h>
int fm_getmntent(FILE *stream, struct mnttab *entry)
{
    struct mntent *mnt = getmntent(stream);
    if (!mnt) return -1;
    entry->mnt_special = mnt->mnt_fsname;
    entry->mnt_mountp = mnt->mnt_dir;
    entry->mnt_fstype = mnt->mnt_type;
    entry->mnt_mntopts = mnt->mnt_opts;
    entry->mnt_time = "0";
    return 0;
}
int volmgt_running(void) { return 0; }
int volmgt_check(char *path) { (void)path; errno = ENOSYS; return 0; }
/* File Manager's original -C mode handles absent Classing Engine services. */
int ce_begin(void *unused) { (void)unused; return ENOSYS; }
CE_NAMESPACE ce_get_namespace_id(char *name) { (void)name; return NULL; }
CE_ATTRIBUTE ce_get_attribute_id(CE_NAMESPACE ns, char *name)
{ (void)ns; (void)name; return NULL; }
char *ce_get_attribute(CE_NAMESPACE ns, CE_ENTRY e, CE_ATTRIBUTE attr)
{ (void)ns; (void)e; (void)attr; return NULL; }
CE_ENTRY ce_get_entry(CE_NAMESPACE ns, int count, ...)
{ (void)ns; (void)count; return NULL; }

char *media_findname(char *name) { (void)name; errno = ENOSYS; return NULL; }

int fm_snprintf(char *buffer, size_t size, const char *format, ...)
{
    va_list args;
    int length;
    va_start(args, format);
    length = vsnprintf(buffer, size, format, args);
    va_end(args);
    if (length < 0 || (size_t)length >= size) {
        fputs("filemgr: formatted path or command exceeds its buffer\n", stderr);
        exit(EXIT_FAILURE);
    }
    return length;
}
char *fm_hasmntopt(struct mnttab *entry, const char *option)
{
    struct mntent mnt = {0};
    mnt.mnt_opts = entry->mnt_mntopts;
    return hasmntopt(&mnt, option);
}
