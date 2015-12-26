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

#ifndef _SO_MATRIX_H
#define _SO_MATRIX_H

#include <string.h>

typedef struct so_Matrix so_Matrix;

so_Matrix *so_Matrix_new();
so_Matrix *so_Matrix_copy(so_Matrix *source);
void so_Matrix_free(so_Matrix *self);
void so_Matrix_ref(so_Matrix *self);
void so_Matrix_unref(so_Matrix *self);
int so_Matrix_get_number_of_columns(so_Matrix *self);
int so_Matrix_get_number_of_rows(so_Matrix *self);
int so_Matrix_set_size(so_Matrix *self, int numrows, int numcols);
char *so_Matrix_get_RowNames(so_Matrix *self, int index);
char *so_Matrix_get_ColumnNames(so_Matrix *self, int index);
int so_Matrix_set_RowNames(so_Matrix *self, int index, char *RowName);
int so_Matrix_set_ColumnNames(so_Matrix *self, int index, char *ColumnName);
double *so_Matrix_get_data(so_Matrix *self);

#endif
