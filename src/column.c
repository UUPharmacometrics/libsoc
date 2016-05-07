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

#include <stdbool.h>
#include <stdlib.h>
#include <so/private/column.h>
#include <pharmml/string.h>

so_Column *so_Column_new()
{
    so_Column *col = calloc(sizeof(so_Column), 1);

    return col;
}

void so_Column_free(so_Column *col)
{
    free(col->columnId);

    // If the data is allocated strings free them.
    if (col->valueType == PHARMML_VALUETYPE_STRING) {
        for (int i = 0; i < col->len; i++) {
            char **column = (char **) col->column;
            free(column[i]);
        }
    }
    free(col->column);
    free(col);
}

int so_Column_set_columnId(so_Column *col, char *columnId)
{
    char *new_columnId = pharmml_strdup(columnId);
    if (new_columnId) {
        col->columnId = new_columnId;
        return 0;
    } else {
        return 1;
    }
}

void so_Column_set_columnType_from_string(so_Column *col, char *columnType)
{
    col->columnType = pharmml_string_to_columnType(columnType);
}

void so_Column_set_valueType_from_string(so_Column *col, char *valueType)
{
    col->valueType = pharmml_string_to_valueType(valueType);
}

void so_Column_set_columnType(so_Column *col, pharmml_columnType columnType)
{
    col->columnType = columnType;
}

void so_Column_set_valueType(so_Column *col, pharmml_valueType valueType)
{
    col->valueType = valueType;
}


int so_Column_add_real(so_Column *col, double real)
{
    if (col->valueType != PHARMML_VALUETYPE_REAL) {
        return 1;
    }
    int new_used_memory = col->used_memory + sizeof(double);
    if (col->alloced_memory < new_used_memory) {
        int new_alloced_memory = col->alloced_memory + 256;
        void *new_column = realloc(col->column, new_alloced_memory);
        if (!new_column) {
            return 1;
        }
        col->alloced_memory = new_alloced_memory;
        col->column = new_column;
    }
    col->used_memory = new_used_memory;
    double *ptr = (double *) col->column;
    ptr[col->len] = real;
    col->len++;
    return 0;
}

int so_Column_add_int(so_Column *col, int integer)
{
    if (col->valueType != PHARMML_VALUETYPE_INT) {
        return 1;
    }
    int new_used_memory = col->used_memory + sizeof(int);
    if (col->alloced_memory < new_used_memory) {
        int new_alloced_memory = col->alloced_memory + 256;
        void *new_column = realloc(col->column, new_alloced_memory);
        if (!new_column) {
            return 1;
        }
        col->alloced_memory = new_alloced_memory;
        col->column = new_column;
    }
    col->used_memory = new_used_memory;
    int *ptr = (int *) col->column;
    ptr[col->len] = integer;
    col->len++;
    return 0;
}

int so_Column_add_string(so_Column *col, char *str)
{
    if (col->valueType != PHARMML_VALUETYPE_STRING) {
        return 1;
    }
    char *copy = pharmml_strdup(str);
    if (!copy) {
        return 1;
    }
    int new_used_memory = col->used_memory + sizeof(char *);
    if (col->alloced_memory < new_used_memory) {
        int new_alloced_memory = col->alloced_memory + 256;
        void *new_column = realloc(col->column, new_alloced_memory);
        if (!new_column) {
            free(copy);
            return 1;
        }
        col->alloced_memory = new_alloced_memory;
        col->column = new_column;
    }
    col->used_memory = new_used_memory;
    char **ptr = (char **) col->column;
    ptr[col->len] = copy;
    col->len++;
    return 0;
}

int so_Column_add_boolean(so_Column *col, bool b)
{
    int new_used_memory = col->used_memory + sizeof(bool);
    if (col->alloced_memory < new_used_memory) {
        int new_alloced_memory = col->alloced_memory + 256;
        void *new_column = realloc(col->column, new_alloced_memory);
        if (!new_column) {
            return 1;
        }
        col->alloced_memory = new_alloced_memory;
        col->column = new_column;
    }
    col->used_memory = new_used_memory;
    bool *ptr = (bool *) col->column;
    ptr[col->len] = b;
    col->len++;
    return 0;
}
