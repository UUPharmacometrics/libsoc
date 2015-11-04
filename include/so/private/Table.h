#ifndef _SO_PRIVATE_TABLE_H
#define _SO_PRIVATE_TABLE_H

#include <so/Table.h>
#include <so/xml.h>
#include <so/private/column.h>

struct so_Table {
    char *name;
    so_Column **columns;
    char *oid;
    char *path;
    pharmml_delimiter delimiter; 
    int use_external_file;
    int numcols;
    int numrows;
    int current_column;
    int defer_reading;
    int in_definition;
    int in_table;
    int in_row;
    int in_real;
    int in_int;
    int in_string;
    int reference_count;
};

so_xml so_Table_xml(so_Table *self);
void so_Table_start_element(so_Table *table, const char *localname, int nb_attributes, const char **attributes);
void so_Table_end_element(so_Table *table, const char *localname);
void so_Table_characters(so_Table *table, const char *ch, int len);

#endif
