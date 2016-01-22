#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pharmml/string.h>

void main()
{
    char *source = "my string";
    char *dest;

    assert(pharmml_string_to_double("3.14") == 3.14);

    dest = pharmml_double_to_string(6.29);
    assert((strcmp(dest, "6.290000") == 0) && "pharmml_double_to_string");
    free(dest);

    assert(pharmml_string_to_int("56") == 56);

    dest = pharmml_int_to_string(28);
    assert((strcmp(dest, "28") == 0) && "pharmml_int_to_string");
    free(dest);

    dest = pharmml_strdup(source);
    assert((dest != source) && "pharmml_strdup different buffers");
    assert((strcmp(source, dest) == 0) && "pharmml_strdup compare strings");
    free(dest);

    dest = pharmml_strndup(source, 4);
    assert((dest != source) && "pharmml_strndup different buffers");
    assert((strcmp("my s", dest) == 0) && "pharmml_strndup compare strings");
    free(dest);

    printf("string PASS\n");
}
