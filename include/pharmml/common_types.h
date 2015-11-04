#ifndef _PHARMML_COMMON_TYPES_H
#define _PHARMML_COMMON_TYPES_H

typedef enum { PHARMML_COLTYPE_UNDEFINED, PHARMML_COLTYPE_ID, PHARMML_COLTYPE_TIME } pharmml_columnType;
typedef enum { PHARMML_VALUETYPE_REAL, PHARMML_VALUETYPE_INT, PHARMML_VALUETYPE_STRING, PHARMML_VALUETYPE_ID, PHARMML_VALUETYPE_BOOLEAN } pharmml_valueType;

const char *pharmml_columnType_to_string(pharmml_columnType coltype);
pharmml_columnType pharmml_string_to_columnType(const char *str);
const char *pharmml_valueType_to_string(pharmml_valueType valtype);
const char *pharmml_valueType_to_element(pharmml_valueType valtype);
pharmml_valueType pharmml_string_to_valueType(const char *str);
int pharmml_valueType_to_size(pharmml_valueType valtype);

double pharmml_string_to_double(const char *str);
char *pharmml_double_to_string(double x);
int pharmml_string_to_int(const char *str);
char *pharmml_int_to_string(int x);

#endif
