#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <pharmml/common_types.h>

void main()
{
    assert(strcmp(pharmml_columnType_to_string(0), "undefined") == 0);
    assert(strcmp(pharmml_columnType_to_string(PHARMML_COLTYPE_STRUCT_PARAMETER), "structParameter") == 0);
    assert(strcmp(pharmml_columnType_to_string(228), "undefined") == 0);

    assert(pharmml_string_to_columnType("undefined") == 0);
    assert(pharmml_string_to_columnType("varParameter_var") == PHARMML_COLTYPE_VAR_PARAMETER_VAR);
    assert(pharmml_string_to_columnType("nonsenseparameter") == PHARMML_COLTYPE_UNDEFINED);

    assert(strcmp(pharmml_valueType_to_string(PHARMML_VALUETYPE_STRING), "string") == 0);
    assert(strcmp(pharmml_valueType_to_string(PHARMML_VALUETYPE_REAL), "real") == 0);
    assert(pharmml_valueType_to_string(228) == NULL);

    assert(strcmp(pharmml_valueType_to_element(PHARMML_VALUETYPE_STRING), "ct:String") == 0);
    assert(strcmp(pharmml_valueType_to_element(PHARMML_VALUETYPE_INT), "ct:Int") == 0);

    assert(pharmml_string_to_valueType("id") == PHARMML_VALUETYPE_ID);
    assert(pharmml_string_to_valueType("int") == PHARMML_VALUETYPE_INT);
    assert(pharmml_string_to_valueType("nonsensetype") == PHARMML_VALUETYPE_ERROR);

    assert(pharmml_valueType_to_size(PHARMML_VALUETYPE_ID) == sizeof(char *));

    printf("common_types PASS\n");
}
