#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <so.h>

void main()
{
    so_SO *so = so_SO_read("data/table1.SO.xml");    

    assert(so_SO_get_number_of_SOBlock(so) == 1);

    so_SOBlock *soblock = so_SO_get_SOBlock(so, 0);
    so_Estimation *est = so_SOBlock_get_Estimation(soblock);
    so_Table *table = so_Estimation_get_Predictions(est);

    assert(so_Table_get_number_of_columns(table) == 4);
    assert(so_Table_get_number_of_rows(table) == 3);

    assert(strcmp(so_Table_get_columnId(table, 0), "ID") == 0);
    assert(strcmp(so_Table_get_columnId(table, 1), "TIME") == 0);
    assert(strcmp(so_Table_get_columnId(table, 2), "PRED") == 0);
    assert(strcmp(so_Table_get_columnId(table, 3), "IPRED") == 0);
    assert(so_Table_get_columnId(table, 4) == NULL);

    assert(so_Table_get_columnType(table, 0) == PHARMML_COLTYPE_ID);
    assert(so_Table_get_columnType(table, 1) == PHARMML_COLTYPE_UNDEFINED);
    assert(so_Table_get_columnType(table, 2) == 0);
    assert(so_Table_get_columnType(table, 3) == PHARMML_COLTYPE_UNDEFINED);

    assert(so_Table_get_valueType(table, 0) == PHARMML_VALUETYPE_STRING);
    assert(so_Table_get_valueType(table, 1) == PHARMML_VALUETYPE_REAL);
    assert(so_Table_get_valueType(table, 2) == PHARMML_VALUETYPE_REAL);
    assert(so_Table_get_valueType(table, 3) == PHARMML_VALUETYPE_REAL);

    char **ids = (char **) so_Table_get_column_from_number(table, 0);
    assert(strcmp(ids[0], "58") == 0);
    assert(strcmp(ids[1], "59") == 0);
    assert(strcmp(ids[2], "60") == 0);

    double *time = (double *) so_Table_get_column_from_name(table, "TIME");
    assert(time[0] == 60.3);
    assert(time[1] == 72.3);
    assert(time[2] == 73.8);

    printf("table PASS\n");
}
