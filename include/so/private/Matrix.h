#ifndef _SO_PRIVATE_MATRIX_H
#define _SO_PRIVATE_MATRIX_H

#include <so/xml.h>

struct so_Matrix {
    char *name;
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

so_xml so_Matrix_xml(so_Matrix *self);
void so_Matrix_start_element(so_Matrix *self, const char *localname, int nb_attributes, const char **attributes);
void so_Matrix_end_element(so_Matrix *self, const char *localname);
void so_Matrix_characters(so_Matrix *self, const char *ch, int len);

#endif
