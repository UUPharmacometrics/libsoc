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

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <libxml/SAX.h>
#include <libxml/xmlwriter.h>

#include <so/Table.h>
#include <so/private/Table.h>
#include <pharmml/common_types.h>
#include <pharmml/string.h>
#include <so/private/column.h>
#include <so/ExternalFile.h>
#include <so/private/ExternalFile.h>

/** \struct so_Table
	 \brief A structure representing a table
*/

/** \memberof so_Table
 * Create a new empty so_Table structure.
 * \return A pointer to the newly created struct or NULL if memory allocation failed
 * \sa so_Table_copy, so_Table_free
 */
so_Table *so_Table_new()
{
    so_Table *table = calloc(sizeof(so_Table), 1);
    if (table) {
        table->reference_count = 1;
    }

    return table;
}

/** \memberof so_Table
 * Create a copy of a so_Table structure. 
 * \return A pointer to the Table copy or NULL if memory allocation failed
 * \sa so_Table_new
 */
so_Table *so_Table_copy(so_Table *source)
{
    so_Table *dest = so_Table_new();
    if (dest) {
        dest->numrows = source->numrows;
        for (int i = 0; i < source->numcols; i++) {
            int fail = so_Table_new_column(dest,
                source->columns[i]->columnId,
                source->columns[i]->columnType,
                source->columns[i]->num_columnType,
                source->columns[i]->valueType,
                source->columns[i]->column);
            if (fail) {
                so_Table_free(dest);
                return NULL;
            }
        } 
    }

    return dest;
}

/** \memberof so_Table
 * Free all memory associated with an so_Table structure
 * without checking the reference count
 * \param self - a pointer to the structure to free
 * \sa so_Table_new, so_Table_unref
 */
void so_Table_free(so_Table *self)
{
    if (self) {
        for (int i = 0; i < self->numcols; i++) {
            so_Column_free(self->columns[i]);
        }
        free(self);
    }
}

/** \memberof so_Table
 * Increase the reference count of an so_Table structure
 * \param self - a pointer to the Table
 * \sa so_Table_unref
 */
void so_Table_ref(so_Table *self)
{
    self->reference_count++;
}

/** \memberof so_Table
 * Decrease the reference count of an so_Table structure.
 * If the reference count reaches zero the so_Table will be freed
 * \param self - a pointer to the Table
 * \sa so_Table_ref, so_Table_free
 */
void so_Table_unref(so_Table *self)
{
    if (self) {
        self->reference_count--;
        if (!self->reference_count) {
            so_Table_free(self);
        }
    }
}

/** \memberof so_Table
 * Set the number of rows in a table
 * \param self - pointer to an so_Table
 * \sa so_Table_get_number_of_rows
 */
void so_Table_set_number_of_rows(so_Table *self, int numrows)
{
   self->numrows = numrows; 
}

/** \memberof so_Table
 * Get the columnId of a specific column
 * \param self - pointer to an so_Table
 * \param index - index of the column
 * \return A pointer to the columnId string
 * \sa so_Table_set_columnId
 */
char *so_Table_get_columnId(so_Table *self, int index)
{
    if (index < 0 || index >= self->numcols) {
        return NULL;
    } else {
        return self->columns[index]->columnId;
    }
}

/** \memberof so_Table
 * Get the columnTypes of a specific column
 * \param self - pointer to an so_Table
 * \param index - index of the column
 * \return Pointer to an array of columnType enums
 * \sa so_Table_set_columnType
 */
pharmml_columnType *so_Table_get_columnType(so_Table *self, int index)
{
    return self->columns[index]->columnType;
}

int so_Table_get_num_columnTypes(so_Table *self, int index)
{
    return self->columns[index]->num_columnType;
}

/** \memberof so_Table
 * Get the valueType of a specific column
 * \param self - pointer to an so_Table
 * \param index - index of the column
 * \return A valueType enum
 * \sa so_Table_set_valueType
 */
pharmml_valueType so_Table_get_valueType(so_Table *self, int index)
{
    return self->columns[index]->valueType;
}

/** \memberof so_Table
 * Set the columnId of a specific column
 * \param self - pointer to an so_Table
 * \param index - index of the column
 * \param columnId - pointer to the columnId string to set
 * \sa so_Table_get_columnId
 */
void so_Table_set_columnId(so_Table *self, int index, char *columnId)
{
    if (index < 0 || index >= self->numcols)
        return;
    
    so_Column_set_columnId(self->columns[index], columnId);
}

/** \memberof so_Table
 * Add a columnType of a specific column
 * \param self - pointer to an so_Table
 * \param index - index of the column
 * \param columnType - the columnType to add
 * \sa so_Table_get_columnType
 */
void so_Table_add_columnType(so_Table *self, int index, pharmml_columnType columnType)
{
    if (index < 0 || index >= self->numcols)
        return;
    
    so_Column_add_columnType(self->columns[index], columnType);
}

/** \memberof so_Table
 * Remove all columnTypes of a specific column
 * \param self - pointer to an so_Table
 * \param index - index of the column
 * \sa so_Table_add_columnType
 */
void so_Table_remove_columnType(so_Table *self, int index)
{
    if (index < 0 || index >= self->numcols)
        return;
    so_Column_remove_columnType(self->columns[index]);
}

/** \memberof so_Table
 * Set the valueType of a specific column
 * \param self - pointer to an so_Table
 * \param index - index of the column
 * \param valueType - the valueType to set
 * \sa so_Table_get_valueType
 */
void so_Table_set_valueType(so_Table *self, int index, pharmml_valueType valueType)
{
    if (index < 0 || index >= self->numcols)
        return;
    
    so_Column_set_valueType(self->columns[index], valueType);
}

/** \memberof so_Table
 * Remove a column from a table
 * \param self - pointer to an so_Table
 * \param index - index of the column to remove
 */
void so_Table_remove_column(so_Table *self, int index)
{
    if (index < 0 || index >= self->numcols) {
        return;
    }

    so_Column_free(self->columns[index]);

    for (int i = index; i < self->numcols - 1; i++) {
        self->columns[i] = self->columns[i + 1];
    }
           
    self->numcols--;
}

/** \memberof so_Table
 * Get the number of columns in a table
 * \param self - pointer to an so_Table
 * \return the number of columns in the table
 */
int so_Table_get_number_of_columns(so_Table *self)
{
    return self->numcols;
}

/** \memberof so_Table
 * Get the number of rows in a table
 * \param self - pointer to an so_Table
 * \return the number of rows in the table
 * \sa so_Table_set_number_of_rows
 */
int so_Table_get_number_of_rows(so_Table *self)
{
    return self->numrows;
}

/** \memberof so_Table
 * Get pointer to column data from a table given the number of the column.
 * \param self - pointer to an so_Table
 * \return pointer to the column data array or NULL if column was not found
 */
void *so_Table_get_column_from_number(so_Table *self, int number)
{
    if (number < 0 || number >= self->numcols) {
        return NULL;
    }

    return self->columns[number]->column;
}

/** \memberof so_Table
 * Get pointer to column data from a table given the columnId of the column.
 * \param self - pointer to an so_Table
 * \return pointer to the column data array or NULL if column was not found.
 */
void *so_Table_get_column_from_name(so_Table *self, char *name)
{
    for (int i = 0; i < self->numcols; i++) {
        if (strcmp(name, self->columns[i]->columnId) == 0) {
            return self->columns[i]->column;
        }
    }

    return NULL;
}

/** \memberof so_Table
 *  Get the index of a column from its columnId
 *  \param self - pointer to an so_Table
 *  \return index of the column or -1 if not found
 */
int so_Table_get_index_from_name(so_Table *self, char *name)
{
    for (int i = 0; i < self->numcols; i++) {
        if (strcmp(name, self->columns[i]->columnId) == 0) {
            return i;
        }
    }

    return -1;
}

/** \memberof so_Table
 * Create a new column and add to table. A new buffer will be created for the data
 * and number_of_rows of the data will be copied.
 * \param self - pointer to an so_Table
 * \param columnId - name of column
 * \param columnType - type of column
 * \param valueType - valueType of column
 * \param data - array of column data
 * \return 0 for success
 * \sa so_Table_new_column_no_copy
 */
int so_Table_new_column(so_Table *self, char *columnId, pharmml_columnType *columnTypes, int num_columnTypes, pharmml_valueType valueType, void *data)
{
    int element_size = pharmml_valueType_to_size(valueType);

    void *buffer = malloc(element_size * self->numrows);
    if (valueType != PHARMML_VALUETYPE_STRING) {
        memcpy(buffer, data, element_size * self->numrows);
    } else {
        int fail = pharmml_copy_string_array((char **) data, (char **) buffer, self->numrows);
        if (fail) {
            free(buffer);
            return fail;
        }
    }

    so_Column *column = so_Column_new();
    if (!column) {
        if (valueType == PHARMML_VALUETYPE_STRING) {
            pharmml_free_string_array(buffer, self->numrows);
        }
        return 1;
    }

    so_Column_set_valueType(column, valueType);
    for (int i = 0; i < num_columnTypes; i++) {
        so_Column_add_columnType(column, columnTypes[i]);
    }
    so_Column_set_columnId(column, columnId);
    column->column = buffer;

    so_Column **column_array = realloc(self->columns, (self->numcols + 1) * sizeof(so_Column *)); 
    if (!column_array) {
        so_Column_free(column);
        return 1;
    }

    self->columns = column_array;
    self->columns[self->numcols] = column;
    self->numcols++;

    return 0;
}

/** \memberof so_Table
 * Create a new column and add to table. Only the pointer to the data will be copied
 * \param self - pointer to an so_Table
 * \param columnId - name of column
 * \param columnType - type of column
 * \param valueType - valueType of column
 * \param data - array of column data
 * \return 0 for success
 * \sa so_Table_new_column
 */
int so_Table_new_column_no_copy(so_Table *self, char *columnId, pharmml_columnType *columnTypes, int num_columnTypes, pharmml_valueType valueType, void *data)
{
    so_Column *column = so_Column_new();
    if (!column) {
        return 1;
    }
    so_Column_set_valueType(column, valueType);
    for (int i = 0; i < num_columnTypes; i++) {
        so_Column_add_columnType(column, columnTypes[i]);
    }
    if (so_Column_set_columnId(column, columnId)) {
        so_Column_free(column);
        return 1;
    }
    column->column = data;
    so_Column **new_columns = realloc(self->columns, (self->numcols + 1) * sizeof(so_Column *));
    if (!new_columns) {
        so_Column_free(column);
        return 1;
    }
    self->columns = new_columns;
    self->columns[self->numcols] = column;
    self->numcols++;
    return 0;
}

/** \memberof so_Table
 * Get column index of the id column
 * \param self - pointer to an so_Table
 * \return index of the id column starting from 0 or -1 if no id column present
 * \sa so_Table_id_column_name
 */
int so_Table_id_column(so_Table *self)
{
    for (int i = 0; i < self->numcols; i++) {
        for (int j = 0; j < self->columns[i]->num_columnType; j++) {
            if (self->columns[i]->columnType[j] == PHARMML_COLTYPE_ID) {
                return i;
            }
        }
    }
    return -1;
}

/** \memberof so_Table
 * Get the columnId of the id column
 * \param self - pointer to an so_Table
 * \return A pointer to the columnId string
 * \sa so_Table_id_column
 */
char *so_Table_id_column_name(so_Table *self)
{
    int index = so_Table_id_column(self);
    if (index != -1) {
        return self->columns[index]->columnId;
    } else {
        return NULL;
    }
}

/** \memberof so_Table
 * Get column index of the idv column
 * \param self - pointer to an so_Table
 * \return index of the idv column starting from 0 or -1 if no idv column present
 */
int so_Table_idv_column(so_Table *self)
{
    for (int i = 0; i < self->numcols; i++) {
        for (int j = 0; j < self->columns[i]->num_columnType; j++) {
            if (self->columns[i]->columnType[j] == PHARMML_COLTYPE_IDV) {
                return i;
            }
        }
    }
    return -1;
}

/** \memberof so_Table
 * Get column index of the dv column
 * \param self - pointer to an so_Table
 * \return index of the dv column starting from 0 or -1 if no dv column present
 */
int so_Table_dv_column(so_Table *self)
{
    for (int i = 0; i < self->numcols; i++) {
        for (int j = 0; j < self->columns[i]->num_columnType; j++) {
            if (self->columns[i]->columnType[j] == PHARMML_COLTYPE_DV) {
                return i;
            }
        }
    }
    return -1;
}

/** \memberof so_Table
 * Get the columnId of the dv column
 * \param self - pointer to an so_Table
 * \return A pointer to the columnId string
 * \sa so_Table_dv_column
 */
char *so_Table_dv_column_name(so_Table *self)
{
    int index = so_Table_dv_column(self);
    if (index != -1) {
        return self->columns[index]->columnId;
    } else {
        return NULL;
    }
}

/** \memberof so_Table
 * Get the columnId of the idv column
 * \param self - pointer to an so_Table
 * \return A pointer to the columnId string
 * \sa so_Table_idv_column
 */
char *so_Table_idv_column_name(so_Table *self)
{
    int index = so_Table_idv_column(self);
    if (index != -1) {
        return self->columns[index]->columnId;
    } else {
        return NULL;
    }
}

so_ExternalFile *so_Table_get_ExternalFile(so_Table *self)
{
    return self->ExternalFile;
}

void so_Table_set_ExternalFile(so_Table *self, so_ExternalFile *value)
{
    so_ExternalFile_unref(self->ExternalFile);
    self->ExternalFile = value;
}

so_ExternalFile *so_Table_create_ExternalFile(so_Table *self)
{
	so_ExternalFile *obj = so_ExternalFile_new();
	if (obj) {
		self->ExternalFile = obj;
	}
	return obj;
}

void so_Table_set_write_external_file(so_Table *self, int write_external_file)
{
    self->write_external_file = write_external_file;
}

int so_Table_xml(so_Table *self, xmlTextWriterPtr writer, char *element_name)
{
    int rc;
    rc = xmlTextWriterStartElement(writer, BAD_CAST element_name);
    if (rc < 0) return 1;

    if (self->superclass_func) {
       rc = (*(self->superclass_func))(self->superclass, writer);
       if (rc != 0) return rc;
    }

    char temp_colnum[10];

    if (self->numcols > 0) {
        rc = xmlTextWriterStartElement(writer, BAD_CAST "ds:Definition");
        if (rc < 0) return 1;
        for (int i = 0; i < self->numcols; i++) {
            rc = xmlTextWriterStartElement(writer, BAD_CAST "ds:Column");
            if (rc < 0) return 1;
            rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "columnId", BAD_CAST self->columns[i]->columnId);
            if (rc < 0) return 1;
            char *columnType_string = pharmml_columnType_array_to_string(self->columns[i]->columnType, self->columns[i]->num_columnType);
            rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "columnType", BAD_CAST columnType_string);
            free(columnType_string);
            if (rc < 0) return 1;
            rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "valueType", BAD_CAST pharmml_valueType_to_string(self->columns[i]->valueType));
            if (rc < 0) return 1;

            snprintf(temp_colnum, 10, "%d", i + 1);
            rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "columnNum", BAD_CAST temp_colnum);
            if (rc < 0) return 1;
            rc = xmlTextWriterEndElement(writer);
            if (rc < 0) return 1;
        }
        rc = xmlTextWriterEndElement(writer);
        if (rc < 0) return 1;
    }

    if (!self->ExternalFile) {
        rc = xmlTextWriterStartElement(writer, BAD_CAST "ds:Table");
        if (rc < 0) return 1;
        for (int i = 0; i < self->numrows; i++) {
            rc = xmlTextWriterStartElement(writer, BAD_CAST "ds:Row");
            for (int j = 0; j < self->numcols; j++) {
                char *value_string;
                char *special_string = NULL;
                if (self->columns[j]->valueType == PHARMML_VALUETYPE_REAL) {
                    double *ptr = (double *) self->columns[j]->column;
                    double number = ptr[i];
                    if (pharmml_is_na(number)) {
                        special_string = "ct:NA";
                    } else if (isnan(number)) {
                        special_string = "ct:NaN";
                    } else if (isinf(number)) {
                        if (number > 0) {
                            special_string = "ct:plusInf";
                        } else {
                            special_string = "ct:minusInf";
                        }
                    } else {
                        value_string = pharmml_double_to_string(number);
                        if (!value_string) return 1;
                    }
                    if (special_string) {
                        rc = xmlTextWriterWriteElement(writer, BAD_CAST special_string, NULL);
                    } else {
                        rc = xmlTextWriterWriteElement(writer, BAD_CAST pharmml_valueType_to_element(self->columns[j]->valueType), BAD_CAST value_string);
                        free(value_string);
                    }
                } else if (self->columns[j]->valueType == PHARMML_VALUETYPE_INT) {
                    int *ptr = (int *) self->columns[j]->column;
                    value_string = pharmml_int_to_string(ptr[i]);
                    if (!value_string) return 1;
                    rc = xmlTextWriterWriteElement(writer, BAD_CAST pharmml_valueType_to_element(self->columns[j]->valueType), BAD_CAST value_string);
                    free(value_string);
                } else if (self->columns[j]->valueType == PHARMML_VALUETYPE_STRING || self->columns[j]->valueType == PHARMML_VALUETYPE_ID) {
                    char **ptr = (char **) self->columns[j]->column;
                    value_string = ptr[i];
                    rc = xmlTextWriterWriteElement(writer, BAD_CAST pharmml_valueType_to_element(self->columns[j]->valueType), BAD_CAST value_string);
                } else if (self->columns[j]->valueType == PHARMML_VALUETYPE_BOOLEAN) {
                    bool *ptr = (bool *) self->columns[j]->column;
                    if (ptr[i]) {
                        rc = xmlTextWriterWriteElement(writer, BAD_CAST "ct:True", NULL);
                    } else {
                        rc = xmlTextWriterWriteElement(writer, BAD_CAST "ct:False", NULL);
                    }
                }
                if (rc < 0) return 1;
            }
            rc = xmlTextWriterEndElement(writer);
            if (rc < 0) return 1;
        }
        rc = xmlTextWriterEndElement(writer);
        if (rc < 0) return 1;
    } else {
        rc = so_ExternalFile_xml(self->ExternalFile, writer, "ds:ExternalFile");
        if (rc) return rc;

        if (self->write_external_file) {
            char *delimiter_string;
            if (strcmp(self->ExternalFile->delimiter, "COMMA") == 0) {
                delimiter_string = ",";
            } else if (strcmp(self->ExternalFile->delimiter, "SPACE") == 0) {
                delimiter_string = " ";
            } else if (strcmp(self->ExternalFile->delimiter, "TAB")  == 0) {
                delimiter_string = "\t";
            } else if (strcmp(self->ExternalFile->delimiter, "SEMICOLON") == 0) {
                delimiter_string = ";";
            }
            FILE *fp = fopen(self->ExternalFile->path, "w");

            for (int i = 0; i < self->numrows; i++) {
                for (int j = 0; j < self->numcols; j++) {
                    char *value_string;
                    if (self->columns[j]->valueType == PHARMML_VALUETYPE_REAL) {
                        double *ptr = (double *) self->columns[j]->column;
                        value_string = pharmml_double_to_string(ptr[i]);
                        if (!value_string) return 1;
                    } else if (self->columns[j]->valueType == PHARMML_VALUETYPE_INT) {
                        int *ptr = (int *) self->columns[j]->column;
                        value_string = pharmml_int_to_string(ptr[i]);
                        if (!value_string) return 1;
                    } else if (self->columns[j]->valueType == PHARMML_VALUETYPE_STRING || self->columns[j]->valueType == PHARMML_VALUETYPE_ID) {
                        char **ptr = (char **) self->columns[j]->column;
                        value_string = ptr[i];
                    } else if (self->columns[j]->valueType == PHARMML_VALUETYPE_BOOLEAN) {
                        bool *ptr = (bool *) self->columns[j]->column;
                        if (ptr[i]) {
                            value_string = "True";
                        } else {
                            value_string = "False";
                        }
                    }

                    fprintf(fp, "%s", value_string);
                    if (j != self->numcols - 1) {
                        fprintf(fp, "%s", delimiter_string);
                    }

                    if (self->columns[j]->valueType == PHARMML_VALUETYPE_REAL || self->columns[j]->valueType == PHARMML_VALUETYPE_INT) {
                        free(value_string);
                    }
                }
                fprintf(fp, "\n");
            }

            fclose(fp);
        }
    }

    rc = xmlTextWriterEndElement(writer);
    if (rc < 0) return 1;

    return 0;
}

int so_Table_start_element(so_Table *table, const char *localname, int nb_attributes, const char **attributes)
{
    if (table->in_externalfile) {
        int fail = so_ExternalFile_start_element(table->ExternalFile, localname, nb_attributes, attributes);
        if (fail) {
            return fail;
        }
    } else if (strcmp("Definition", localname) == 0) {
        table->in_definition = 1;
    } else if (strcmp("Table", localname) == 0) {
        table->in_table = 1;
    } else if (strcmp("ExternalFile", localname) == 0) {
 		so_ExternalFile *ext_file = so_ExternalFile_new();
		if (!ext_file) {
			return 1;
		}
		int fail = so_ExternalFile_init_attributes(ext_file, nb_attributes, attributes);
		if (fail) {
			so_ExternalFile_free(ext_file);
			return 1;
		}
        table->ExternalFile = ext_file;
        table->in_externalfile = 1;
    } else if (table->in_definition && strcmp("Column", localname) == 0) {
        so_Column *col = so_Column_new();
        if (!col) {
            return 1;
        }
        unsigned int index = 0;
        for (int indexAttribute = 0; indexAttribute < nb_attributes; ++indexAttribute, index += 5) {
            const char *localname = attributes[index];
            char *valueBegin = (char *) attributes[index + 3];
            char *valueEnd = (char *) attributes[index + 4];

            if (strcmp(localname, "columnId") == 0) {
                char end_char = *valueEnd;
                *valueEnd = '\0';
                int fail = so_Column_set_columnId(col, valueBegin);
                if (fail) {
                    so_Column_free(col);
                    return fail;
                }
                *valueEnd = end_char;
            } else if (strcmp(localname, "columnType") == 0) {
                char end_char = *valueEnd;
                *valueEnd = '\0';
                so_Column_set_columnType_from_string(col, valueBegin);
                *valueEnd = end_char;
            } else if (strcmp(localname, "valueType") == 0) {
                char end_char = *valueEnd;
                *valueEnd = '\0';
                so_Column_set_valueType_from_string(col, valueBegin);
                *valueEnd = end_char;
            }
        }

        so_Column **new_columns = realloc(table->columns, (table->numcols + 1) * sizeof(so_Column *));
        if (!new_columns) {
            so_Column_free(col);
            return 1;
        }
        table->columns = new_columns;
        table->columns[table->numcols] = col;
        table->numcols++;
    } else if (table->in_table && strcmp("Row", localname) == 0) {
        table->numrows++;
        table->current_column = 0;
        table->in_row = 1;
    } else if (table->in_row && strcmp("Real", localname) == 0) {
        if (!table->defer_reading) {
            table->in_real = 1;
            table->current_column++;
        }
    } else if (table->in_row && strcmp("Int", localname) == 0) {
        if (!table->defer_reading) {
            table->in_int = 1;
            table->current_column++;
        }
    } else if ((table->in_row && (strcmp("String", localname) == 0)) || (strcmp("Id", localname) == 0)) {
        if (!table->defer_reading) {
            table->in_string = 1;
            table->current_column++;
        }
    } else if (table->in_row && strcmp("True", localname) == 0) {
        if (!table->defer_reading) {
            so_Column *column = table->columns[table->current_column];
            int fail = so_Column_add_boolean(column, 1);
            if (fail) {
                return 1;
            }
            table->current_column++;
        }
    } else if (table->in_row && strcmp("False", localname) == 0) {
        if (!table->defer_reading) {
            so_Column *column = table->columns[table->current_column];
            int fail = so_Column_add_boolean(column, 0);
            if (fail) {
                return 1;
            }
            table->current_column++;
        }
    } else if (table->in_row && strcmp("plusInf", localname) == 0) {
        so_Column *column = table->columns[table->current_column];
        table->current_column++;
        int fail = so_Column_add_real(column, INFINITY);
        if (fail) {
            return 1;
        }
    } else if (table->in_row && strcmp("minusInf", localname) == 0) {
        so_Column *column = table->columns[table->current_column];
        table->current_column++;
        int fail = so_Column_add_real(column, -INFINITY);
        if (fail) {
            return 1;
        }
    } else if (table->in_row && strcmp("NA", localname) == 0) {
        so_Column *column = table->columns[table->current_column];
        table->current_column++;
        int fail = so_Column_add_real(column, pharmml_na());
        if (fail) {
            return 1;
        }
    } else if (table->in_row && strcmp("NaN", localname) == 0) {
        so_Column *column = table->columns[table->current_column];
        table->current_column++;
        int fail = so_Column_add_real(column, pharmml_nan());
        if (fail) {
            return 1;
        }
    }

    return 0;
}

void so_Table_end_element(so_Table *table, const char *localname)
{
    if (strcmp(localname, "ExternalFile") == 0 && table->in_externalfile) {
       table->in_externalfile = 0; 
    } else if (table->in_externalfile) {
        so_ExternalFile_end_element(table->ExternalFile, localname);
    } else if (strcmp("Definition", localname) == 0) {
        table->in_definition = 0;
    } else if (strcmp("Table", localname) == 0) {
        table->in_table = 0;
    } else if (strcmp("Row", localname) == 0) {
        table->in_row = 0;
    } else if (strcmp("Real", localname) == 0) {
        table->in_real = 0;
    } else if (strcmp("Int", localname) == 0) {
        table->in_int = 0;
    } else if ((strcmp("String", localname) == 0) || (strcmp("Id", localname) == 0)) {
        table->in_string = 0;
    }
}

int so_Table_characters(so_Table *table, const char *ch, int len)
{
    int fail = 0;

    if (table->in_externalfile) {
		fail = so_ExternalFile_characters(table->ExternalFile, ch, len);
		if (fail) return 1;
    }

    so_Column *column;

    char *str = (char *) ch;
    char saved = str[len];

    if (table->in_real || table->in_int || table->in_string) {
        if (table->current_column - 1 >= table->numcols) {   // Too many columns in this SO
            return 1;
        }
        column = table->columns[table->current_column - 1];
        str[len] = '\0';

        if (table->in_real) {
            double real = pharmml_string_to_double(str);
            fail = so_Column_add_real(column, real);
        } else if (table->in_int) {
            int integer = pharmml_string_to_int(str);
            fail = so_Column_add_int(column, integer);
        } else if (table->in_string) {
            fail = so_Column_add_string(column, str);
        }

        str[len] = saved;

    }

    return fail;
}
