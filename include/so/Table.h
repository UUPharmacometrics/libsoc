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

#ifndef _SO_TABLE_H
#define _SO_TABLE_H

#include <string.h>
#include <pharmml/common_types.h>

typedef struct so_Table so_Table;

typedef enum { PHARMML_DELIMITER_TAB, PHARMML_DELIMITER_SPACE, PHARMML_DELIMITER_COMMA, PHARMML_DELIMITER_SEMICOLON } pharmml_delimiter;

so_Table *so_Table_new(char *name);
void so_Table_free(so_Table *table);
void so_Table_ref(so_Table *self);
void so_Table_unref(so_Table *self);
void so_Table_set_number_of_rows(so_Table *self, int numrows);
void so_Table_remove_column(so_Table *self, int index);
char *so_Table_get_columnId(so_Table *self, int index);
pharmml_columnType so_Table_get_columnType(so_Table *self, int index);
pharmml_valueType so_Table_get_valueType(so_Table *self, int index);
void so_Table_set_columnId(so_Table *self, int index, char *columnId);
void so_Table_set_columnType(so_Table *self, int index, pharmml_columnType columnType);
void so_Table_set_valueType(so_Table *self, int index, pharmml_valueType valueType);
int so_Table_get_number_of_columns(so_Table *self);
int so_Table_get_number_of_rows(so_Table *self);
void *so_Table_get_column_from_number(so_Table *self, int number);
void *so_Table_get_column_from_name(so_Table *self, char *name);
void so_Table_new_column(so_Table *self, char *columnId, pharmml_columnType columnType, pharmml_valueType valueType, void *data);
void so_Table_new_column_no_copy(so_Table *self, char *columnId, pharmml_columnType columnType, pharmml_valueType valueType, void *data);
void so_Table_use_external_file(so_Table *self);
void so_Table_no_external_file(so_Table *self);
pharmml_delimiter so_Table_get_external_file_delimiter(so_Table *self);
void so_Table_set_external_file_delimiter(so_Table *self, pharmml_delimiter delimiter);
char *so_Table_get_external_file_oid(so_Table *self);
void so_Table_set_external_file_oid(so_Table *self, char *oid);
char *so_Table_get_external_file_path(so_Table *self);
void so_Table_set_external_file_path(so_Table *self, char *path);

#endif
