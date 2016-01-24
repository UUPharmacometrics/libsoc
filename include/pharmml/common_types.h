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

#ifndef _PHARMML_COMMON_TYPES_H
#define _PHARMML_COMMON_TYPES_H

#include <stdlib.h>

typedef enum {
    PHARMML_COLTYPE_UNDEFINED, PHARMML_COLTYPE_ADDL, PHARMML_COLTYPE_ADM, PHARMML_COLTYPE_ARM, PHARMML_COLTYPE_CENSORING, PHARMML_COLTYPE_CMT,
    PHARMML_COLTYPE_COVARIATE, PHARMML_COLTYPE_DOSE, PHARMML_COLTYPE_DURATION, PHARMML_COLTYPE_DV,
    PHARMML_COLTYPE_DVID, PHARMML_COLTYPE_EPOCH, PHARMML_COLTYPE_EVID, PHARMML_COLTYPE_ID, PHARMML_COLTYPE_IDV, PHARMML_COLTYPE_II,
    PHARMML_COLTYPE_LIMIT, PHARMML_COLTYPE_MDV, PHARMML_COLTYPE_OCCASION, PHARMML_COLTYPE_RATE, PHARMML_COLTYPE_REG,
    PHARMML_COLTYPE_REPLICATE, PHARMML_COLTYPE_SS, PHARMML_COLTYPE_SSENDTIME, PHARMML_COLTYPE_SSPERIOD, PHARMML_COLTYPE_TIME,
    PHARMML_COLTYPE_INDIV_PARAMETER, PHARMML_COLTYPE_POP_PARAMETER, PHARMML_COLTYPE_RAND_EFFECT, PHARMML_COLTYPE_RESIDUAL,
    PHARMML_COLTYPE_STAT_PRECISION, PHARMML_COLTYPE_STRATA_VARIABLE, PHARMML_COLTYPE_STRUCT_PARAMETER, PHARMML_COLTYPE_VAR_PARAMETER_VAR,
    PHARMML_COLTYPE_VAR_PARAMETER_STDEV, PHARMML_COLTYPE_VAR_PARAMETER_COV, PHARMML_COLTYPE_VAR_PARAMETER_CORR
} pharmml_columnType;

typedef enum { PHARMML_VALUETYPE_REAL, PHARMML_VALUETYPE_INT, PHARMML_VALUETYPE_STRING, PHARMML_VALUETYPE_ID, PHARMML_VALUETYPE_BOOLEAN, PHARMML_VALUETYPE_ERROR } pharmml_valueType;

const char *pharmml_columnType_to_string(pharmml_columnType coltype);
pharmml_columnType pharmml_string_to_columnType(const char *str);
const char *pharmml_valueType_to_string(pharmml_valueType valtype);
const char *pharmml_valueType_to_element(pharmml_valueType valtype);
pharmml_valueType pharmml_string_to_valueType(const char *str);
int pharmml_valueType_to_size(pharmml_valueType valtype);
double pharmml_na();
int pharmml_is_na(double x);
double pharmml_nan();

#endif
