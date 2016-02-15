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

#ifndef _SO_PRIVATE_TABLE_H
#define _SO_PRIVATE_TABLE_H

#include <so/Table.h>
#include <so/private/column.h>
#include <so/ExternalFile.h>
#include <libxml/xmlwriter.h>

struct so_Table {
    so_Column **columns;
    so_ExternalFile *ExternalFile;
    int (*superclass_func)(void *, xmlTextWriterPtr writer);
    void *superclass;
    int write_external_file;
    int numcols;
    int numrows;
    int current_column;
    int defer_reading;
    int in_definition;
    int in_table;
    int in_externalfile;
    int in_row;
    int in_real;
    int in_int;
    int in_string;
    int reference_count;
};

int so_Table_xml(so_Table *self, xmlTextWriterPtr writer, char *element_name);
int so_Table_start_element(so_Table *table, const char *localname, int nb_attributes, const char **attributes);
void so_Table_end_element(so_Table *table, const char *localname);
int so_Table_characters(so_Table *table, const char *ch, int len);

#endif
