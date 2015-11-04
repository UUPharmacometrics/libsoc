#ifndef _SO_ESTRING_H
#define _SO_ESTRING_H

#include <so/xml.h>

typedef struct so_estring so_estring;

so_estring *so_estring_new(char *name);
void so_estring_free(so_estring *self);
void so_estring_ref(so_estring *self);
void so_estring_unref(so_estring *self);
void so_estring_set_name(so_estring *self, char *name);
char *so_estring_get_name(so_estring *self);
void so_estring_set_string(so_estring *self, char *string);
char *so_estring_get_string(so_estring *self);

#endif
