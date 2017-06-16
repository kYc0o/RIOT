#ifndef COMMON_H
#define COMMON_H

#include <unistd.h>
#include <sys/types.h>

off_t fsize(const char *filename);
int to_file(const char *filename, void *buf, size_t len);
int from_file(const char *filename, void *buf, size_t len);
int do_sha256(const char *filename, void *tgt, size_t offset);

#endif /* COMMON_H */
