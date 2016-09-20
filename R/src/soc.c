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

#include <R.h>
#include <Rdefines.h>
#include <so.h>
#include "soc.h"

SEXP r_so_SO_read(SEXP name)
{
    const char *s = CHAR(STRING_ELT(name, 0));

    so_SO *so = so_SO_read((char *) s);

    if (!so) {
        error(so_get_last_error());
    }

    SEXP ptr = R_MakeExternalPtr(so, R_NilValue, R_NilValue);

    return ptr;
}

SEXP r_so_SO_write(SEXP so, SEXP filename, SEXP pretty)
{
    struct so_SO *c_so = R_ExternalPtrAddr(so);
    const char *c_filename = CHAR(STRING_ELT(filename, 0));
    int c_pretty = INTEGER(pretty)[0];

    int fail = so_SO_write(c_so, (char *) c_filename, c_pretty);

    SEXP ret;
    PROTECT(ret = NEW_INTEGER(1));
    INTEGER_POINTER(ret)[0] = fail;
    UNPROTECT(1);

    return ret;
}

SEXP r_so_Table_ref(SEXP self)
{
    so_Table_ref(R_ExternalPtrAddr(self));
    return R_NilValue;
}

SEXP r_so_Table_free(SEXP self)
{
    so_Table_free(R_ExternalPtrAddr(self));
    return R_NilValue;
}

SEXP r_so_Matrix_free(SEXP self)
{
    so_Matrix_free(R_ExternalPtrAddr(self));
    return R_NilValue;
}


so_Table *df2table(SEXP df)
{
    so_Table *table = so_Table_new();

    if (!table) {
        error("Failed to create so_Table object");
    }

    int numcols = length(df); 

    int numrows = length(VECTOR_ELT(df, 0));
    so_Table_set_number_of_rows(table, numrows);

    const char *current_col_name;
    pharmml_columnType column_type;
    pharmml_valueType value_type;
    void *data;

    for (int i = 0; i < numcols; i++) {
        SEXP col_names = GET_ATTR(df, R_NamesSymbol);
        current_col_name = CHAR(STRING_ELT(col_names, i));

        SEXP col_types = GET_ATTR(df, install("columnType"));
        if (col_types == R_NilValue) {
            column_type = PHARMML_COLTYPE_UNDEFINED;
        } else {
            column_type = pharmml_string_to_columnType(CHAR(STRING_ELT(col_types, i)));
        }

        if (isReal(VECTOR_ELT(df, i))) {
            value_type = PHARMML_VALUETYPE_REAL;
            data = (void *) NUMERIC_POINTER(VECTOR_ELT(df, i));
            so_Table_new_column(table, (char *) current_col_name, column_type, value_type, data);
        } else if (isString(VECTOR_ELT(df, i))) {
            value_type = PHARMML_VALUETYPE_STRING;
            char **buffer = malloc(numrows * sizeof(char *));
            for (int j = 0; j < numrows; j++) {
                buffer[j] = pharmml_strdup(CHAR(STRING_ELT(VECTOR_ELT(df, i), j)));
            }
            so_Table_new_column_no_copy(table, (char *) current_col_name, column_type, value_type, buffer);
        } else if (isInteger(VECTOR_ELT(df, i))) {
            value_type = PHARMML_VALUETYPE_INT;
            data = (void *) INTEGER_POINTER(VECTOR_ELT(df, i));
            so_Table_new_column(table, (char *) current_col_name, column_type, value_type, data);
        }
    }

    return table;
}

SEXP table2df(so_Table *table)
{
    if (!table) {
        return R_NilValue;
    }

    SEXP col, list, row_names;

    int numcols = so_Table_get_number_of_columns(table);
    int numrows = so_Table_get_number_of_rows(table);

    // Create an empty data.frame
    PROTECT(list = NEW_LIST(numcols));
    SEXP class_name = PROTECT(allocVector(STRSXP, 1));
    SET_STRING_ELT(class_name, 0, mkChar("data.frame"));
    SET_CLASS(list, class_name);

    // Create the row.names attribute
    PROTECT(row_names = NEW_INTEGER(numrows));
    int *row_names_ptr = INTEGER_POINTER(row_names);
    for (int i = 1; i <= numrows; i++) {
        *row_names_ptr++ = i;
    }
    SET_ATTR(list, R_RowNamesSymbol, row_names);
    
    // Create the names attribute
    SEXP names;
    PROTECT(names = NEW_STRING(numcols));
    for (int i = 0; i < numcols; i++) {
        SET_STRING_ELT(names, i, mkChar(so_Table_get_columnId(table, i)));
    }
    SET_ATTR(list, R_NamesSymbol, names);

    // Create columnType attribute
    SEXP column_type;
    PROTECT(column_type = NEW_STRING(numcols));
    for (int i = 0; i < numcols; i++) {
        SET_STRING_ELT(column_type, i, mkChar(pharmml_columnType_to_string(so_Table_get_columnType(table, i))));
    }
    SET_ATTR(list, install("columnType"), column_type);

    // Create the columns
    for (int j = 0; j < numcols; j++) {
        pharmml_valueType vt = so_Table_get_valueType(table, j);
        if (vt == PHARMML_VALUETYPE_REAL) {
            double *col1 = (double *) so_Table_get_column_from_number(table, j);
            PROTECT(col = NEW_NUMERIC(numrows));
            double *ptr = NUMERIC_POINTER(col);
            for (int i = 0; i < numrows; i++) {
                *ptr++ = col1[i];
            }
            SET_ELEMENT(list, j, col);
        } else if (vt == PHARMML_VALUETYPE_INT) {
            int *col1 = (int *) so_Table_get_column_from_number(table, j);
            PROTECT(col = NEW_INTEGER(numrows));
            int *ptr = INTEGER_POINTER(col);
            for (int i = 0; i < numrows; i++) {
                *ptr++ = col1[i];
            }
            SET_ELEMENT(list, j, col);
        } else if (vt == PHARMML_VALUETYPE_STRING) {
            char **col2 = (char **) so_Table_get_column_from_number(table, j);
            PROTECT(col = NEW_STRING(numrows));
            for (int i = 0; i < numrows; i++) {
                SET_STRING_ELT(col, i, mkChar(col2[i]));
            }
            SET_ELEMENT(list, j, col);
        }
    }

    UNPROTECT(5 + numcols);

    return list;
}

so_Matrix *Rmatrix2matrix(SEXP R_matrix)
{
    so_Matrix *matrix = so_Matrix_new();

    if (!matrix) {
        error("Failed to create so_Matrix object");
    }

    SEXP dimnames = getAttrib(R_matrix, R_DimNamesSymbol);
    SEXP rownames = VECTOR_ELT(dimnames, 0);
    SEXP colnames = VECTOR_ELT(dimnames, 1);

    int numrows = length(rownames);
    int numcols = length(colnames);

    if (so_Matrix_set_size(matrix, numrows, numcols) != 0) {
        error("Error in so_Matrix_set_size");
    }

    for (int i = 0; i < numrows; i++) {
        if (so_Matrix_set_RowNames(matrix, i, (char *) CHAR(STRING_ELT(rownames, i))) != 0) {
            error("Error in so_Matrix_set_RowNames");
        }
    }
    for (int i = 0; i < numcols; i++) {
        if (so_Matrix_set_ColumnNames(matrix, i, (char *) CHAR(STRING_ELT(colnames, i))) != 0) {
            error("Error in so_Matrix_set_ColumnNames");
        }
    }

    memcpy(so_Matrix_get_data(matrix), REAL(R_matrix), numrows * numcols * sizeof(double)); 

    return matrix;
}

SEXP matrix2Rmatrix(so_Matrix *matrix)
{
    if (!matrix) {
        return R_NilValue;
    }

    int numrows = so_Matrix_get_number_of_rows(matrix);
    int numcols = so_Matrix_get_number_of_columns(matrix);

    SEXP R_matrix = PROTECT(allocMatrix(REALSXP, numcols, numrows));
    double *ptr = REAL(R_matrix);
    memcpy(ptr, so_Matrix_get_data(matrix), numrows * numcols * sizeof(double));

    SEXP rownames = PROTECT(rownames = NEW_STRING(numrows));
    for (int i = 0; i < numrows; i++) {
        SET_STRING_ELT(rownames, i, mkChar(so_Matrix_get_RowNames(matrix, i)));
    }

    SEXP colnames = PROTECT(colnames = NEW_STRING(numcols));
    for (int i = 0; i < numrows; i++) {
        SET_STRING_ELT(colnames, i, mkChar(so_Matrix_get_ColumnNames(matrix, i)));
    }

    SEXP dimnames = PROTECT(allocVector(VECSXP, 2));
    SET_VECTOR_ELT(dimnames, 0, rownames);
    SET_VECTOR_ELT(dimnames, 1, colnames);
    setAttrib(R_matrix, R_DimNamesSymbol, dimnames);

    UNPROTECT(4);

    return R_matrix;
}
