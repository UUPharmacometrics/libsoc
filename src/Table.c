/* libsoc - Library to handle standardised output files
 * Copyright (C) 2015 Rikard Nordgren
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
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
#include <libxml/SAX.h>
#include <libxml/tree.h>

#include <so/Table.h>
#include <so/private/Table.h>
#include <so/private/util.h>
#include <pharmml/common_types.h>
#include <so/private/column.h>

/** \struct so_Table
	 \brief A structure representing a table
*/

/** \memberof so_Table
 * Create a new empty so_Table structure.
 * \return A pointer to the newly created struct
 * \sa so_Table_free
 */
so_Table *so_Table_new(char *name)
{
    so_Table *table = extcalloc(sizeof(so_Table));
    table->name = extstrdup(name);
    table->reference_count = 1;

    return table;
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
        free(self->name);
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
 * Get the columnType of a specific column
 * \param self - pointer to an so_Table
 * \param index - index of the column
 * \return A columnType enum
 * \sa so_Table_set_columnType
 */
pharmml_columnType so_Table_get_columnType(so_Table *self, int index)
{
    return self->columns[index]->columnType;
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
 * Set the columnType of a specific column
 * \param self - pointer to an so_Table
 * \param index - index of the column
 * \param columnType - the columnType to set
 * \sa so_Table_get_columnType
 */
void so_Table_set_columnType(so_Table *self, int index, pharmml_columnType columnType)
{
    if (index < 0 || index >= self->numcols)
        return;
    
    so_Column_set_columnType(self->columns[index], columnType);
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
 * Create a new column and add to table. A new buffer will be created for the data
 * and number_of_rows of the data will be copied.
 * \param self - pointer to an so_Table
 * \param columnId - name of column
 * \param columnType - type of column
 * \param valueType - valueType of column
 * \param data - array of column data
 * \sa so_Table_new_column_no_copy
 */
void so_Table_new_column(so_Table *self, char *columnId, pharmml_columnType columnType, pharmml_valueType valueType, void *data)
{
    int element_size = pharmml_valueType_to_size(valueType);

    void *buffer = extmalloc(element_size * self->numrows);
    if (valueType != PHARMML_VALUETYPE_STRING) {
        memcpy(buffer, data, element_size * self->numrows);
    } else {
        char **string_buffer = (char **) buffer;
        char **source = (char **) data;
        for (int i = 0; i < self->numrows; i++) {
            string_buffer[i] = extstrdup(source[i]);
        }
    }

    so_Column *column = so_Column_new();
    so_Column_set_valueType(column, valueType);
    so_Column_set_columnType(column, columnType);
    so_Column_set_columnId(column, columnId);
    column->column = buffer;
    self->numcols++;
    self->columns = extrealloc(self->columns, self->numcols * sizeof(so_Column *));
    self->columns[self->numcols - 1] = column;
}

/** \memberof so_Table
 * Create a new column and add to table. Only the pointer to the data will be copied
 * \param self - pointer to an so_Table
 * \param columnId - name of column
 * \param columnType - type of column
 * \param valueType - valueType of column
 * \param data - array of column data
 * \sa so_Table_new_column
 */
void so_Table_new_column_no_copy(so_Table *self, char *columnId, pharmml_columnType columnType, pharmml_valueType valueType, void *data)
{
    so_Column *column = so_Column_new();
    so_Column_set_valueType(column, valueType);
    so_Column_set_columnType(column, columnType);
    so_Column_set_columnId(column, columnId);
    column->column = data;
    self->numcols++;
    self->columns = extrealloc(self->columns, self->numcols * sizeof(so_Column *));
    self->columns[self->numcols - 1] = column;
}

void so_Table_use_external_file(so_Table *self)
{
    self->use_external_file = 1;
}

void so_Table_no_external_file(so_Table *self)
{
    self->use_external_file = 0;
    free(self->oid);
    self->oid = NULL;
    free(self->path);
    self->path = NULL;
}

pharmml_delimiter so_Table_get_external_file_delimiter(so_Table *self)
{
    return self->delimiter;
}

void so_Table_set_external_file_delimiter(so_Table *self, pharmml_delimiter delimiter)
{
    self->delimiter = delimiter;
}

char *so_Table_get_external_file_oid(so_Table *self)
{
    return self->oid;
}

void so_Table_set_external_file_oid(so_Table *self, char *oid)
{
    self->oid = extstrdup(oid);
}

char *so_Table_get_external_file_path(so_Table *self)
{
    return self->path;
}

void so_Table_set_external_file_path(so_Table *self, char *path)
{
    self->path = extstrdup(path);
}

so_xml so_Table_xml(so_Table *self)
{
    xmlNodePtr xml = xmlNewNode(NULL, BAD_CAST self->name);

    xmlNodePtr def = xmlNewChild(xml, NULL, BAD_CAST "ds:Definition", NULL);

    char temp_colnum[10];

    for (int i = 0; i < self->numcols; i++) {
        xmlNodePtr col = xmlNewChild(def, NULL, BAD_CAST "ds:Column", NULL);
        xmlNewProp(col, BAD_CAST "columnId", BAD_CAST self->columns[i]->columnId);
        xmlNewProp(col, BAD_CAST "columnType", BAD_CAST pharmml_columnType_to_string(self->columns[i]->columnType));
        xmlNewProp(col, BAD_CAST "valueType", BAD_CAST pharmml_valueType_to_string(self->columns[i]->valueType));
        snprintf(temp_colnum, 10, "%d", i + 1);
        xmlNewProp(col, BAD_CAST "columnNum", BAD_CAST temp_colnum);
    }

    if (!self->use_external_file) {
        xmlNodePtr tab = xmlNewChild(xml, NULL, BAD_CAST "ds:Table", NULL);

        for (int i = 0; i < self->numrows; i++) {
            xmlNodePtr row = xmlNewChild(tab, NULL, BAD_CAST "ds:Row", NULL);
            for (int j = 0; j < self->numcols; j++) {
                char *value_string;
                if (self->columns[j]->valueType == PHARMML_VALUETYPE_REAL) {
                    double *ptr = (double *) self->columns[j]->column;
                    value_string = pharmml_double_to_string(ptr[i]);
                } else if (self->columns[j]->valueType == PHARMML_VALUETYPE_INT) {
                    int *ptr = (int *) self->columns[j]->column;
                    value_string = pharmml_int_to_string(ptr[i]);
                } else if (self->columns[j]->valueType == PHARMML_VALUETYPE_STRING || self->columns[j]->valueType == PHARMML_VALUETYPE_ID) {
                    char **ptr = (char **) self->columns[j]->column;
                    value_string = ptr[i];
                }
                xmlNewChild(row, NULL, BAD_CAST pharmml_valueType_to_element(self->columns[j]->valueType), BAD_CAST value_string);
                if (self->columns[j]->valueType == PHARMML_VALUETYPE_REAL || self->columns[j]->valueType == PHARMML_VALUETYPE_INT) {
                    free(value_string);
                }
            }
        }
    } else {
        xmlNodePtr ext = xmlNewChild(xml, NULL, BAD_CAST "ds:ExternalFile", NULL);
        xmlNewProp(ext, BAD_CAST "oid", BAD_CAST self->oid);
       
        xmlNewChild(ext, NULL, BAD_CAST "ds:path", BAD_CAST self->path);
        xmlNewChild(ext, NULL, BAD_CAST "ds:format", BAD_CAST "CSV");
        char *delimiter;
        char *delimiter_string;
        if (self->delimiter == PHARMML_DELIMITER_COMMA) {
            delimiter = "COMMA";
            delimiter_string = ",";
        } else if (self->delimiter == PHARMML_DELIMITER_SPACE) {
            delimiter = "SPACE";
            delimiter_string = " ";
        } else if (self->delimiter == PHARMML_DELIMITER_TAB) {
            delimiter = "TAB";
            delimiter_string = "\t";
        } else if (self->delimiter == PHARMML_DELIMITER_SEMICOLON) {
            delimiter = "SEMICOLON";
            delimiter_string = ";";
        }
        xmlNewChild(ext, NULL, BAD_CAST "ds:delimiter", BAD_CAST delimiter);

        FILE *fp = fopen(self->path, "w");

        for (int i = 0; i < self->numrows; i++) {
            for (int j = 0; j < self->numcols; j++) {
                char *value_string;
                if (self->columns[j]->valueType == PHARMML_VALUETYPE_REAL) {
                    double *ptr = (double *) self->columns[j]->column;
                    value_string = pharmml_double_to_string(ptr[i]);
                } else if (self->columns[j]->valueType == PHARMML_VALUETYPE_INT) {
                    int *ptr = (int *) self->columns[j]->column;
                    value_string = pharmml_int_to_string(ptr[i]);
                } else if (self->columns[j]->valueType == PHARMML_VALUETYPE_STRING || self->columns[j]->valueType == PHARMML_VALUETYPE_ID) {
                    char **ptr = (char **) self->columns[j]->column;
                    value_string = ptr[i];
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

    return xml;
}

void so_Table_start_element(so_Table *table, const char *localname, int nb_attributes, const char **attributes)
{
    if (strcmp("Definition", localname) == 0) {
        table->in_definition = 1;
    } else if (strcmp("Table", localname) == 0) {
        table->in_table = 1;
    } else if (table->in_definition && strcmp("Column", localname) == 0) {
        so_Column *col = so_Column_new();
        unsigned int index = 0;
        for (int indexAttribute = 0; indexAttribute < nb_attributes; ++indexAttribute, index += 5) {
            const char *localname = attributes[index];
            const char *prefix = attributes[index + 1];
            const char *nsURI = attributes[index + 2];
            char *valueBegin = (char *) attributes[index + 3];
            char *valueEnd = (char *) attributes[index + 4];

            if (strcmp(localname, "columnId") == 0) {
                char end_char = *valueEnd;
                *valueEnd = '\0';
                so_Column_set_columnId(col, valueBegin);
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

        table->numcols++;
        table->columns = extrealloc(table->columns, table->numcols * sizeof(so_Column *));
        table->columns[table->numcols - 1] = col;
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
    } else if (table->in_row && (strcmp("String", localname) == 0) || (strcmp("Id", localname) == 0)) {
        if (!table->defer_reading) {
            table->in_string = 1;
            table->current_column++;
        }
    }
}

void so_Table_end_element(so_Table *table, const char *localname)
{
    if (strcmp("Definition", localname) == 0) {
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

void so_Table_characters(so_Table *table, const char *ch, int len)
{
    so_Column *column;

    char *str = (char *) ch;
    char saved = str[len];

    if (table->in_real || table->in_int || table->in_string) {
        column = table->columns[table->current_column - 1];
        str[len] = '\0';
    }

    if (table->in_real) {
        double real = pharmml_string_to_double(str);
        so_Column_add_real(column, real);
    } else if (table->in_int) {
        int integer = pharmml_string_to_int(str);
        so_Column_add_int(column, integer);
    } else if (table->in_string) {
        so_Column_add_string(column, str);
    }

    if (table->in_real || table->in_int || table->in_string) {
        str[len] = saved;
    }
}
