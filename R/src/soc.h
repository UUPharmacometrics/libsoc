#ifndef _SOC_H
#define _SOC_H

#include <so/estring.h>

so_Table *df2table(SEXP df, char *table_name);
SEXP table2df(so_Table *table);
so_estring *character2estring(SEXP string, char *name);
SEXP estring2character(so_estring *estring);
SEXP matrix2Rmatrix(so_Matrix *matrix);
so_Matrix *Rmatrix2matrix(SEXP R_matrix, char *matrix_name);

#endif
