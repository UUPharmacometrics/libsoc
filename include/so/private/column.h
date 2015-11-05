#ifndef _SO_PRIVATE_COLUMN_H
#define _SO_PRIVATE_COLUMN_H

#include <string.h>
#include <stdbool.h>
#include <pharmml/common_types.h>
#include <so/private/util.h>


typedef struct {
    char *columnId;
    pharmml_columnType columnType;
    pharmml_valueType valueType;
    int alloced_memory;
    int used_memory;
    int len;
    void *column;
} so_Column;

so_Column *so_Column_new();
void so_Column_free(so_Column *col);
void so_Column_set_columnId(so_Column *col, char *columnId);
void so_Column_set_valueType_from_string(so_Column *col, char *valueType);
void so_Column_set_columnType_from_string(so_Column *col, char *columnType);
void so_Column_set_valueType(so_Column *col, pharmml_valueType valueType);
void so_Column_set_columnType(so_Column *col, pharmml_columnType columnType);
void so_Column_add_real(so_Column *col, double real);
void so_Column_add_int(so_Column *col, int integer);
void so_Column_add_string(so_Column *col, char *str);
void so_Column_add_boolean(so_Column *col, bool b);

#endif
