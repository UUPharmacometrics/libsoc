#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <pharmml/common_types.h>

void main()
{
    assert(strcmp(pharmml_columnType_to_string(0), "undefined") == 0);

    printf("common_types PASS\n");
}
