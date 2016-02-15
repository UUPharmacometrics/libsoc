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

#ifndef _SO_PRIVATE_MATRIX_H
#define _SO_PRIVATE_MATRIX_H

#include <libxml/xmlwriter.h>

struct so_Matrix {
    double *data;
    char **rownames;
    char **colnames;
    int numcols;
    int numrows;
    int current_row;
    int current_col;
    int in_matrix;
    int in_rownames;
    int in_columnnames;
    int in_matrixrow;
    int in_real;
    int in_string;
    int reference_count;
};

int so_Matrix_xml(so_Matrix *self, xmlTextWriterPtr writer, char *element_name);
int so_Matrix_start_element(so_Matrix *self, const char *localname, int nb_attributes, const char **attributes);
void so_Matrix_end_element(so_Matrix *self, const char *localname);
int so_Matrix_characters(so_Matrix *self, const char *ch, int len);

#endif
