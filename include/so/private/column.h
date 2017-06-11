/* libsoc - Library to handle standardised output files
 * Copyright (C) 2015 Rikard Nordgren
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 * 
 * his library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _SO_PRIVATE_COLUMN_H
#define _SO_PRIVATE_COLUMN_H

#include <string.h>
#include <stdbool.h>
#include <pharmml/common_types.h>


typedef struct {
    char *columnId;
    int num_columnType;
    pharmml_columnType *columnType;
    pharmml_valueType valueType;
    int alloced_memory;
    int used_memory;
    int len;
    void *column;
} so_Column;

so_Column *so_Column_new();
void so_Column_free(so_Column *col);
int so_Column_set_columnId(so_Column *col, char *columnId);
void so_Column_set_valueType_from_string(so_Column *col, char *valueType);
int so_Column_add_columnType_from_string(so_Column *col, char *columnType);
void so_Column_remove_columnType(so_Column *col);
int so_Column_set_columnType_from_string(so_Column *col, char *columnType);
void so_Column_set_valueType(so_Column *col, pharmml_valueType valueType);
int so_Column_add_columnType(so_Column *col, pharmml_columnType columnType);
int so_Column_add_real(so_Column *col, double real);
int so_Column_add_int(so_Column *col, int integer);
int so_Column_add_string(so_Column *col, char *str);
int so_Column_add_boolean(so_Column *col, bool b);

#endif
