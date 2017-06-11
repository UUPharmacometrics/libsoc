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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <pharmml/common_types.h>

#define NUMCOLTYPES 44
static char *coltypenames[] = {
    "undefined", "addl", "adm", "arm", "censoring", "cmt", "covariate", "date", "dat1", "dat2", "dat3", "dose", "duration", "dv", "dvid", "epoch",
    "evid", "id", "idv", "ii", "limit", "mdv", "occasion", "rate", "reg", "replicate", "ss", "ssEndTime", "ssPeriod", "time", "varLevel", "ytype",
    "indivParameter", "popParameter", "randEffect", "residual", "statPrecision", "strataVariable", "structParameter",
    "varParameter", "variance", "stdev", "covariance", "correlation",
};

#define NUMVALTYPES 5
static const char *valtypenames[] = { "real", "int", "string", "id", "boolean" }; 
static const char *valtypeelements[] = { "ct:Real", "ct:Int", "ct:String", "ct:Id", NULL }; 


const char *pharmml_columnType_to_string(pharmml_columnType coltype)
{
    if (coltype < NUMCOLTYPES) {
        return coltypenames[coltype];
    } else {
        return coltypenames[0];
    }
}

pharmml_columnType pharmml_string_to_columnType(const char *str)
{
    for (int i = 0; i < NUMCOLTYPES; i++) {
        if (strcmp(str, coltypenames[i]) == 0) {
            return i;
        }
    }

    return PHARMML_COLTYPE_UNDEFINED;
}

char *pharmml_columnType_array_to_string(pharmml_columnType *colTypes, int num_columnTypes)
{
    char *string;
    if (num_columnTypes == 0) {
        string = malloc(10);
        if (!string)
            return NULL;
        strcpy(string, coltypenames[0]);
    } else {
        const char *first_columnType_string = pharmml_columnType_to_string(colTypes[0]);
        string = malloc(strlen(first_columnType_string) + 1);
        if (!string)
            return NULL;
        strcpy(string, first_columnType_string);
        for (int i = 1; i < num_columnTypes; i++) {
            const char *substr = pharmml_columnType_to_string(colTypes[i]);
            char *new_string = realloc(string, strlen(string) + strlen(substr) + 2);
            if (!new_string) {
                free(string);
                return NULL;
            }
            string = new_string;
            strcat(string, " ");
            strcat(string, substr);
        }
    }
    return string;
}

const char *pharmml_valueType_to_string(pharmml_valueType valtype)
{
    if (valtype < NUMVALTYPES) {
        return valtypenames[valtype];
    } else {
        return NULL;
    }
}

const char *pharmml_valueType_to_element(pharmml_valueType valtype)
{
    if (valtype < NUMVALTYPES) {
        return valtypeelements[valtype];
    } else {
        return "";
    }
}

pharmml_valueType pharmml_string_to_valueType(const char *str)
{
    for (int i = 0; i < NUMVALTYPES; i++) {
        if (strcmp(str, valtypenames[i]) == 0) {
            return i;
        }
    }

    return PHARMML_VALUETYPE_ERROR;
}

int pharmml_valueType_to_size(pharmml_valueType valtype)
{
    if (valtype == PHARMML_VALUETYPE_REAL) {
        return sizeof(double);
    } else if (valtype == PHARMML_VALUETYPE_INT) {
        return sizeof(int);
    } else if (valtype == PHARMML_VALUETYPE_STRING || valtype == PHARMML_VALUETYPE_ID) {
        return sizeof(char *);
    } else if (valtype == PHARMML_VALUETYPE_BOOLEAN) {
        return sizeof(bool);
    }
    return sizeof(char *);   // Emergency fallback
}

#define PHARMML_NA 0x7FF00000000007A2ULL 
#define PHARMML_NAN 0x7FF0000000000001ULL 

union pharmml_double {
    double x;
    uint64_t y;
};

double pharmml_na()
{
    union pharmml_double my_double;

    my_double.y = PHARMML_NA; 

    return my_double.x;
}

int pharmml_is_na(double x)
{
    union pharmml_double my_double;
    my_double.x = x;

    return my_double.y == PHARMML_NA;
}

double pharmml_nan()
{
    union pharmml_double my_double;

    my_double.y = PHARMML_NAN;

    return my_double.x;
}
