#ifndef _UTIL_H
#define _UTIL_H

#include <stdlib.h>

void *extmalloc(size_t size);
void *extcalloc(size_t size);
void *extrealloc(void *memory, size_t size);
char *extstrdup(const char *str);
char *extstrndup(const char *str, size_t n);

#endif
