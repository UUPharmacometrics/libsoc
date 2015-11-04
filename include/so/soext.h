#ifndef _SO_SOEXT_H
#define _SO_SOEXT_H

char *so_get_last_error();
so_SO *so_SO_read(char *filename);
void so_SO_write(so_SO *self, char *filename, int pretty);
so_SOBlock *so_SO_get_SOBlock_from_name(so_SO *self, char *name);

#endif
