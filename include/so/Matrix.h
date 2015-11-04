#ifndef _SO_MATRIX_H
#define _SO_MATRIX_H

#include <string.h>

/** \struct so_Matrix
	 \brief A structure representing an element containing a ct:Matrix
*/
typedef struct so_Matrix so_Matrix;

so_Matrix *so_Matrix_new(char *name);
void so_Matrix_free(so_Matrix *self);
void so_Matrix_ref(so_Matrix *self);
void so_Matrix_unref(so_Matrix *self);
int so_Matrix_get_number_of_columns(so_Matrix *self);
int so_Matrix_get_number_of_rows(so_Matrix *self);
void so_Matrix_set_size(so_Matrix *self, int numrows, int numcols);
char *so_Matrix_get_RowNames(so_Matrix *self, int index);
char *so_Matrix_get_ColumnNames(so_Matrix *self, int index);
void so_Matrix_set_RowNames(so_Matrix *self, int index, char *RowName);
void so_Matrix_set_ColumnNames(so_Matrix *self, int index, char *ColumnName);
double *so_Matrix_get_data(so_Matrix *self);

#endif
