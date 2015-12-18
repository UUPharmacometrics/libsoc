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
#include <libxml/SAX.h>
#include <libxml/tree.h>

#include <so/Matrix.h>
#include <so/private/Matrix.h>
#include <pharmml/common_types.h>

/** \struct so_Matrix
	 \brief A structure representing a matrix
*/

/** \memberof so_Matrix
 * Create a new empty so_Matrix structure.
 * \return A pointer to the newly created struct or NULL if memory allocation failed
 * \sa so_Matrix_copy, so_Matrix_free
 */
so_Matrix *so_Matrix_new(char *name)
{
    so_Matrix *matrix = calloc(sizeof(so_Matrix), 1);
    if (matrix) {
        matrix->name = pharmml_strdup(name);
        matrix->reference_count = 1;
        if (!matrix->name) {
            free(matrix);
            matrix = NULL;
        } 
    }
    
    return matrix;
}

/** \memberof so_Matrix
 * Create a copy of a so_Matrix structure. 
 * \return A pointer to the Matrix copy or NULL if memory allocation failed
 * \sa so_Matrix_new
 */
so_Matrix *so_Matrix_copy(so_Matrix *source)
{
    so_Matrix *dest = so_Matrix_new(source->name);
    if (dest) {
        bool fail = false;
        if (so_Matrix_set_size(dest, source->numrows, source->numcols) != 0) {
            memcpy(dest->data, source->data, source->numrows * source->numcols * sizeof(double));
            for (int i = 0; i < source->numrows; i++) {
                dest->rownames[i] = pharmml_strdup(source->rownames[i]); 
                if (!dest->rownames[i]) {
                    fail = true;
                    break;
                }
            }
            if (!fail) {
                for (int i = 0; i < source->numcols; i++) {
                    dest->colnames[i] = pharmml_strdup(source->colnames[i]);
                    if (!dest->colnames[i]) {
                        fail = true;
                        break;
                    }
                }
            }
        } else {
            fail = true;
        }

        if (fail) {
            so_Matrix_free(dest);
            dest = NULL;
        }
    }

    return dest;
}

/** \memberof so_Matrix
 * Free all memory associated with a so_Matrix structure.
 * \param self - a pointer to the structure to free
 * \sa so_Matrix_new
 */
void so_Matrix_free(so_Matrix *self)
{
    if (self) {
        free(self->name);
        free(self->data);
        if (self->rownames) {
            for (int i = 0; i < self->numrows; i++) {
                free(self->rownames[i]);
            }
            free(self->rownames);
        }
        if (self->colnames) {
            for (int i = 0; i < self->numcols; i++) {
                free(self->colnames[i]);
            }
            free(self->colnames);
        }
        free(self);
    }
}

/** \memberof so_Matrix
 * Increase the reference count of an so_Matrix structure 
 * \param self - a pointer to the structure
 * \sa so_Matrix_unref
 */
void so_Matrix_ref(so_Matrix *self)
{
    self->reference_count++;
}

/** \memberof so_Matrix
 * Decrease the reference count of an so_Matrix structure.
 * If the reference count reaches zero the so_Matrix will be freed
 * \param self - a pointer to the Matrix
 * \sa so_Matrix_ref, so_Matrix_free
 */
void so_Matrix_unref(so_Matrix *self)
{
    if (self) {
        self->reference_count--;
        if (!self->reference_count) {
            so_Matrix_free(self);
        }
    }
}

/** \memberof so_Matrix
 * Get the number of columns in a matrix
 * \param self - pointer to an so_Marix
 * \return the number of columns in the matrix
 * \sa so_Matrix_get_number_of_rows
 */
int so_Matrix_get_number_of_columns(so_Matrix *self)
{
    return self->numcols;
}

/** \memberof so_Matrix
 * Get the number of rows in a matrix
 * \param self - pointer to an so_Matrix
 * \return the number of rows in the matrix
 * \sa so_Matrix_get_number_of_columns
 */
int so_Matrix_get_number_of_rows(so_Matrix *self)
{
    return self->numrows;
}

/** \memberof so_Matrix
 * Set the size of a newly created matrix.
 * Note that resizing of a matrix is not supported.
 * \param self - pointer to an so_Matrix
 * \param numrows - the number of rows to set
 * \param numcols - the number of columns to set
 * \return - 0 for success
 */
int so_Matrix_set_size(so_Matrix *self, int numrows, int numcols)
{
    self->data = calloc(numcols * numrows * sizeof(double), 1);
    self->colnames = calloc(numcols * sizeof(char *), 1);
    self->rownames = calloc(numrows * sizeof(char *), 1);

    if (self->data && self->colnames && self->rownames) {
        self->numrows = numrows;
        self->numcols = numcols;
        return 0;
    } else {
        free(self->data);
        free(self->colnames);
        free(self->rownames);
        self->data = NULL;
        self->colnames = NULL;
        self->rownames = NULL;
        return 1;
    }
}

/** \memberof so_Matrix
 * Get the name of a specific row in a matrix
 * \param self - pointer to an so_Matrix
 * \param index - index of the row
 * \return the name of the row
 * \sa so_Matrix_get_ColumnNames
 */
char *so_Matrix_get_RowNames(so_Matrix *self, int index)
{
    return self->rownames[index];
}

/** \memberof so_Matrix
 * Get the name of a specific column in a matrix
 * \param self - pointer to an so_Matrix
 * \param index - index of the column
 * \return the name of the column
 * \sa so_Matrix_get_RowNames
 */
char *so_Matrix_get_ColumnNames(so_Matrix *self, int index)
{
    return self->colnames[index];
}

/** \memberof so_Matrix
 * Set the name of a specific row in a matrix
 * \param self - pointer to an so_Matrix
 * \param index - index of the row
 * \param RowName - the name of the row
 * \result - 0 for success
 * \sa so_Matrix_set_ColumnNames
 */
int so_Matrix_set_RowNames(so_Matrix *self, int index, char *RowName)
{
    if (self->rownames[index]) {
        free(self->rownames[index]);
    }
    char *new_name = pharmml_strdup(RowName); 
    if (!new_name) {
        return 1;
    }
    self->rownames[index] = new_name;
    return 0;
}

/** \memberof so_Matrix
 * Set the name of a specific column in a matrix
 * \param self - pointer to an so_Matrix
 * \param index - index of the column
 * \param ColumnName - the name of the column
 * \result - 0 for success
 * \sa so_Matrix_set_RowNames
 */
int so_Matrix_set_ColumnNames(so_Matrix *self, int index, char *ColumnName)
{
    if (self->colnames[index]) {
        free(self->colnames[index]);
    }
    char *new_name = pharmml_strdup(ColumnName);
    if (!new_name) {
        return 1;
    }
    self->colnames[index] = new_name;
    return 0;
}

/** \memberof so_Matrix
 * Get a pointer to the marix data
 * \param self - pointer to an so_Matrix
 * \result - a pointer to the matrix data in row major order
 */
double *so_Matrix_get_data(so_Matrix *self)
{
    return self->data;
}

so_xml so_Matrix_xml(so_Matrix *self)
{
    xmlNodePtr xml = xmlNewNode(NULL, BAD_CAST self->name);

    xmlNodePtr def = xmlNewChild(xml, NULL, BAD_CAST "ct:Matrix", NULL);
    xmlNewProp(def, BAD_CAST "matrixType", BAD_CAST "Any");

    xmlNodePtr rownames = xmlNewChild(def, NULL, BAD_CAST "ct:RowNames", NULL);
    for (int i = 0; i < self->numrows; i++) {
        xmlNewChild(rownames, NULL, BAD_CAST "ct:String", BAD_CAST self->rownames[i]);
    }

    xmlNodePtr colnames = xmlNewChild(def, NULL, BAD_CAST "ct:ColumnNames", NULL);
    for (int i = 0; i < self->numcols; i++) {
        xmlNewChild(colnames, NULL, BAD_CAST "ct:String", BAD_CAST self->colnames[i]); 
    }

    for (int row = 0; row < self->numrows; row++) {
        xmlNodePtr matrix_row = xmlNewChild(def, NULL, BAD_CAST "ct:MatrixRow", NULL);
        for (int col = 0; col < self->numcols; col++) {
            char *value_string = pharmml_double_to_string(self->data[row * self->numcols + col]);
            xmlNewChild(matrix_row, NULL, BAD_CAST "ct:Real", BAD_CAST value_string);
            free(value_string);
        }
    }

    return xml;
}

int so_Matrix_start_element(so_Matrix *self, const char *localname, int nb_attributes, const char **attributes)
{
    if (strcmp("Matrix", localname) == 0) {
        self->in_matrix = 1;
    } else if (strcmp("RowNames", localname) == 0) {
        self->in_rownames = 1;
    } else if (strcmp("ColumnNames", localname) == 0) {
        self->in_columnnames = 1;
    } else if (strcmp("MatrixRow", localname) == 0) {
        self->in_matrixrow = 1;
    } else if (strcmp("String", localname) == 0) {
        self->in_string = 1;
    } else if (strcmp("Real", localname) == 0) {
        self->in_real = 1;
    }
    return 0;
}

void so_Matrix_end_element(so_Matrix *self, const char *localname)
{
    if (strcmp("Matrix", localname) == 0) {
        self->in_matrix = 0;
    } else if (strcmp("RowNames", localname) == 0) {
        self->in_rownames = 0;
    } else if (strcmp("ColumnNames", localname) == 0) {
        self->in_columnnames = 0;
    } else if (strcmp("MatrixRow", localname) == 0) {
        self->in_matrixrow = 0;
        self->current_col = 0;
        self->current_row++;
    } else if (strcmp("String", localname) == 0) {
        self->in_string = 0;
    } else if (strcmp("Real", localname) == 0) {
        self->in_real = 0;
        self->current_col++;
    }
}

int so_Matrix_characters(so_Matrix *self, const char *ch, int len)
{
    if (self->in_rownames && self->in_string) {
        char *row_name = pharmml_strndup(ch, len);
        if (!row_name) {
            return 1;
        }
        char **new_rownames = realloc(self->rownames, (self->numrows + 1) * sizeof(char *));
        if (!new_rownames) {
            free(row_name);
            return 1;
        }
        self->rownames = new_rownames;
        self->rownames[self->numrows] = row_name;
        self->numrows++;
    } else if (self->in_columnnames && self->in_string) {
        char *col_name = pharmml_strndup(ch, len);
        if (!col_name) {
            return 1;
        }
        char **new_colnames = realloc(self->colnames, (self->numcols + 1) * sizeof(char *));
        if (!new_colnames) {
            free(col_name);
            return 1;
        }
        self->colnames = new_colnames;
        self->colnames[self->numcols] = col_name;
        self->numcols++;
    } else if (self->in_matrixrow && self->in_real) {
        if (self->data == NULL) {
            void *new_data = malloc(self->numcols * self->numrows * sizeof(double));
            if (!new_data) {
               return 1;
            }
            self->data = new_data; 
        }
        char *str = (char *) ch;
        char saved = str[len];
        str[len] = '\0';
        double num = pharmml_string_to_double(str);
        self->data[self->current_row * self->numcols + self->current_col] = num; 
        str[len] = saved;
    }
}
